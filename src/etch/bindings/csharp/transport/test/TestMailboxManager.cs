using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Support;
using NUnit.Framework;
using System.Net;

namespace Etch.Transport.Test
{
    [TestFixture]
    public class TestMailboxManager
    {
        private MailboxManager mmgr;
        private Message msg;
        // private DefaultValueFactory vf = new DefaultValueFactory();
        private MyMessageHandler mh;
        private MyMessagizer m;

        private bool duplexFlag = true;
        private bool registerFlag = false;

        private readonly ValueFactory vf = new MyValueFactory();

        public TestMailboxManager() 
        { 
            //mh = new MyMessageHandler(this);
            //m = new MyMessagizer( this );
        }

        public Message ConstructSimpleMessage()
        {
            Message msg = new Message( MyValueFactory.mt_function, vf );
            return msg;
        }

        public Message ConstructAddMessage() 
	    {
		    msg = new Message( MyValueFactory.mt_add, vf );
		    msg.Add( MyValueFactory.mf_x, 4 );
		    msg.Add( MyValueFactory.mf_y, 5 );
    		
		    return msg;
	    }

        public void SetUp()
        {
            mh = new MyMessageHandler( this );
            
            msg = ConstructAddMessage();
            mmgr = new MailboxManager( mh, 1000 );
            m = new MyMessagizer( this, mmgr );
            mmgr.SetSource( m );
        }

        //[Test]
        //public void Up() 
        //{
        //    SetUp();
        //    Assert.IsNull(mh.what);
        //    mmgr.Up(m);
        //    Assert.AreEqual(mh.what, What.MMGRMESSAGEHANDLERUP);
        //}

        //[Test]
        //public void Down() 
        //{
        //    SetUp();
        //    Assert.IsNull(mh.what);
        //    mmgr.Down(m);
        //    Assert.AreEqual(mh.what, What.MMGRMESSAGEHANDLERDown);
        //}

        //[Test]
        //public void Close() 
        //{
        //    SetUp();
        //    Assert.IsNull(m.what);
        //    mmgr.Close(true);
        //    Assert.AreEqual(m.what, What.MYMESSAGIZERClose);
        //}
    	
        //[Test]
        //public void LocalAddress() 
        //{
        //    SetUp();
        //    Assert.IsNull(m.what);
        //    mmgr.LocalAddress();
        //    Assert.AreEqual(m.what, What.MYMESSAGIZERLocalAddress);
        //}
    	
        //[Test]
        //public void RemoteAddress() 
        //{
        //    SetUp();
        //    Assert.IsNull(m.what);
        //    mmgr.RemoteAddress();
        //    Assert.AreEqual(m.what, What.MYMESSAGIZERRemoteAddress);
        //}
    	
        //[Test]
        //public void ShutdownInput() 
        //{
        //    SetUp();
        //    Assert.IsNull(m.what);
        //    mmgr.ShutdownInput();
        //    Assert.AreEqual(m.what, What.MYMESSAGIZERShutdownInput);
        //}
    	
        //[Test]
        //public void ShutdownOutput() 
        //{
        //    SetUp();
        //    Assert.IsNull(m.what);
        //    mmgr.ShutdownOutput();
        //    Assert.AreEqual(m.what, What.MYMESSAGIZERShutdownOutput);
        //}
    	
        //[Test]
        //public void Stop() 
        //{
        //    SetUp();
        //    Assert.IsNull(m.what);
        //    mmgr.Stop();
        //    Assert.AreEqual(m.what, What.MYMESSAGIZERSTOP);
        //}
    	
	    [Test]
	    public void SimplexMessage() 
	    {
            SetUp();
		    Assert.IsNull(m.what);
    		
		    Message message = ConstructSimpleMessage();
		    mmgr.Messagex( null, message );
    		
		    Assert.AreEqual(m.what, What.MYMESSAGIZERMESSAGE);
	    }
    	
	    [Test]
	    public void DuplexMessage() 
	    {
            SetUp();
		    Assert.IsNull(m.what);
		    Assert.IsNull(mh.what);
    		
		    Message message = ConstructAddMessage();
            mmgr.Message( null, message, 1, 0 );
    		
		    Assert.AreEqual(m.what, What.MYMESSAGIZERMESSAGE);
    		
		    /* 
		     * Because PlainMailbox will intercept messages ...
		     * The Default messagehandler won't be called.
		     */
		    Assert.IsNull(mh.what);
    		
    		
	    }
    	
	    [Test]
	    public void DuplicateRegistration() 
	    {
            SetUp();
		    Message message = ConstructAddMessage();
		    Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

		    /*
		     * Check for duplicate registration
		     */
		    try 
		    {
			    mmgr.Register(mb);
		    }
		    catch (Exception)
		    {
			    registerFlag = true;
		    }
		    Assert.IsTrue(registerFlag);
    		
	    }
    	
	    [Test]
	    public void TestUnregistration() 
	    {
            SetUp();
		    Message message = ConstructAddMessage();
		    Mailbox mb =  mmgr.Message( null, message, 1, 0 );

		    /*
		     * Unregister and then register again ... 
		     */
		    mmgr.Unregister(mb);
            registerFlag = false;
		    try 
		    {
                mmgr.Register( ( Mailbox ) mb );
		    }
		    catch (Exception)
		    {
                registerFlag = true;
		    }
		    Assert.IsFalse(registerFlag);
    	
	    }
    	
	    [Test]
	    public void NoMailboxOrReplyToId() 
	    {
            SetUp();
		    Assert.IsNull(mh.what);
		    Assert.IsNull(m.what);
    		
		    duplexFlag = false;
		    Message message = ConstructAddMessage();
    		
		    m.Messagex(null, message);
    		
		    Assert.AreEqual(m.what, What.MYMESSAGIZERMESSAGE);
		    /*
		     * Since no mailbox, default message handler receives
		     * messages.
		     */
		    Assert.AreEqual(mh.what, What.MMGRMESSAGEHANDLERMESSAGE);
	    }

        public enum What
        {
            MMGRMESSAGEHANDLERSTARTED,
            MMGRMESSAGEHANDLERUP,
            MMGRMESSAGEHANDLERDown,
            MMGRMESSAGEHANDLERSTOPPED,
            MYMESSAGIZERClose,
            MYMESSAGIZERLocalAddress,
            MYMESSAGIZERRemoteAddress,
            MYMESSAGIZERShutdownInput,
            MYMESSAGIZERShutdownOutput,
            MYMESSAGIZERSTOP,
            MYMESSAGIZERMESSAGE,
            MMGRMESSAGEHANDLERMESSAGE, 
            OOB_CONTROL_HANDLER, 
            OOB_NOTIFY_HANDLER, 
            OOB_QUERY_HANDLER
        };

        public class MyMessageHandler : MessageHandler<MailboxMessageSource>
        {
            public Enum what;

            public Who xsender;

            public Message xmsg;

            public Object xcontrol;

            public Object xvalue;

            public Object xquery;

            public Object xevent;

            public Object xquery_result;

            public TestMailboxManager _tmm;

            public MyMessageHandler()
            {
                // nothing to do.
            }

            public MyMessageHandler( TestMailboxManager tmm )
            {
                _tmm = tmm;
            }
    		
		    public bool Message( Who sender, Message msg ) 
		    {
			    what = What.MMGRMESSAGEHANDLERMESSAGE;
                xsender = sender;
                xmsg = msg;
                return true;
		    }

            #region SourceHandler<MailboxMessageSource> Members

            public MailboxMessageSource GetSource()
            {
                // ignore
                return null;
            }

            public void SetSource( MailboxMessageSource src )
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

        public class MyMessagizer : PacketHandler, MessagizerMessageSource
        {
            private MessageHandler handler;
    		
		    public Enum what;

            public TestMailboxManager _tmm;

            public MyMessagizer( TestMailboxManager tmm )
            {
                _tmm = tmm;
            }

            public MyMessagizer( TestMailboxManager tmm, MessageHandler handler)
            {
                _tmm = tmm;
                this.handler = handler;
            }

            public override string ToString()
            {
                return "My Messagizer";
            }

            public void Packet( PacketSource src, Who sender, FlexBuffer buf ) 
		    {
			    
			    // ignore
		    }

            public void Messagex( Who recipient, Message msg ) 
		    {
			    // message from mailbox manager
			    what = What.MYMESSAGIZERMESSAGE;
    			
			    // send message back ... 
                if ( _tmm.duplexFlag == true )
                    msg.InReplyTo = msg.MessageId;
				    
			    handler.Message(null, msg);
		    }

            #region PacketHandler Members

            public void Packet( Who sender, FlexBuffer buf )
            {
                throw new Exception( "The method or operation is not implemented." );
            }

            #endregion

            #region SourceHandler<PacketSource> Members

            public PacketSource GetSource()
            {
                return null;
            }

            public void SetSource( PacketSource src )
            {
                // ignore
            }

            #endregion

            #region Session Members

            public object SessionQuery( object query )
            {
                return null;
            }

            public void SessionControl( object control, object value )
            {
                // ignore
            }

            public void SessionNotify( object eventObj )
            {
                // ignore
            }

            #endregion

            #region Source Members

            public object GetHandler()
            {
                return null;
            }

            public void SetHandler( object handler )
            {
                // ignore
            }

            #endregion

            #region Transport Members

            public object TransportQuery( object query )
            {
                return null;
            }

            public void TransportControl( object control, object value )
            {
                // ignore
            }

            public void TransportNotify( object eventObj )
            {
                // ignore
            }

            #endregion
        }

        public class MyValueFactory : DefaultValueFactory
        {
            private readonly static TypeMap types = new TypeMap();

            private readonly static FieldMap fields = new FieldMap();

            public readonly static XType mt_function = types.Get( "function" );

            public readonly static XType mt_add = types.Get( "add" );

            public readonly static XType mt_add_result = types.Get( "add_result" );

            public readonly static Field mf_x = fields.Get( "x" );

            public readonly static Field mf_y = fields.Get( "y" );

            public readonly static Field mf_result = fields.Get( "xresult" );

            static MyValueFactory()
            {
                DefaultValueFactory.Init( types, fields );

                mt_function.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );
                mt_function.PutValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.Get( 0 ) );

                mt_add.PutValidator( mf_x, Validator_int.Get( 0 ) );
                mt_add.PutValidator( mf_y, Validator_int.Get( 0 ) );
                mt_add.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );
                mt_add.PutValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.Get( 0 ) );

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
