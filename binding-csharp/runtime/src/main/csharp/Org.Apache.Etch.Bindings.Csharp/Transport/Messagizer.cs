// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    /// <summary>
    /// Messagizer is a PacketHandler which accepts packets from
    /// a PacketSource and turns them into messages for delivery
    /// to a MessageHandler. Messagizer is also a MessageSource,
    /// accepting messages and turning them into packets for delivery
    /// to a PacketSource.
    /// </summary>
    public class Messagizer : TransportMessage, SessionPacket
    {
        /// <summary>
        /// Name of format name in uri or resources. The value is "Messagizer.format".
        /// The result of looking up this name should be a String.
        /// <seealso cref="Messagizer(TransportPacket, URL, Resources)"/>
        /// </summary>
        public const String FORMAT = "Messagizer.format";

        /// <summary>
        /// Constructs the Messagizer as a session of transport using uri and resources.
        /// </summary>
        /// <param name="transport">used to deliver packets to the peer.</param>
        /// <param name="uri">the uri being used to configure the transport stack.</param>
        /// <param name="resources">the associated set of resources for this service.</param>      
        
        public Messagizer(TransportPacket transport, string uri, Resources resources)
        	: this(transport, new URL(uri), resources)
        {
        }


        /// <summary>
        /// Constructs the Messagizer as a session of transport using uri and resources.
        /// </summary>
        /// <param name="transport">used to deliver packets to the peer.</param>
        /// <param name="uri">the uri being used to configure the transport stack.</param>
        /// <param name="resources">the associated set of resources for this service.</param>
        public Messagizer(TransportPacket transport, URL uri, Resources resources)
        {
            // find the format.

            this.transport = transport;

            String format = uri.GetTerm(FORMAT, FormatFactory.BINARY);

            // find the format factory.

            FormatFactory ff = FormatFactory.Get( format );
            if ( ff == null )
                throw new ArgumentException(
                String.Format( "format factory is not defined as '{0}' in format factories", format ) );

            // find the value factory.

            ValueFactory vf = ( ValueFactory ) resources.Get( TransportConsts.VALUE_FACTORY );
            if ( vf == null )
                throw new ArgumentException(String.Format(
                    "value factory is not defined as '{0}' in resources",
                    TransportConsts.VALUE_FACTORY));

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
}
