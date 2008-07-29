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
    public class Messagizer : TransportMessage,SessionPacket
    {

        /// <summary>
        /// Name of format name in uri or resources. The value is "Messagizer.format".
        /// The result of looking up this name should be a String.
        /// <seealso cref="Messagizer(TransportPacket, URL, Resources)"/>
        /// </summary>
        public static readonly String FORMAT = "Messagizer.format";

        /// <summary>
        /// Name of value factory in resources. The value is "Messagizer.valueFactory".
        /// The result of looking up this name should be a {@link ValueFactory} for
        /// the service.
        /// <seealso cref="Messagizer( TransportPacket, URL, Resources)"/>
        /// </summary>
        public static readonly String VALUE_FACTORY = "Messagizer.valueFactory";

        

        /// <summary>
        /// Constructs the Messagizer with null handler and tagged data format
        /// constructed from the format factory.
        /// </summary>
        /// <param name="uri">the uri being used to configure the transport stack.</param>
        /// <param name="resources">the associated set of resources for this service.</param>      
        
        public Messagizer(TransportPacket transport, string uri, Resources resources)
        	: this(transport, new URL(uri), resources)
        {
        }


        /// <summary>
        /// Constructs the Messagizer with null handler and tagged data format
        /// constructed from the format factory.
        /// </summary>
        /// <param name="uri">the uri being used to configure the transport stack.</param>
        /// <param name="resources">the associated set of resources for this service.</param>
        private Messagizer(TransportPacket transport, URL uri, Resources resources)
        {
            // find the format.

            this.transport = transport;

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

            tdi = ff.NewTaggedDataInput( vf, uri.ToString() );
            tdo = ff.NewTaggedDataOutput( vf, uri.ToString() );

            transport.SetSession(this);
        }

      
        // private readonly ValueFactory vf;
        private readonly TaggedDataInput tdi;
        private readonly TaggedDataOutput tdo;
        private TransportPacket transport;
        private SessionMessage session;

        public override string ToString()
        {
            return String.Format("Messagizer : {0}", transport);
        }

        public TransportPacket GetTransport()
        {
            return transport;
        }

       

        


        private readonly FlexBuffer msgBuf = new FlexBuffer();

        
        public Object SessionQuery( Object query )
        {
            return session.SessionQuery( query );
        }

        public void SessionControl( Object control, Object value )
        {
            session.SessionControl( control, value );
        }

        public void SessionNotify( Object eventObj )
        {
            session.SessionNotify( eventObj );
        }

        

        public Object TransportQuery( Object query )
        {
            return transport.TransportQuery( query );
        }

        public void TransportControl( Object control, Object value )
        {
            transport.TransportControl( control, value );
        }

        public void TransportNotify( Object eventObj )
        {
            transport.TransportNotify( eventObj );
        }

        #region TransportMessage Members

        public void SetSession(SessionMessage session)
        {
            this.session = session;
        }

        public SessionMessage GetSession()
        {
            return this.session;
        }

        public void TransportMessage(Who recipient, Message msg)
        {

            // packetize the message.

            lock (msgBuf)
            {
                try
                {
                    // assert that msgBuf is reset.
                    // leave space for the packet header
                    msgBuf.Skip(transport.HeaderSize(), true);
                    tdo.WriteMessage(msg, msgBuf);
                    msgBuf.SetIndex(0);
                    transport.TransportPacket(recipient,msgBuf);
                }
                finally
                {
                    msgBuf.Reset();
                }
            }
           
        }

        #endregion

        #region SessionPacket Members

        public void SessionPacket(Who sender, FlexBuffer buf)
        {
            Message msg = tdi.ReadMessage(buf);
            bool handled = session.SessionMessage(sender,msg);
            if (!handled)
                session.SessionNotify(new UnwantedMessage(sender,msg));
        }

        #endregion
    }

    public class UnwantedMessage 
    {
        public UnwantedMessage(Who sender, Message msg)
        {
            _sender = sender;
            _msg = msg;
        }

        private Who _sender;

        private Message _msg;

        public override string ToString()
        {
            return " UnWanted Message: Sender " + _sender + " Message: " + _msg;
        }
    }
}
