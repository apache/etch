using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Etch.Support;
using Etch.Util;

namespace Etch.Transport
{
    /// <summary>
    /// Implementation of a connection which handles a socket listener.
    /// </summary>
    public class Listener : Connection<ListenerHandler> , DataSource
    {

        /// <summary>
        /// Constructs the Listener.
        /// </summary>
        /// <param name="handler">tarGet to receive listener events.</param>
        /// <param name="backlog">max number of pending connection requests.</param>
        /// <param name="host">address to listen to. Null means any local address.</param>
        /// <param name="port">port to listen on. Port must be >= 0. Port of 0
        /// means allocate an available port.</param>
        /// <param name="delay">how long in milliseconds to wait before retrying a
        /// failure. Delay must be >= 0. Delay of 0 means do not retry.</param>

        public Listener( ListenerHandler handler, int backlog, String host,
            int port, int delay ) : base( handler )
        {
            if ( backlog < 0 )
                throw new ArgumentException( "backlog < 0" );

            if ( port < 0 || port > 65535 )
                throw new ArgumentException( "port < 0 || port > 65535" );

            if ( delay < 0 )
                throw new ArgumentException( "delay < 0" );

            this.backlog = backlog;
            this.host = host;
            this.port = port;
            this.delay = delay;
        }

        /// <summary>
        /// Query term from URI to specify backlog value to ServerSocket. The
        /// value is "TcpListener.backlog".
        /// </summary>
        public readonly static String BACKLOG = "TcpListener.backlog";

        /// <summary>
        /// Constructs a Listener, initialized from the URI. Listens on the host
        /// and port specified in the URI. To listen on all interfaces, specify
        /// host as ALL_INTFS ("0.0.0.0"). If port is specified or defaulted to 0,
        /// an unused port will be selected.
        /// </summary>
        /// <param name="uri"></param>
        public Listener( URL uri ) : this( null, (int)uri.GetIntegerTerm( BACKLOG, 0 ), TranslateHost( uri.Host ), uri.Port, 0 )
        {}

        private int backlog;
	
	    private String host;
    	
	    private int port;
    	
	    private int delay;

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        
        protected override void Stop0()
        {
            Close( true );
            base.Stop0();
        }
        
        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        /// <returns></returns>
        private Socket CheckSocket()
        {
            Socket ss = serverSocket;
            if ( ss == null )
                throw new SocketException();

            return ss;
        }

        private Socket serverSocket;

        public override string ToString()
        {
            if ( serverSocket == null )
                return String.Format("Listener (down, {0}, {1})", host, port);

            return String.Format("Listener (up, {0})", serverSocket.LocalEndPoint);
        }

        [MethodImpl (MethodImplOptions.Synchronized)]
        protected override bool OpenSocket( bool reconnect )
        {
            bool first = true;
            while ( IsStarted() )
            {
                if ( reconnect || !first )
                {
                    if ( delay == 0 )
                        return false;

                    System.Threading.Monitor.Wait(this, delay);

                    if ( !IsStarted() )
                        break;
                }

                try
                {
                    IPHostEntry ipEntry = Dns.GetHostEntry( host );
                    IPAddress[] addr = ipEntry.AddressList;
                    string sIPAddress = addr[ 0 ].ToString();

                    IPEndPoint ipe = new IPEndPoint( addr[ 0 ], port );
                    
                    serverSocket = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );

                    serverSocket.Bind( ipe );
                    serverSocket.Listen( backlog );

                    return true;
                }
                //catch ( Exception e )
                catch (SocketException e )
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
            // nothing to do.
        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        protected override void ReadSocket()
        {
            Socket ss = CheckSocket();
            while ( IsStarted() )
            {
                Socket s = ss.Accept();

                try
                {
                    handler.Accepted( s );
                }
                catch ( Exception e )
                {
                    s.Close();
                    FireException("accepted", e);
                }
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="reset"></param>
        /// Exception:
        ///     throws Exception
        public override void Close( bool reset )
        {
            Socket ss = CheckSocket();
            if ( ss != null )
            {
                serverSocket = null;
                ss.Close();
            }
        }

        public override EndPoint LocalAddress()
        {
            return CheckSocket().LocalEndPoint;
        }

        public override EndPoint RemoteAddress()
        {
            // ignore
            return null;
        }

        public void Data( Who recepient, FlexBuffer buf )
        {
            // ignore
        }
        
        public DataHandler GetHandler()
        {
            return (DataHandler) handler;
        }

        public void SetHandler( DataHandler handler )
        {
            base.SetHandler( handler );
            handler.SetSource( this );
        }
    }
}
