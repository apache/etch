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
using System.Collections.Generic;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    [TestFixture]
    public class TestPacketizer
    {
        private static MyPacketHandler mph ;
        private static Packetizer p ;
        private static MyDataHandler dh;

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestPacketizer" );
        }

        public void SetUp()
        {
            dh = new MyDataHandler();
            p = new Packetizer(dh,"tcp:",null);
            mph = new MyPacketHandler( p);
           
          
        }

        public class DummyWho : Who 
        {
            public DummyWho()
            {
                // do nothing
            }
        }
        private Who whoDummy = new DummyWho();

        [Test]
        public void SendPacket()
        {
            SetUp();
            FlexBuffer buf = new FlexBuffer( new byte[] { 0, 0, 0, 0, 0, 0, 0, 0 } );
            
            byte[][] result = ( FlexBuffer.littleEndian )? new byte[][] { new byte[] { 239, 190, 173, 222, 0, 0, 0, 0 }, } :
                new byte[][] { new byte[] { 222, 173, 190, 239, 0, 0, 0, 0 }, };

        //    p.Packet( whoDummy, buf );
            p.TransportPacket(whoDummy,buf);
            Assert.AreEqual( dh.what, What.DATA );
            Assert.IsTrue( dh.Check( result ) );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );
        }

        [Test]
        public void SendPacket1()
        {
            SetUp();
            // Create Packet to send
            FlexBuffer buf = new FlexBuffer( new byte[] { 0, 0, 0, 0, 0, 0, 0, 0, 1 } );
            
            byte[][] result = ( FlexBuffer.littleEndian ) ? new byte[][] { new byte[] { 239, 190, 173, 222, 1, 0, 0, 0, 1 },  } : 
                new byte[][] { new byte[] { 222, 173, 190, 239, 0, 0, 0, 1, 1 },  };

        //    p.Packet( null, buf );
            p.TransportPacket(null,buf);
            Assert.AreEqual( dh.what, What.DATA );
            Assert.IsTrue( dh.Check( result ) );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );
        }
        
        [Test]
        public void SendPacket2() 
	    {
            SetUp();
		    // Create Packet to send
		    FlexBuffer buf = new FlexBuffer (new byte[] { 0, 0, 0, 0, 0, 0, 0, 0, 2, 3 });
            byte[][] result = ( FlexBuffer.littleEndian ) ? new byte[][] { new byte[] { 239, 190, 173, 222, 2, 0, 0, 0, 2, 3 }, } : 
                new byte[][] { new byte[] { 222, 173, 190, 239, 0, 0, 0, 2, 2, 3 }, };
    		
		    p.TransportPacket(null, buf);
		    Assert.AreEqual( dh.what, What.DATA );
		    Assert.IsTrue(dh.Check(result));
		    Assert.IsNull(dh.xsender);
		    Assert.IsNull(dh.xbuf);
	    }
        
        ////////////////////////////////////////////////////////////
        // these tests are for a single Packet in a single buffer //
        ////////////////////////////////////////////////////////////

        [Test]
        public void SendSingleSingleData0()
        {
            SetUp();
            // Create Data to send
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 0, 0, 0, 0 } ) :
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0, 0, 0 } );
		    
            byte[][] result = new byte[][] { };
    		
		    p.SessionData(null, buf);
		    Assert.IsTrue(mph.Check(result));
    		
		    Assert.IsNull(mph.what);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);
        }
        
        [Test]
	    public void SendSingleSingleData1() 
	    {
            SetUp();
		    // length = 1
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 1, 0, 0, 0, 1 } ) :
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0, 0, 1, 1 } );

		    byte[][] result = new byte[][] { new byte[] { 1 },  };
    		
		    p.SessionData(null, buf);
		    Assert.IsTrue(mph.Check(result));
    		
		    Assert.AreEqual(mph.what, What.PACKET);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);
	    }
    	
	    [Test]
	    public void SendSingleSingleData2()  
	    {
            SetUp();
		    // length = 2
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 2, 0, 0, 0, 3, 4 } ) :
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0, 0, 2, 3, 4 } );

		    byte[][] result = new byte[][] { new byte[] { 3, 4 }, };
    		
		    p.SessionData(null, buf);
		    Assert.IsTrue(mph.Check(result));
    		
		    Assert.AreEqual(mph.what, What.PACKET);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);
	    }
    	
	    ////////////////////////////////////////////////////////
	    // these tests are for two Packets in a single buffer //
	    ////////////////////////////////////////////////////////
    	
	    [Test]
	    public void SendDoubleSingleData0()  
	    {
            SetUp();
		    // length = 0
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 0, 0, 0, 0, 239, 190, 173, 222, 0, 0, 0, 0 } ) : 
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0, 0, 0, 222, 173, 190, 239, 0, 0, 0, 0 } );

		    byte[][] result = new byte[][] {};
    		
		    p.SessionData(null, buf);
    		
		    Assert.IsTrue(mph.Check(result));
		    Assert.IsNull(mph.what);
		    Assert.IsNull(mph.xbuf);
		    Assert.IsNull(mph.xsender);
	    }
    	
	    [Test]
	    public void SendDoubleSingleData1()  
	    {
            SetUp();
		    // length = 1
            FlexBuffer buf = ( FlexBuffer.littleEndian )? new FlexBuffer( new byte[] { 239, 190, 173, 222, 1, 0, 0, 0, 1, 239, 190, 173, 222, 1, 0, 0, 0, 2 } ) : 
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0, 0, 1, 1, 222, 173, 190, 239, 0, 0, 0, 1, 2 } );

		    byte[][] result = new byte[][] { new byte[] { 1 }, new byte[] { 2 } };
    		
		    p.SessionData(null, buf);
    		
		    Assert.IsTrue(mph.Check(result));
		    Assert.AreEqual(mph.what, What.PACKET);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);
	    }
    	
	    [Test]
	    public void SendDoubleSingleData2()  
	    {
            SetUp();
		    // length = 2
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 2, 0, 0, 0, 3, 4, 239, 190, 173, 222, 2, 0, 0, 0, 5, 6 } ) : 
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0, 0, 2, 3, 4, 222, 173, 190, 239, 0, 0, 0, 2, 5, 6 } );

		    byte[][] result = new byte[][] { new byte[] { 3, 4 }, new byte[] { 5, 6 } } ;
    		
		    p.SessionData(null, buf);
    		
		    Assert.IsTrue(mph.Check(result));
		    Assert.AreEqual(mph.what, What.PACKET);
		    Assert.IsNull(mph.xbuf);
		    Assert.IsNull(mph.xsender);
	    }
    	
	    //////////////////////////////////////////////////////////////////////
	    // these tests are for two Packets in two buffers with header split //
	    //////////////////////////////////////////////////////////////////////
    	
	    [Test]
	    public void SendDoubleSingle_HeaderSplit_Data0()  
	    {
            SetUp();
		    // length = 0
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 0, 0 } ) : 
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0 } );
		    byte[][] result = new byte[][] { } ;
    		
		    p.SessionData(null, buf);
    		
		    Assert.IsTrue(mph.Check(result));
		    Assert.IsNull(mph.what);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);

            FlexBuffer buf2 = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 0, 0, 239, 190, 173, 222, 0, 0, 0, 0 } ) :
                new FlexBuffer( new byte[] { 0, 0, 222, 173, 190, 239, 0, 0, 0, 0 } );
		    byte[][] result2 = new byte[][] {};
    		
		    p.SessionData(null, buf2);
    		
		    Assert.IsTrue(mph.Check(result2));
		    Assert.IsNull(mph.what);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);
	    }
    	
	    [Test]
	    public void SendDoubleSingle_HeaderSplit_Data1()  
	    {
            SetUp();
		    // length = 1
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 1, 0 } ) : 
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0 } );
		    byte[][] result = new byte[][] { } ;
    		
		    p.SessionData(null, buf);
    		
		    Assert.IsTrue(mph.Check(result));
		    Assert.IsNull(mph.what);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);

            FlexBuffer buf2 = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 0, 0, 1, 239, 190, 173, 222, 1, 0, 0, 0, 2 } ) : 
                new FlexBuffer( new byte[] { 0, 1, 1, 222, 173, 190, 239, 0, 0, 0, 1, 2 } );
		    byte[][] result2 = new byte[][] {new byte[] { 1 }, new byte[] { 2 }};
    		
		    p.SessionData(null, buf2);
    		
		    Assert.IsTrue(mph.Check(result2));
		    Assert.AreEqual(mph.what, What.PACKET);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);
	    }
    	
	    [Test]
	    public void sendDoubleSingle_HeaderSplit_Data2()  
	    {
            SetUp();
		    // length = 2
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 2, 0 } ) : 
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0 } );
		    byte[][] result = new byte[][] { } ;
    		
		    p.SessionData(null, buf);
    		
		    Assert.IsTrue(mph.Check(result));
		    Assert.IsNull(mph.what);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);

            FlexBuffer buf2 = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 0, 0, 3, 4, 239, 190, 173, 222, 2, 0, 0, 0, 5, 6 } ) :
                new FlexBuffer( new byte[] { 0, 2, 3, 4, 222, 173, 190, 239, 0, 0, 0, 2, 5, 6 } );

		    byte[][] result2 = new byte[][] { new byte[] { 3, 4 }, new byte[] { 5, 6 } };
    		
		    p.SessionData(null, buf2);
    		
		    Assert.IsTrue(mph.Check(result2));
		    Assert.AreEqual(mph.what, What.PACKET);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);
	    }
    	
	    ////////////////////////////////////////////////////////////////////
	    // these tests are for two Packets in two buffers with body split //
	    ////////////////////////////////////////////////////////////////////
    	
	    [Test]
	    public void SendDoubleSingle_BodySplit_Data2()  
	    {
            SetUp();
		    // length = 2
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 2, 0, 0, 0, 1 } ) : 
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0, 0, 2, 1 } );
		    
            byte[][] result = new byte[][] { } ;
    		
		    p.SessionData(null, buf);
    		
		    Assert.IsTrue(mph.Check(result));
		    Assert.IsNull(mph.what);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);

            FlexBuffer buf2 = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 2, 239, 190, 173, 222, 2, 0, 0, 0, 3, 4 } ) :
                new FlexBuffer( new byte[] { 2, 222, 173, 190, 239, 0, 0, 0, 2, 3, 4 } );

		    byte[][] result2 = new byte[][] { new byte[] { 1, 2 }, new byte[] { 3, 4 } };
    		
		    p.SessionData(null, buf2);
    		
		    Assert.IsTrue(mph.Check(result2));
		    Assert.AreEqual(mph.what, What.PACKET);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);
	    }
    	
	    [Test]
	    public void SendDoubleSingle_BodySplit_Data3()  
	    {
            SetUp();
		    // length = 3
            FlexBuffer buf = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 239, 190, 173, 222, 3, 0, 0, 0, 5, 6 } ) : 
                new FlexBuffer( new byte[] { 222, 173, 190, 239, 0, 0, 0, 3, 5, 6 } );

		    byte[][] result = new byte[][] { } ;
    		
		    p.SessionData(null, buf);
    		
		    Assert.IsTrue(mph.Check(result));
		    Assert.IsNull(mph.what);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);

            FlexBuffer buf2 = ( FlexBuffer.littleEndian ) ? new FlexBuffer( new byte[] { 7, 239, 190, 173, 222, 3, 0, 0, 0, 8, 9, 10 } ) :
                new FlexBuffer( new byte[] { 7, 222, 173, 190, 239, 0, 0, 0, 3, 8, 9, 10 } );

		    byte[][] result2 = new byte[][] { new byte[] { 5, 6, 7 }, new byte[] { 8, 9, 10 } };
    		
		    p.SessionData(null, buf2);
    		
		    Assert.IsTrue(mph.Check(result2));
		    Assert.AreEqual(mph.what, What.PACKET);
		    Assert.IsNull(mph.xsender);
		    Assert.IsNull(mph.xbuf);
	    }
    	
	    [Test]
	    public void Up()  
	    {
            SetUp();
            p.SessionNotify( SessionConsts.UP );
    		
		    Assert.AreEqual(mph.what, What.UP);
		    Assert.IsNull(mph.xbuf);
		    Assert.IsNull(mph.xsender);
	    }
    	
	    [Test]
	    public void Down()   
	    {
            SetUp();
            p.SessionNotify( SessionConsts.DOWN );
    		
		    Assert.AreEqual(mph.what, What.DOWN);
		    Assert.IsNull(mph.xbuf);
		    Assert.IsNull(mph.xsender);
	    }
    	
	    [Test]
	    public void LocalAddress()   
	    {
            SetUp();
		    p.TransportQuery( Connection<SessionData>.LOCAL_ADDRESS );
    		
		    Assert.AreEqual(dh.what, What.LOCALADDRESS);
		    Assert.IsNull(dh.xsender);
		    Assert.IsNull(dh.xbuf);
	    }
    	
	    [Test] 
	    public void RemoteAddress()   
	    {
            SetUp();
            p.TransportQuery( Connection<SessionData>.REMOTE_ADDRESS );
    		
		    Assert.AreEqual(dh.what, What.REMOTEADDRESS);
		    Assert.IsNull(dh.xsender);
		    Assert.IsNull(dh.xbuf);
	    }
    	
	    [Test] 
	    public void Stop()   
	    {
            SetUp();
            p.TransportControl( TransportConsts.STOP, null );
    		
		    Assert.AreEqual(dh.what, What.STOP);
		    Assert.IsNull(dh.xsender);
		    Assert.IsNull(dh.xbuf);
	    }
        
        public enum What { DATA, PACKET, UP, DOWN, RESET, LOCALADDRESS, REMOTEADDRESS, 
            STOP, STARTED, STOPPED, EXCEPTION };


        // This is like messagizer handling packets
        public class MyPacketHandler : SessionPacket
        {
            public Enum what;
            public Who xsender;
            public FlexBuffer xbuf;
            public Who xrecipient;
            public TransportPacket _tp;

            public MyPacketHandler()
            {
                // nothing to do.
            }

            public MyPacketHandler( TransportPacket tp )
            {
                _tp = tp;
                _tp.SetSession(this);
            }

            private List<byte[]> list = new List<byte[]>();

            // Auxiliary methods

            public void Clear()
            {
                list.Clear();
            }

            public bool Check( byte[][] result )
            {
                bool flag = Check( list.Count == result.Length );
                if ( flag == false )
                    return flag;

                for ( int i = 0; i < list.Count; i++ )
                {
                    flag = Check( list[i], result[i] );
                    if ( flag == false )
                        return flag;
                }
                return true;
            }

            private bool Check( byte[] a, byte[] b )
            {
                bool flag = Check( a.Length == b.Length );
                if ( flag == false )
                    return flag;

                for ( int i = 0; i < a.Length; i++ )
                {
                    flag = Check( a[ i ] == b[ i ] );
                    if ( flag == false )
                        return flag;
                }
                return true;
            }

            private bool Check( bool ok )
            {
                return ok;
            }

            public void tp_print()
            {
                foreach ( byte b in list[0] )
                Console.WriteLine(b);
            }

            // PacketHandler methods

            /// <summary>
            /// 
            /// </summary>
            /// <param name="src"></param>
            /// <param name="sender"></param>
            /// <param name="buf"></param>
            /// Exception:
            ///      
            public void Packet( Who sender, FlexBuffer buf )
            {
                what = What.PACKET;
                xsender = sender;
                list.Add(buf.GetAvailBytes());
            }
    		
            // DataSource methods
		
            /// <summary>
            /// 
            /// </summary>
            /// <param name="recipient"></param>
            /// <param name="buf"></param>
            /// Exception:
            ///      
		    public void Data( Who recipient, FlexBuffer buf )
		    {
    			
			    what = What.DATA;
                this.xrecipient = recipient;
			    list.Add( buf.GetAvailBytes() );
		    }

            

            #region Session Members

            public object SessionQuery( object query )
            {
                throw new Exception( "unsupported query " + query );
            }

            public void SessionControl( object control, object value )
            {
                throw new Exception( " unsupported control " + control );
            }

            public void SessionNotify( object eventObj )
            {
                if ( eventObj.Equals( SessionConsts.UP ) )
                {
                    what = What.UP;
                    return;
                }
                if ( eventObj.Equals( SessionConsts.DOWN ) )
                {
                    what = What.DOWN;
                    return;
                }
            }

            #endregion

           

            #region SessionPacket Members

            public void SessionPacket(Who sender, FlexBuffer buf)
            {
                what = What.PACKET;
                xsender = sender;
                list.Add(buf.GetAvailBytes());
            }

            #endregion

           
        }

        public class MyDataHandler : TransportData
        {
            public Enum what;
            public Who xsender;
            public FlexBuffer xbuf;
            public Who xrecipient;
            private List<byte[]> list = new List<byte[]>();

            private SessionData session;
            public MyDataHandler()
            {
                
            }

            #region TransportData Members

            public void TransportData(Who recipient, FlexBuffer buf)
            {
                what = What.DATA;
                this.xrecipient = recipient;
                list.Add(buf.GetAvailBytes());
            }

            #endregion

            #region Transport<SessionData> Members

            public object TransportQuery(object query)
            {
                if (query.Equals(Connection<SessionData>.LOCAL_ADDRESS))
                {
                    what = What.LOCALADDRESS;
                    return null;
                }
                if (query.Equals(Connection<SessionData>.REMOTE_ADDRESS))
                {
                    what = What.REMOTEADDRESS;
                    return null;
                }
                throw new Exception(" unsupported query " + query);
            }

            public void TransportControl(object control, object value)
            {
                if (control.Equals(TransportConsts.STOP))
                {
                    what = What.STOP;
                    return;
                }
                throw new Exception(" unsupported control " + control);
            }

            public void TransportNotify(object eventObj)
            {
                
            }

            public void SetSession(SessionData session)
            {
                this.session = session;
            }

            public SessionData GetSession()
            {
                return session;
            }

            #endregion

            // Auxiliary methods

            public void Clear()
            {
                list.Clear();
            }

            public bool Check(byte[][] result)
            {
                bool flag = Check(list.Count == result.Length);
                if (flag == false)
                    return flag;

                for (int i = 0; i < list.Count; i++)
                {
                    flag = Check(list[i], result[i]);
                    if (flag == false)
                        return flag;
                }
                return true;
            }

            private bool Check(byte[] a, byte[] b)
            {
                bool flag = Check(a.Length == b.Length);
                if (flag == false)
                    return flag;

                for (int i = 0; i < a.Length; i++)
                {
                    flag = Check(a[i] == b[i]);
                    if (flag == false)
                        return flag;
                }
                return true;
            }

            private bool Check(bool ok)
            {
                return ok;
            }
        }

    }
}
