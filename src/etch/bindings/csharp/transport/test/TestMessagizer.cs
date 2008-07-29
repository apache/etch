using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using System.Net;
using Etch.Support;
using Etch.Msg;
using Etch.Transport.Fmt.Binary;

namespace Etch.Transport.Test
{
    [TestFixture]
    public class TestMessagizer
    {

        
        private readonly Who who = new MyWho();

        //static Etch.Support.DefaultValueFactory vf = new Etch.Support.DefaultValueFactory();
        public DefaultValueFactory vf;
        public MyMessageHandler mh;
        public MyPacketSource ps;
        Messagizer m;

        public TestMessagizer()
        {
            //ps = new MyPacketSource( this );
        }

        /// <summary>
        /// Call before the test fixture
        /// </summary>
        [TestFixtureSetUp]
        public void SetUpBeforeClass() 
	    {
            vf = new MyValueFactory();

            
	    }

        public void SetUpBeforeEveryTest()
        {

            ps = new MyPacketSource();
            mh = new MyMessageHandler();

            m = new Messagizer( mh, vf );
            m.SetSource( ps );

        }

        #region Test PacketHandler methods 
	    
        public class MyWho : Who
        {
            public MyWho()
            {
                //nothing
            }
        }

        //[Test]
        //public void Up() 
        //{
        //    SetUpBeforeEveryTest();

        //    PacketHandler ph = m;

        //    Assert.IsNull(mh.what);
        //    Assert.IsNull( mh.src );

        //    ph.Up( ps );

        //    Assert.AreEqual( mh.src, m );
        //    Assert.AreEqual(mh.what, What.TESTMESSAGEHANDLERUP);
        //    Assert.AreEqual( ps, m.GetPacketSource() );
        //}

        [Test]
        public void Packet1()
        {
            SetUpBeforeEveryTest();

            PacketHandler ph = m;

            FlexBuffer buf = new FlexBuffer( new byte[] { (byte)BinaryTaggedData.VERSION, 
                unchecked((byte)Etch.Transport.Fmt.TypeCode.INT1), 1, unchecked((byte)Etch.Transport.Fmt.TypeCode.NONE) } );

            mh.handled = true;

            ph.Packet( who, buf );

            Assert.AreEqual( What.TESTMESSAGEHANDLERMESSAGE, mh.what );
            Assert.AreEqual( ps, m.GetSource() );
            Assert.AreEqual( who, mh.xsender);
            Assert.IsNotNull( mh.xmsg );
            Assert.AreEqual( MyValueFactory.mt_add, mh.xmsg.GetXType );
            Assert.AreEqual( 0, mh.xmsg.Count );

        }

        [Test]
        public void Packet2()
        {
            SetUpBeforeEveryTest();

            PacketHandler ph = m;

            FlexBuffer buf = new FlexBuffer( new byte[] { (byte)BinaryTaggedData.VERSION, 
                unchecked((byte)Etch.Transport.Fmt.TypeCode.INT1), 1, unchecked((byte)Etch.Transport.Fmt.TypeCode.NONE) } );

            mh.handled = false;

            ph.Packet( who, buf );

            Assert.AreEqual( What.TESTMESSAGEHANDLERMESSAGE, mh.what );
            Assert.AreEqual( ps, m.GetSource() );
            Assert.AreEqual( who, mh.xsender );
            Assert.IsNotNull( mh.xmsg );
            Assert.AreEqual( MyValueFactory.mt_add, mh.xmsg.GetXType );
            Assert.AreEqual( 0, mh.xmsg.Count );

        }

        [Test]
        public void Packet3()
        {
            SetUpBeforeEveryTest();

            PacketHandler ph = m;

            FlexBuffer buf = new FlexBuffer( new byte[] { (byte)BinaryTaggedData.VERSION, 
                unchecked((byte)Etch.Transport.Fmt.TypeCode.INT1), 2, unchecked((byte)Etch.Transport.Fmt.TypeCode.NONE) } );

            mh.handled = true;

            ph.Packet( who, buf );

            Assert.AreEqual( What.TESTMESSAGEHANDLERMESSAGE, mh.what );
            Assert.AreEqual( ps, m.GetSource() );
            Assert.AreEqual( who, mh.xsender );
            Assert.IsNotNull( mh.xmsg );
            Assert.AreEqual( MyValueFactory.mt_add_result, mh.xmsg.GetXType );
            Assert.AreEqual( 0, mh.xmsg.Count );

        }

        [Test]
        public void Packet4()
        {
            SetUpBeforeEveryTest();

            PacketHandler ph = m;

            FlexBuffer buf = new FlexBuffer( new byte[] { (byte)BinaryTaggedData.VERSION, 
                unchecked((byte)Etch.Transport.Fmt.TypeCode.INT1), 2, unchecked((byte)Etch.Transport.Fmt.TypeCode.NONE) } );

            mh.handled = false;

            ph.Packet( who, buf );

            Assert.AreEqual( What.TESTMESSAGEHANDLERMESSAGE, mh.what );
            Assert.AreEqual( ps, m.GetSource() );
            Assert.AreEqual( who, mh.xsender );
            Assert.IsNotNull( mh.xmsg );
            Assert.AreEqual( MyValueFactory.mt_add_result, mh.xmsg.GetXType );
            Assert.AreEqual( 0, mh.xmsg.Count );

        }

        //[Test]
        //public void Down() 
        //{
        //    SetUpBeforeEveryTest();
        //    PacketHandler ph = m;

        //    Assert.IsNull( mh.what );
        //    Assert.IsNull( mh.src );

        //    ph.Down(ps);

        //    Assert.AreEqual( mh.src, m );
        //    Assert.AreEqual( mh.what, What.TESTMESSAGEHANDLERDOWN );
        //    Assert.AreEqual( ps, m.GetPacketSource() );
        //}

        #endregion Test PacketHandler methods

        #region MessagizerMessageSource methods

        //[Test]
        //public void Close1() 
        //{
        //    SetUpBeforeEveryTest();

        //    MessagizerMessageSource mms = m;
        //    Assert.IsNull( ps.what );

        //    mms.Close( true );

        //    Assert.AreEqual(ps.what, What.PACKETSOURCECLOSE);
        //    Assert.IsTrue( ps.reset );
        //}

        //[Test]
        //public void Close2()
        //{
        //    SetUpBeforeEveryTest();

        //    MessagizerMessageSource mms = m;
        //    Assert.IsNull( ps.what );

        //    mms.Close( false );

        //    Assert.AreEqual( ps.what, What.PACKETSOURCECLOSE );
        //    Assert.IsFalse( ps.reset );
        //}

        //[Test]
        //public void LocalAddress() 
        //{
        //    SetUpBeforeEveryTest();

        //    MessagizerMessageSource mms = m;
        //    EndPoint isa = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), 23 );
        //    ps.localAddress  =  isa;

        //    Assert.AreEqual( isa, mms.LocalAddress() );
        //}
    	
        //[Test]
        //public void RemoteAddress() 
        //{
        //    SetUpBeforeEveryTest();

        //    MessagizerMessageSource mms = m;
        //    EndPoint isa = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), 23 );
        //    ps.remoteAddress = isa;

        //    Assert.AreEqual( isa, mms.RemoteAddress() );
        //}
    	
        //[Test]
        //public void ShutdownInput() 
        //{
        //    SetUpBeforeEveryTest();

        //    MessagizerMessageSource mms = m;
        //    Assert.IsNull( ps.what );
        //    m.ShutdownInput();
        //    Assert.AreEqual( ps.what, What.PACKETSOURCESHUTDOWNINPUT );
        //}
    	
        //[Test]
        //public void ShutdownOutput() 
        //{
        //    SetUpBeforeEveryTest();

        //    MessagizerMessageSource mms = m;
        //    Assert.IsNull( ps.what );
        //    m.ShutdownOutput();

        //    Assert.AreEqual( ps.what, What.PACKETSOURCESHUTDOWNOUTPUT );
        //}
    	
        //[Test]
        //public void Stop() 
        //{
        //    SetUpBeforeEveryTest();

        //    MessagizerMessageSource mms = m;
        //    Assert.IsNull( ps.what );

        //    mms.Stop();
        //    Assert.AreEqual(ps.what, What.PACKETSOURCESTOP);
        //}

        public void Message1()
        {
            SetUpBeforeEveryTest();
            
            MessagizerMessageSource mms = m;

            Message msg = new Message( MyValueFactory.mt_add, vf );
            ps.headerSize = 0;

            Assert.IsNull( ps.what );

            mms.Messagex( who, msg );

            Assert.AreEqual( What.PACKETSOURCEPACKET, ps.what );
            Assert.AreEqual( who, ps.xrecipient );
            Assert.IsNotNull( ps.xbuf );
            Assert.AreEqual( 3, ps.xbuf.Length );
            Assert.AreEqual( BinaryTaggedData.VERSION, ps.xbuf[ 0 ] );
            Assert.AreEqual( ( byte ) 65, ps.xbuf[ 1 ] );
            Assert.AreEqual( Etch.Transport.Fmt.TypeCode.NONE, ps.xbuf[ 2 ] );
        }

        public void Message2()
        {
            SetUpBeforeEveryTest();

            MessagizerMessageSource mms = m;

            Message msg = new Message( MyValueFactory.mt_add, vf );
            ps.headerSize = 8;

            Assert.IsNull( ps.what );

            mms.Messagex( who, msg );

            Assert.AreEqual( What.PACKETSOURCEPACKET, ps.what );
            Assert.AreEqual( who, ps.xrecipient );
            Assert.IsNotNull( ps.xbuf );
            Assert.AreEqual( 11, ps.xbuf.Length );
            Assert.AreEqual( ( byte ) 0, ps.xbuf[ 0 ] );
            Assert.AreEqual( ( byte ) 0, ps.xbuf[ 1 ] );
            Assert.AreEqual( ( byte ) 0, ps.xbuf[ 2 ] );
            Assert.AreEqual( ( byte ) 0, ps.xbuf[ 3 ] );
            Assert.AreEqual( ( byte ) 0, ps.xbuf[ 4 ] );
            Assert.AreEqual( ( byte ) 0, ps.xbuf[ 5 ] );
            Assert.AreEqual( ( byte ) 0, ps.xbuf[ 6 ] );
            Assert.AreEqual( ( byte ) 0, ps.xbuf[ 7 ] );
            Assert.AreEqual( BinaryTaggedData.VERSION, ps.xbuf[ 8 ] );
            Assert.AreEqual( ( byte ) 65, ps.xbuf[ 9 ] );
            Assert.AreEqual( Etch.Transport.Fmt.TypeCode.NONE, ps.xbuf[ 10 ] );
        }

	    
        #endregion MessagizerMessageSource methods

        // Do following test in stress testing.
	    /*
        [Test]
	    public void SendreceiveMessageIterative() 
	    {
            SetUpBeforeEveryTest();
		    report = false;

            int n = 100000;
    		
		    //long t0 = Timer.Init();
            //Timer t0 = new Timer().Init();
		    for (int i = 0; i < n; i++) {
			    MyMessageHandler.Send( m, ps );
			    Assert.IsTrue(ps.checkFlag);
		    }
		    //long t1 = t0.ElapsedNanos();
		    //double t = (t1-t0) / 1000000000.0;
		    //Console.WriteLine( "took " + t + " to send " + n  + " ( " + n/t + " + per sec)\n" );
	    }
        */

        public enum What
        {
            PACKETSOURCEPACKET,
            TESTMESSAGEHANDLERMESSAGE,
            OOB_QUERY_SOURCE,
		    OOB_CONTROL_SOURCE,
		    OOB_NOTIFY_SOURCE,
		    OOB_QUERY_HANDLER,
		    OOB_CONTROL_HANDLER,
		    OOB_NOTIFY_HANDLER
        };

        #region MyPacketSource
        public class MyPacketSource : PacketSource
        {
            TestMessagizer _tm;

            public What what;
            
            public bool checkFlag = true;
            //public PktSrcMessageHandler mh;
            
            public Messagizer m;

            public PacketHandler handler;
            public int headerSize = 0;
            public bool reset;
            public Who xrecipient;

            public Object xquery;
            /** */
            public Object xquery_result;
            /** */
            public Object xcontrol;
            /** */
            public Object xvalue;
            /** */
            public Object xevent;

            public byte[] xbuf;

            public int HeaderSize()
            {
                return headerSize;
            }

            public MyPacketSource()
            {
                // nothing to do.
            }

            public MyPacketSource( TestMessagizer tm )
            {
                _tm = tm;

            }
            /**
            public class PktSrcMessageHandler : MessageHandler<MessagizerMessageSource>
            {
                MyPacketSource _mps;

                public PktSrcMessageHandler()
                {
                    // nothing to do.
                }

                public PktSrcMessageHandler( MyPacketSource mps )
                {
                    _mps = mps;
                    
                }

                public void Up( MessagizerMessageSource src ) 
                {
                    Console.WriteLine( "up" );
                }

                public bool Message( MessagizerMessageSource src, Who sender, Message msg )
				{
				    if (report)
					    Console.WriteLine( "got reply " + msg );
    				
				    
				    if ( (int)(msg.Get(mf_result)) != 9 ) 
					    _mps.checkFlag = false;

                    return true;
    				
			    }

                public void Down( MessagizerMessageSource src ) 
                {
                    Console.WriteLine( "down" );
                }
            }
    		**/
		    public void Packet( Who recipient, FlexBuffer buf ) 
		    {
                what = What.PACKETSOURCEPACKET;
                this.xrecipient = recipient;
                this.xbuf = buf.GetAvailBytes();
		    }
		    

            #region Transport Members

            public object TransportQuery( object query )
            {
                what = What.OOB_QUERY_SOURCE;
                xquery = query;
                return xquery_result;
            }

            public void TransportControl( object control, object value )
            {
                what = What.OOB_CONTROL_SOURCE;
                xcontrol = control;
                xvalue = value;
            }

            public void TransportNotify( object eventObj )
            {
                what = What.OOB_NOTIFY_SOURCE;
                xevent = eventObj;
            }

            #endregion

            #region Source Members

            public object GetHandler()
            {
                return handler;
            }

            public void SetHandler( object handler )
            {
                this.handler = (PacketHandler)handler;
            }

            #endregion
        }

        #endregion MyPacketSource

        #region MyMessageHandler

        public class MyMessageHandler : MessageHandler<MessagizerMessageSource>
        {
            public Enum what;
            public Who xsender;
            public Message xmsg;
            public bool handled;

            public Object xquery;
            public Object xquery_result;
            public Object xcontrol;
            public Object xvalue;
            public Object xevent;

            

            public bool Message( Who sender, Message msg ) 
		    {

                what = What.TESTMESSAGEHANDLERMESSAGE;
                this.xsender = sender;
                this.xmsg = msg;
                return handled;

                /**
			    if (report)
		    	    Console.WriteLine( "message: "+msg );
    		    
		        if (msg.IsType( mt_add ))
		        {
		    	    int? x = (int?)( msg.Get( mf_x ) );
                    int? y = ( int? ) ( msg.Get( mf_y ) );

                    Int32 r = ( int ) ( x + y );
    		    	
		    	    Message rmsg = msg.Reply( mt_add_result );
		    	    rmsg.Add( mf_result, r );
    		    	
		    	    if (report)
		    		    Console.WriteLine( "replying with "+rmsg );
    		    	
		    	    src.Message( sender, rmsg );
		        }

                return true;
                 * */
	        }

            #region SourceHandler<MessagizerMessageSource> Members

            public MessagizerMessageSource GetSource()
            {
                return null;
            }

            public void SetSource( MessagizerMessageSource src )
            {
                // ignore
            }

            #endregion

            #region Session Members

            public object SessionQuery( object query )
            {
                what = What.OOB_QUERY_HANDLER;
                xquery = query;
                return xquery_result;
            }

            public void SessionControl( object control, object value )
            {
                what = What.OOB_CONTROL_HANDLER;
                xcontrol = control;
                xvalue = value;
            }

            public void SessionNotify( object eventObj )
            {
                what = What.OOB_NOTIFY_HANDLER;
                xevent = eventObj;
            }

            #endregion
        }
        #endregion MyMessageHandler

        public class MyValueFactory : DefaultValueFactory
        {
            private readonly static TypeMap types = new TypeMap();

            private readonly static FieldMap fields = new FieldMap();

            public readonly static XType mt_add = types.Add( new XType( 1, "add" ) );

            public readonly static XType mt_add_result = types.Add( new XType( 2, "add_result" ) );

            public readonly static Field mf_x = fields.Add( new Field( 3, "x" ) );

            public readonly static Field mf_y = fields.Add( new Field( 4, "y" ) );

            public readonly static Field mf_result = fields.Add( new Field( 5, "xresult" ) );

            static MyValueFactory()
            {
                DefaultValueFactory.Init( types, fields );

                mt_add.PutValidator( mf_x, Validator_int.Get( 0 ) );
                mt_add.PutValidator( mf_y, Validator_int.Get( 0 ) );
                mt_add.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );
                
                mt_add_result.PutValidator( mf_result, Validator_int.Get( 0 ) );
                mt_add_result.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );
                mt_add_result.PutValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.Get( 0 ) );

            }

            public override XType GetType( int? id )
            {
                return types.Get( id );
            }

            public override XType GetType( string name )
            {
                return types.Get( name );
            }

            public override ICollection<XType> GetTypes()
            {
                return types.Values();
            }

            public override Field GetField( int? id )
            {
                return fields.Get( id );
            }

            public override Field GetField( string name )
            {
                return fields.Get( name );
            }

            public override ICollection<Field> GetFields()
            {
                return fields.Values();
            }
        }
    }
}
