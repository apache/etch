using System;
using System.IO;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using Etch.Support;
using Etch.Util;

namespace Etch.Transport
{
    /// <summary>
    /// Implementation of connection which handles a tcp connection.
    /// </summary>
    public class TcpConnection : Connection<DataHandler>, DataSource  
    {
        /// <summary>
        /// Constructs the Connection using an existing socket. No attempt
        /// is made to reconnect if the connection goes down.
        /// </summary>
        /// <param name="handler">target to receive data events.</param>
        /// <param name="socket">existing connected socket (e.g., from Listener).</param>
        public TcpConnection( DataHandler handler, Socket socket ) : base (handler)
        {
            if ( socket == null )
                throw new ArgumentException( "socket == null" );

            this.socket = socket;
            this.host = null;
            this.port = 0;
            this.delay = 0;
        }


        /// <summary>
        /// Constructs the Connection. If delay is greater than 0, an attempt
        /// will be made to reconnect a broken connection, or retry a
        /// failed connection attempt.
        /// </summary>
        /// <param name="handler">target to receive data events.</param>
        /// <param name="host">address to connect to. Host cannot be null.</param>
        /// <param name="port">port to connect to. Port must be > 0 & < 65536.</param>
        /// <param name="delay">how long in milliseconds to wait before retrying a
        /// failure. Delay must be >= 0. Delay of 0 means do not retry.</param>

        public TcpConnection( DataHandler handler, String host, int port,
		    int delay ) : base (handler)
        {
            if ( host == null )
                throw new ArgumentException( "host == null" );

            if ( port <= 0 || port >= 65536 )
                throw new ArgumentException( "port <= 0 || port >= 65536 " );

            if ( delay < 0 )
                throw new ArgumentException( "delay < 0" );

            this.socket = null;
            this.host = host;
            this.port = port;
            this.delay = delay;
        }

        /// <summary>
        /// Term on the uri which specifies the reconnect delay.
        /// </summary>
        public readonly static String RECONNECT_DELAY = "TcpConnection.reconnect_delay";

        /// <summary>
        /// Term on the uri which specifies the default auto flush flag.
        /// </summary>
        private static readonly String AUTO_FLUSH = "TcpConnection.autoFlush";

        /// <summary>
        /// Term on the uri which specifies the default keep alive flag.
        /// </summary>
        private static readonly String KEEP_ALIVE = "TcpConnection.keepAlive";

        /// <summary>
        /// Term on the uri which specifies the default linger time.
        /// </summary>
        private static readonly String LINGER_TIME = "TcpConnection.lingerTime";

        /// <summary>
        /// Term on the uri which specifies the default no delay flag.
        /// </summary>
        private static readonly String NO_DELAY = "TcpConnection.noDelay";

        /// <summary>
        /// Term on the uri which specifies the default traffic class.
        /// </summary>
        private static readonly String TRAFFIC_CLASS = "TcpConnection.trafficClass";

        /// <summary>
        /// Term on the uri which specifies the default buffer size parameter.
        /// </summary>
        private static readonly String BUFFER_SIZE = "TcpConnection.bufferSize";

        public TcpConnection( URL uri, Resources resources, Socket socket ) : base( null )
        {
            if ( socket == null )
            {
                String host = uri.Host;
                if ( host == null )
                    throw new ArgumentNullException( "host == null" );

                int? port = uri.Port;
                if ( port == null )
                    throw new ArgumentNullException( "port == null" );

                if ( port <= 0 || port >= 65536 )
                    throw new ArgumentOutOfRangeException( "port <= 0 || port >= 65536" );

                int reconnectDelay = (int) uri.GetIntegerTerm( RECONNECT_DELAY, 0 );
                if ( reconnectDelay < 0 )
                    throw new ArgumentOutOfRangeException( "delay<0" );

                this.socket = null;
                this.host = host;
                this.port = (int)port;
                this.delay = reconnectDelay;
            }
            else
            {
                this.socket = socket;
                this.host = null;
                this.port = 0;
                this.delay = 0;
            }

            bool? defAutoFlush = uri.GetBooleanTerm( AUTO_FLUSH );
            if ( defAutoFlush != null )
                SetDefaultAutoFlush( (bool)defAutoFlush );

            bool? defKeepAlive = uri.GetBooleanTerm( KEEP_ALIVE );
            if ( defKeepAlive != null )
                SetDefaultKeepAlive( (bool)defKeepAlive );

            int? defLingerTime = uri.GetIntegerTerm( LINGER_TIME );
            if ( defLingerTime != null )
                SetDefaultLingerTime( (int)defLingerTime );

            bool? defNoDelay = uri.GetBooleanTerm( NO_DELAY );
            if ( defNoDelay != null )
                SetDefaultNoDelay( (bool)defNoDelay );

            int? defTrafficClass = uri.GetIntegerTerm( TRAFFIC_CLASS );
            if ( defTrafficClass != null )
                SetDefaultTrafficClass( (int)defTrafficClass );

            int? defBufferSize = uri.GetIntegerTerm( BUFFER_SIZE );
            if ( defBufferSize != null )
                SetDefaultBufferSize( (int)defBufferSize );
        }

        private Socket socket;
	
	    private String host;
    	
	    private int port;
    	
	    private int delay;

        public override string ToString()
        {

            if ( socket != null )
                return String.Format( "TCP (up, {0})", socket.LocalEndPoint );

            return String.Format("TCP (down, {0}, {1})", host, port);
        }

        /// <summary>
        /// Sets the default value of the keepAlive flag. Note that this is a
        /// tcp keep alive, which is not universally supported and claimed by
        /// some to slow down connection performance. Also, tcp keep alive only
        /// sends a protocol level packet every 2 hrs if there is no other traffic,
        /// which means you won't really get timely notice if the peer or network
        /// goes down. It is probably better to implement an application level
        /// keep alive.
        /// </summary>
        /// <param name="keepAlive">true enables tcp keep alive, requesting the periodic
        /// exchange of protocol packets to keep the tcp connection alive. The
        /// default is false.</param>
        public void SetDefaultKeepAlive( bool keepAlive )
        {
            this.keepAlive = keepAlive;
        }

        private bool keepAlive = false;

        /// <summary>
        /// Sets the default value for the tcp linger parameter. 
        /// </summary>
        /// <param name="lingerTime">the linger time in seconds. A negative value requests
        /// platform defined handling of close. A zero value causes an immediate
        /// discarding of any buffered data and a reset of the connection upon
        /// close. A positive value causes close to delay for up to that many
        /// seconds for tcp to transmit and receive acknowledgement of any pending
        /// data. The default is 30 seconds.</param>

        public void SetDefaultLingerTime( int lingerTime )
        {
            this.lingerTime = lingerTime;
        }

        private int lingerTime = 30;

        /// <summary>
        /// Sets the default value for the tcp noDelay parameter. Normally
        /// true, enabling noDelay causes data queued for transmission to
        /// be sent asap. When disabled, data may be delayed a bit to allow
        /// it to be combined with other data moving through the channel.
        /// Also known as "Nagle's Algorithm". When sending "packetized
        /// data" on a stream, such as ipc/rpc, noDelay should be enabled.
        /// When sending large chunks of stream data, such as http requests
        /// and responses, noDelay should be disabled. In any event, calling
        /// flush usually forces any queued data onto the wire anyway.
        /// </summary>
        /// <param name="tcpNoDelay">true enables noDelay. The default is true.</param>
        /// <seealso cref="flush()"/>
        public void SetDefaultNoDelay( bool tcpNoDelay )
        {
            this.tcpNoDelay = tcpNoDelay;
        }

        private bool tcpNoDelay = true;

        /// <summary>
        /// Sets the default value for the traffic class (or type of service
        /// or dscp). For example, call signalling traffic is classified as
        /// DSCP CS3 (011000) with resulting traffic class value of 01100000,
        /// 0x60, or 96. For more on DSCP see RFC 2474 (supercedes TOS defined
        /// in RFC 791)
        /// </summary>
        /// <param name="trafficClass">traffic class or type of service or dscp. The
        /// default is 0, which implies normal handling.</param>

        public void SetDefaultTrafficClass( int trafficClass )
        {
            this.trafficClass = trafficClass;
        }

        private int trafficClass = 0;

        /// <summary>
        /// Sets the default value for the buffer size parameter. If using buffered
        /// sends, you'll probably want to disable auto flush and flush manually
        /// only when needed.
        /// </summary>
        /// <param name="bufferSize">positive if calls to send should be buffered, zero if
        /// calls to send should go to the socket directly. The default is 0.</param>
        public void SetDefaultBufferSize( int bufferSize )
        {
            this.bufferSize = bufferSize;
        }

        private int bufferSize = 0;

        /// <summary>
        /// Sets the default value of the autoFlush flag.
        /// </summary>
        /// <param name="autoFlush">true if each call to send should be automatically
        /// flushed. The default is false.</param>
        /// <seealso cref="Send(byte[])"/>
        /// <seealso cref="Send(byte[], int, int)"/>

        public void SetDefaultAutoFlush( bool autoFlush )
        {
            this.autoFlush = autoFlush;
        }

        private bool autoFlush = false;

        
        protected override void Stop0()
        {
            Close( false );
            base.Stop0();
        }
        

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        /// Exception:
        ///     throws IOException
        private Socket CheckSocket() 
	    {
		    Socket s = socket;
            if ( s == null )
                throw new SocketException();
		    return s;
	    }

        [MethodImpl(MethodImplOptions.Synchronized)]
        protected override bool OpenSocket( bool reconnect )
        {
            // if a one time connection from a server socket listener, just
            // return the existing socket.
            if ( !reconnect && socket != null )
                return true;

            // if a one time connection from a server socket listener, and
            // this is a reconnect, then bail.
            if ( reconnect && host == null )
                return false;

            // if a reconnect but no retries allowed, then bail.
            if ( reconnect && delay <= 0 )
                return false;

            // ok, we don't have an existing socket, and this is either the first
            // connection attempt or a reconnect with delay > 0.

            bool first = true;

            while ( IsStarted() )
            {
                // if reconnect is false and first is true, this is our
                // very first attempt to connect. otherwise, we are trying
                // to reconnect a broken link or establish a link where we
                // have already failed at least once.

                if ( reconnect || !first )
                {
                    if ( delay == 0 )
                        return false;

                    System.Threading.Monitor.Wait( this, delay );

                    if ( !IsStarted() )
                        break;
                }

                // try to open a socket.

                try
                {
                    IPHostEntry ipEntry = Dns.GetHostEntry( host );
                    IPAddress[] addr = ipEntry.AddressList;
                    string sIPAddress = addr[ 0 ].ToString();

                    IPEndPoint ipe = new IPEndPoint( addr[0], port );
                    socket = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
                    socket.Connect(ipe);
                    return true;
                }
                catch ( Exception e )
                {
                    if ( first )
                    {
                        first = false;
                        FireException( "open", e );
                    }
                }
            }

            return false;
        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        protected override void SetUpSocket()
        {
            Socket s = CheckSocket();

            // s.SetKeepAlive( keepAlive );
            // s.SetTrafficClass( trafficClass );

            s.LingerState = new LingerOption(true, lingerTime) ;
            s.NoDelay = tcpNoDelay ;

            // No streams required since Windows sockets don't need that
            // mechanism
        }


        protected override void ReadSocket()
        {
            Socket s = CheckSocket();
            FlexBuffer buf = new FlexBuffer( new byte[ 8192 ] );

            try
            {
                while ( IsStarted() )
                {
                    int count = 0;
                    int n = 0;
                    
                    n = socket.Receive( buf.GetBuf(), count, buf.Length() - count, SocketFlags.None ) ;
                    
                    count += n;

                    if ( n <= 0 )
                        break;

                    buf.SetLength( n );
                    buf.SetIndex( 0 );
                    FireData(buf);
                }
            }
            catch (Exception e)
            {
                if ( e.Message.Contains( "blocking operation" ) )
                    return;
                if ( "socket closed".Equals( e.Message ) )
                    return;
                throw e;
            }   
        }


        public override void Close( bool reset )
        {
            Socket s = socket;
            if ( s != null )
            {
                try
                {
                    try
                    {
                        if ( reset )
                            s.LingerState = new LingerOption( true, 0 );
                        else
                        {
                            Flush();
                            ShutdownOutput();
                        }
                    }
                    finally
                    {
                        s.Close( 30 );
                    }
                }
                catch ( Exception e )
                {
                    if ( ( e is SocketException && ( e.Message.Contains( "did not properly respond" ) ) 
                        || ( e is ObjectDisposedException ) ) )
                        return;

                    throw e;
                }
                finally
                {
                    socket = null;
                }
            }
        }

        /////////////////////////
        // DATA SOURCE METHODS //
        /////////////////////////

        public void Data( Who recipient, FlexBuffer buf )
        {
            Send( buf.GetBuf(), buf.Index(), buf.Avail() );
        }

        /// <summary>
        /// Sends some data to the remote end. The output data is buffered
        /// until the buffer is full or the buffer is flushed.
        /// </summary>
        /// <param name="buf">the bytes to be sent</param>
        /// Exception:
        ///     throws Exception if there is a problem transmitting the
        ///     data. Such a problem causes the current connection to be
        ///     reset.
        /// <seealso cref="Flush()"/>
        /// <seealso cref="SetDefaultAutoFlush(bool)"/>
        public void Send( byte[] buf )
        {
            Send( buf, 0, buf.Length );
            
        }


        /// <summary>
        /// Sends some data to the remote end. The output data is buffered
        /// until the buffer is full or the buffer is flushed.
        /// </summary>
        /// <param name="buf">the bytes to be sent.</param>
        /// <param name="off">the offset into buf of the first byte to send.</param>
        /// <param name="len">the number of bytes to send.</param>
        /// Exception:
        ///     throws Exception if there is a problem transmitting the
        ///     data. Such a problem causes the current connection to be
        ///     reset.
        /// <seealso cref="Flush()"/>
        /// <seealso cref="SetDefaultAutoFlush(bool)"/>

        public void Send( byte[] buf, int off, int len)
        {
            try
            {
                Socket s = CheckSocket();
                
                int bytesSent = s.Send( buf, off, len, SocketFlags.None );

                if ( autoFlush )
                    Flush();

            }
            catch ( Exception e )
            {
                Close( true );
                throw e;
            }
        }

        /// <summary>
        /// Pushes any buffered data to the wire
        /// </summary>
        /// Exception:
        ///     throws Exception
        public void Flush()
        {
            try
            {
                //socket.SendBufferSize = 0;
            }
            catch ( Exception e )
            {
                Console.WriteLine(" Exception caught !! --> "+ e.Message);
                Console.WriteLine(e.StackTrace);
            }
        }

        /// <summary>
        /// Shuts down the input channel. This side will get
        /// an eof reading the input. The other side will get
        /// an error trying to transmit.
        /// </summary>
        /// Exception:
        ///     throws Exception
        public void ShutdownInput() 
        {
            Socket s = CheckSocket();
            s.Shutdown( SocketShutdown.Receive );
        }

        /// <summary>
        /// Shuts down the output channel. The other side will
        /// get an eof reading the input. This side will get an
        /// error trying to transmit.
        /// </summary>
        /// Exception:
        ///     throws Exception
        public void ShutdownOutput()
        {
            Socket s = CheckSocket();
            s.Shutdown( SocketShutdown.Send );
        }


        /// <summary>
        /// 
        /// </summary>
        /// <returns>Remote socket address</returns>
        /// Exception:
        ///     throws Exception
        /*
         * public EndPoint GetRemoteSocketAddress()
        {
            return CheckSocket().RemoteEndPoint;
        }*/

        /// <summary>
        /// 
        /// </summary>
        /// <param name="buf"></param>
        /// Exception:
        ///     throws Exception
        private void FireData( FlexBuffer buf )
        {
            handler.Data( null, buf );
        }

        public override EndPoint LocalAddress()
        {
            return CheckSocket().LocalEndPoint;
        }

        public override EndPoint RemoteAddress()
        {
            return CheckSocket().RemoteEndPoint;
        }
    }
}
