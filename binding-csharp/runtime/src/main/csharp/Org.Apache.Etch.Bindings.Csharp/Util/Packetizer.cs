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
using System.Diagnostics;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Packetizes a stream data source. Reads a packet header,
    /// a 32-bit flag and a 32-bit length, little-endian, verifies
    /// the flag, and then, using the length from the header,
    /// reads the packet data and passes it to the packet handler.
    /// As a packet source, accepts a packet and prepends a packet
    /// header to it before delivering it to a data source.
    /// </summary>
    public class Packetizer : SessionData, TransportPacket
    {

        /// <summary>
        /// URI term to specify max packet size
        /// </summary>
        public const String MAX_PKT_SIZE_TERM = "Packetizer.maxPktSize";

        private const int SIG = unchecked( (int) 0xdeadbeef );

	    private const int HEADER_SIZE = 8;
        
        /// <summary>
        /// The default maximum packet size that will be accepted, 16376 bytes.
        /// </summary>
        public const int DEFAULT_MAX_PKT_SIZE = 16384 - HEADER_SIZE;

        /// <summary>
        /// Constructs the Packetizer with the specified transport
        /// and the packet size. 
        /// </summary>
        /// <param name="transport">Transport to send data</param>
        /// <param name="maxPktSize">the maximum packet size that will be accepted.
        /// Must be >= 0. If maxPktSize == 0, the default will be used.</param>
        private Packetizer( TransportData transport, int maxPktSize )
        {
            if ( maxPktSize < 0 )
                throw new ArgumentOutOfRangeException( "maxPktSize < 0" );

            this.transport = transport;
            this.maxPktSize = maxPktSize;
            
            transport.SetSession(this);
        }

   

        public Packetizer( TransportData transport, URL uri, Resources resources )
            : this( transport, (int)uri.GetIntegerTerm( MAX_PKT_SIZE_TERM, DEFAULT_MAX_PKT_SIZE ) )
        {
            // nothing to do.
        }

        public Packetizer(TransportData transport, String uri, Resources resources)
        	: this(transport, new URL(uri), resources)
        {
           // nothing to do.
        }

        private readonly TransportData transport;

        private SessionPacket session;

        private readonly int maxPktSize;

        public override string ToString()
        {
            return String.Format("Packetizer / {0}", transport);
        }

        private bool wantHeader = true;

        private int bodyLen;

        private readonly FlexBuffer savedBuf = new FlexBuffer();

        private int ProcessHeader( FlexBuffer buf, bool reset )
        {
            int sig = buf.GetInt();
            if ( sig != SIG )
                throw new Exception( "bad SIG" );

            int pktSize = buf.GetInt();

            if ( reset )
                buf.Reset();

            if ( pktSize < 0 || (maxPktSize > 0 && pktSize > maxPktSize) )
                throw new Exception( "pktSize < 0 || (maxPktSize > 0 && pktSize > maxPktSize)" );

            return pktSize;

        }

  

        public int HeaderSize()
        {
            return HEADER_SIZE;
        }

       


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

        #region TransportPacket Members

        public void SetSession(SessionPacket session)
        {
            this.session = session;
        }

        public SessionPacket GetSession()
        {
            return this.session;
        }

        public void TransportPacket(Who recipient, FlexBuffer buf)
        {
            // Data-ize the packet

            // assert index is at the start of the header.
            int dataSize = buf.Avail();
            if (dataSize < HEADER_SIZE)
                throw new ArgumentException("dataSize < HEADER_SIZE");

			int pktSize = dataSize - HEADER_SIZE;
			if (maxPktSize > 0 && pktSize > maxPktSize)
				throw new ArgumentException( "maxPktSize > 0 && pktSize > maxPktSize" );
			
            int index = buf.Index();
            buf.PutInt(SIG);
            buf.PutInt(pktSize);
            buf.SetIndex(index);
            transport.TransportData(recipient, buf);
        }

        #endregion

        #region SessionData Members

        public void SessionData(Who sender, FlexBuffer buf)
        {
            while (buf.Avail() > 0)
            {
                if (wantHeader)
                {
                    // do we have enough to make a header
                    if ((savedBuf.Length() + buf.Avail()) >= HEADER_SIZE)
                    {
                        int pktSize;
                        if (savedBuf.Length() == 0)
                        {
                            // savedBuf is empty, entire header in buf.
                            pktSize = ProcessHeader(buf, false);
                        }
                        else   // header split across savedBuf and buf
                        {
                            // move just enough data from buf to savedBuf to have a header.

                            int needFromBuf = HEADER_SIZE - savedBuf.Length();
                            savedBuf.Put(buf, needFromBuf);
                            savedBuf.SetIndex(0);

                            pktSize = ProcessHeader(savedBuf, true);


                        }
                        if (pktSize == 0)
                            continue;

                        bodyLen = pktSize;
                        wantHeader = false;
                    }
                    else     // want header but not enough space to make it
                    {
                        // save buf in savedBuf.

                        savedBuf.SetIndex(savedBuf.Length());
                        savedBuf.Put(buf);
                    }
                }
                else if ((savedBuf.Length() + buf.Avail()) >= bodyLen)
                {
                    // want body, and there's enough to make it.

                    // three possible cases: the body is entirely in savedBuf,
                    // the body is split, or the body is entirely in buf. assert
                    // that the body cannot entirely be in savedBuf, or else
                    // we'd have processed it last time.

                    Debug.Assert(savedBuf.Length() < bodyLen);

                    if (savedBuf.Length() == 0)
                    {
                        // savedBuf is empty, entire body in buf.

                        int length = buf.Length();
                        int index = buf.Index();
                        buf.SetLength(index + bodyLen);

                      //  handler.Packet(sender, buf);
                        session.SessionPacket(sender,buf);

                        buf.SetLength(length);
                        buf.SetIndex(index + bodyLen);

                        wantHeader = true;
                    }

                    else    // body split across savedBuf and buf
                    {
                        // move just enough data from buf to savedBuf to have a body.

                        int needFromBuf = bodyLen - savedBuf.Length();
                        savedBuf.Put(buf, needFromBuf);
                        savedBuf.SetIndex(0);

               //         handler.Packet(sender, savedBuf);
                        session.SessionPacket(sender,savedBuf);

                        savedBuf.Reset();
                        wantHeader = true;
                    }
                }

                else     // want body, but there's not enough to make it.
                {
                    // save buf in savedBuf.

                    savedBuf.Put(buf);
                }
            }
            // buf is now empty, and there's nothing else to do.
            Debug.Assert(buf.Avail() == 0);
        }

        #endregion
    }
}
