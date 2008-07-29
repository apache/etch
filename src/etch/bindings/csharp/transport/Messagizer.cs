using System;
using System.Text;
using System.Net;
using System.Collections.Generic;
using Etch.Msg;
using Etch.Support;
using Etch.Transport.Fmt.Binary;
using Etch.Util;

namespace Etch.Transport
{
    /// <summary>
    /// Messagizer is a PacketHandler which accepts packets from
    /// a PacketSource and turns them into messages for delivery
    /// to a MessageHandler. Messagizer is also a MessageSource,
    /// accepting messages and turning them into packets for delivery
    /// to a PacketSource.
    /// </summary>
    public class Messagizer : PacketHandler, MessagizerMessageSource
    {

        /// <summary>
        /// Name of format name in uri or resources. The value is "Messagizer.format".
        /// The result of looking up this name should be a String.
        /// <seealso cref="Messagizer(URL, Resources)"/>
        /// </summary>
        public static readonly String FORMAT = "Messagizer.format";

        /// <summary>
        /// Name of value factory in resources. The value is "Messagizer.valueFactory".
        /// The result of looking up this name should be a {@link ValueFactory} for
        /// the service.
        /// <seealso cref="Messagizer(URL, Resources)"/>
        /// </summary>
        public static readonly String VALUE_FACTORY = "Messagizer.valueFactory";

        /// <summary>
        /// Constructs the Messagizer with binary tagged data format.
        /// </summary>
        /// <param name="handler"></param>
        /// <param name="vf"></param>
        public Messagizer( MessageHandler<MessagizerMessageSource> handler, ValueFactory vf )
        {
            this.handler = handler;
            // this.vf = vf;
            tdi = new BinaryTaggedDataInput( vf, (FlexBuffer) null );
            tdo = new BinaryTaggedDataOutput( null, vf );
        }

        /// <summary>
        /// Constructs the Messagizer with null handler and tagged data format
        /// constructed from the format factory.
        /// </summary>
        /// <param name="uri">the uri being used to configure the transport stack.</param>
        /// <param name="resources">the associated set of resources for this service.</param>
        public Messagizer( URL uri, Resources resources )
        {
            // find the format.

            String format = uri.GetTerm( FORMAT );
            if ( format == null )
            {
                format = ( String ) resources.Get( FORMAT );
                if ( format == null )
                    throw new ArgumentException( String.Format( "format name is not defined as '{0}' in uri or resources", FORMAT ) );
            }

            // find the format factory.

            FormatFactory ff = FormatFactory.Get( format );
            if ( ff == null )
                throw new ArgumentException(
                String.Format( "format factory is not defined as '{0}' in format factories", format ) );

            // find the value factory.

            ValueFactory vf = ( ValueFactory ) resources.Get( VALUE_FACTORY );
            if ( vf == null )
                throw new ArgumentException(
                    String.Format( "value factory is not defined as '{0}' in resources", VALUE_FACTORY ) );

            tdi = ff.NewTaggedDataInput( vf, uri );
            tdo = ff.NewTaggedDataOutput( vf, uri );
        }

        private MessageHandler<MessagizerMessageSource> handler;

        public /*MessageHandler<MessagizerMessageSource>*/ Object GetHandler()
        {
            return handler;
        }

        public void SetHandler( Object handler )
        {
            this.handler = ( MessageHandler<MessagizerMessageSource> ) handler;
        }

        // private readonly ValueFactory vf;
        private readonly TaggedDataInput tdi;
        private readonly TaggedDataOutput tdo;

        public override string ToString()
        {
            return String.Format("Messagizer : {0}", src);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="src"></param>
        /// <param name="sender"></param>
        /// <param name="buf"></param>
        /// Exception:
        ///     throws Exception
        public void Packet( Who sender, FlexBuffer buf ) 
	    {
            // messagize the packet

            tdi.SetBuffer( buf );
            Message msg = tdi.ReadMessage();
            handler.Message( sender, msg );
	    }

        ////////////////////
        // Message Source //
        ////////////////////
	
        /// <summary>
        /// 
        /// </summary>
        /// <param name="recipient"></param>
        /// <param name="msg"></param>
        /// Exception:
        ///     throws Exception
        public void Messagex( Who recipient, Message msg )
	    {
		    // packetize the message.
    		
		    lock (msgBuf)
		    {
			    try
			    {
				    // assert that msgBuf is reset.
				    // leave space for the packet header
				    msgBuf.Skip( src.HeaderSize(), true );
                    tdo.SetBuffer( msgBuf );
                    tdo.WriteMessage( msg );
				    msgBuf.SetIndex( 0 );
				    src.Packet( recipient, msgBuf );
			    }
			    finally
			    {
				    msgBuf.Reset();
			    }
		    }
	    }

        private readonly FlexBuffer msgBuf = new FlexBuffer();

        ///////////////////////////
        // SourceHandler methods //
        ///////////////////////////

        public PacketSource GetSource()
        {
            return src;
        }

        public void SetSource( PacketSource src )
        {
            this.src = src;
        }

        private PacketSource src;

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

        public void SetHandler( MessageHandler<MessagizerMessageSource> handler )
        {
            this.handler = handler;
            handler.SetSource( this );
        }

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
