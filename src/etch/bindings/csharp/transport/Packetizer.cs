using System;
using System.Text;
using System.Net;
using System.Collections.Generic;
using System.Diagnostics;
using Etch.Support;
using Etch.Util;

namespace Etch.Transport
{
    /// <summary>
    /// Packetizes a stream data source. Reads a packet header,
    /// a 32-bit flag and a 32-bit length, little-endian, verifies
    /// the flag, and then, using the length from the header,
    /// reads the packet data and passes it to the packet handler.
    /// As a packet source, accepts a packet and prepends a packet
    /// header to it before delivering it to a data source.
    /// </summary>
    public class Packetizer : DataHandler, PacketSource
    {

        /// <summary>
        /// URI term to specify max packet size
        /// </summary>
        public readonly static String MAX_PKT_SIZE_TERM = "Packetizer.maxPktSize";
        
        /// <summary>
        /// The default maximum packet size that will be accepted,
        /// 10240 bytes.
        /// </summary>
        public readonly static int DEFAULT_MAX_PKT_SIZE = 10240;

        /// <summary>
        /// Constructs the Packetizer with the specified packet handler
        /// and the packet size. 
        /// </summary>
        /// <param name="handler">handler to receive the packets</param>
        /// <param name="maxPktSize">the maximum packet size that will be accepted.
        /// Must be >= 0. If maxPktSize == 0, the default will be used.</param>
        public Packetizer( PacketHandler handler, int maxPktSize )
        {
            if ( maxPktSize < 0 )
                throw new ArgumentOutOfRangeException( "maxPktSize is < 0" );
            if ( maxPktSize == 0 )
                maxPktSize = DEFAULT_MAX_PKT_SIZE;

            this.handler = handler;
            this.maxPktSize = maxPktSize;
        }

        /// <summary>
        /// Constructs the Packetizer with the specified packet handler and default
        /// maximum packet size.
        /// </summary>
        /// <param name="handler">the handler to receive the packets.</param>
        public Packetizer( PacketHandler handler ) : this( handler, DEFAULT_MAX_PKT_SIZE )
        {
        }

        /// <summary>
        /// Constructs the Packetizer with null packet handler and the specfied
        /// maximum packet size.
        /// </summary>
        /// <param name="maxPktSize">the maximum packet size that will be accepted.
        /// Must be > 0.</param>
        public Packetizer( int maxPktSize )
            : this( null, maxPktSize )
        {
        }

        /// <summary>
        /// Constructs the Packetizer with null packet handler and default maximum
        /// packet size.
        /// </summary>
        public Packetizer() : this( null, DEFAULT_MAX_PKT_SIZE )
        {
        }

        public Packetizer( URL uri, Resources resources )
            : this( null, (int)uri.GetIntegerTerm( MAX_PKT_SIZE_TERM, DEFAULT_MAX_PKT_SIZE ) )
        {
        }

        private readonly int maxPktSize;

        public override string ToString()
        {
            return String.Format("Packetizer / {0}", src);
        }

        private int maxBodyLen = 10240;

        private bool wantHeader = true;

        private int bodyLen;

        private readonly FlexBuffer savedBuf = new FlexBuffer();

        private static int SIG = Convert.ToInt32( "0Xdeadbeef", 16 );

	    private static int HEADER_SIZE = 8;

        public void Data( Who sender, FlexBuffer buf )
        {
            // SetDataSource( src );

            // there are two options here. one is that we have no buffered data
		    // and the entire packet is contained within the buf. in that case
		    // we could optimize the daylights out of the process and directly
		    // drop the packet on the handler.
            while ( buf.Avail() > 0 )
            {
                if ( wantHeader )
                {
                    // do we have enough to make a header
                    if ( ( savedBuf.Length() + buf.Avail() ) >= HEADER_SIZE )
                    {
                        int len;
                        if ( savedBuf.Length() == 0 )
                        {
                            // savedBuf is empty, entire header in buf.
                            len = ProcessHeader( buf, false );
                        }
                        else   // header split across savedBuf and buf
                        {
                            // move just enough data from buf to savedBuf to have a header.

                            int needFromBuf = HEADER_SIZE - savedBuf.Length();
                            savedBuf.Put( buf, needFromBuf );
                            savedBuf.SetIndex( 0 );

                            len = ProcessHeader( savedBuf, true );

                            
                        }
                        if ( len == 0 )
                            continue;

                        bodyLen = len;
                        wantHeader = false;
                    }
                    else     // want header but not enough space to make it
                    {
                        // save buf in savedBuf.

                        savedBuf.SetIndex( savedBuf.Length() );
                        savedBuf.Put( buf );
                    }
                }
                else if ( ( savedBuf.Length() + buf.Avail() ) >= bodyLen )
                {
                    // want body, and there's enough to make it.

                    // three possible cases: the body is entirely in savedBuf,
                    // the body is split, or the body is entirely in buf. assert
                    // that the body cannot entirely be in savedBuf, or else
                    // we'd have processed it last time.

                    Debug.Assert( savedBuf.Length() < bodyLen );

                    if ( savedBuf.Length() == 0 ) 
                    {
                        // savedBuf is empty, entire body in buf.

                        int length = buf.Length();
                        int index = buf.Index();
                        buf.SetLength( index+bodyLen );

                        handler.Packet( sender, buf );

                        buf.SetLength( length );
                        buf.SetIndex( index+bodyLen );

                        wantHeader = true;
                    }

                    else    // body split across savedBuf and buf
                    {
                        // move just enough data from buf to savedBuf to have a body.

                        int needFromBuf = bodyLen - savedBuf.Length();
                        savedBuf.Put( buf, needFromBuf );
                        savedBuf.SetIndex( 0 );

                        handler.Packet( sender, savedBuf );

                        savedBuf.Reset();
                        wantHeader = true;
                    }
                }

                else     // want body, but there's not enough to make it.
                {
                    // save buf in savedBuf.

                    savedBuf.Put( buf );
                }
            }
            // buf is now empty, and there's nothing else to do.
            Debug.Assert( buf.Avail() == 0 );
        }

        private int ProcessHeader( FlexBuffer buf, bool reset )
        {
            int sig = buf.GetInt();
            if ( sig != SIG )
                throw new Exception( "Bad sig" );

            int len = buf.GetInt();

            if ( reset )
                buf.Reset();

            if ( len < 0 || len > maxBodyLen )
                throw new Exception( "len < 0 || len > maxBodyLen" );

            return len;

        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        /// <param name="recipient"></param>
        /// <param name="buf"></param>
        public void Packet( Who recipient, FlexBuffer buf )
        {
            // Dataize the packet

            // assert index is at the start of the header.
            int len = buf.Avail();

            if ( len < HEADER_SIZE )
                throw new ArgumentException("len < HEADER_SIZE");

            int index = buf.Index();
            buf.PutInt( SIG );
            buf.PutInt( len - HEADER_SIZE );
            buf.SetIndex( index );
            src.Data( recipient, buf );
        }

        public int HeaderSize()
        {
            return HEADER_SIZE;
        }

        ///////////////////////////
	    // SourceHandler methods //
	    ///////////////////////////

        public Source GetSource()
        {
            return src;
        }

        public void SetSource( Source src )
        {
            this.src = (DataSource) src;
        }

        private DataSource src;


        public Object SessionQuery( Object query )
        {
            return handler.SessionQuery( query );
        }

        public void SessionControl( Object control, Object value )
        {
            handler.SessionControl( control, value );
        }

        public void SessionNotify( Object eventObj )
        {
            handler.SessionNotify( eventObj );
        }


        ////////////////////
	    // Source methods //
	    ////////////////////

        public Object GetHandler()
        {
            return handler;
        }

        public void SetHandler( Object handler )
        {
            this.handler = (PacketHandler) handler;
            ( ( PacketHandler ) handler ).SetSource( this );
        }

        private PacketHandler handler;

        public Object TransportQuery( Object query )
        {
            return src.TransportQuery( query );
        }

        public void TransportControl( Object control, Object value )
        {
            src.TransportControl( control, value );
        }

        public void TransportNotify( Object eventObj )
        {
            src.TransportNotify( eventObj );
        }
    }
}
