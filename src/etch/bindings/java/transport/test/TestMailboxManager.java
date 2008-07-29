package etch.bindings.java.transport.test;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;
import metreos.core.Who;
import metreos.core.io.PacketHandler;
import metreos.core.io.PacketSource;
import metreos.util.FlexBuffer;

import org.junit.Before;
import org.junit.Test;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator_int;
import etch.bindings.java.msg.Validator_long;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.support.Mailbox;
import etch.bindings.java.transport.MailboxManager;
import etch.bindings.java.transport.MailboxMessageSource;
import etch.bindings.java.transport.MessageHandler;
import etch.bindings.java.transport.MessagizerMessageSource;

public class TestMailboxManager 
{

	private MailboxManager mmgr;
	private Message msg;
//	private DefaultValueFactory vf = new DefaultValueFactory();
	private MyMessageHandler mh = new MyMessageHandler();
	private MyMessagizer m;
	
	/** eh */
	boolean duplexFlag = true;
	private boolean registerFlag = false;
	
	public Message constructSimpleMessage() throws Exception
	{
		Type mt_function = new Type( "function" );

		mt_function.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
		mt_function.putValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.get( 0 ) );
		
		DefaultValueFactory vf = new DefaultValueFactory();
		vf.addType(mt_function);
		
		msg = new Message(mt_function, vf);
		return msg;
	}
	
	public Message constructAddMessage() throws Exception 
	{
		Type mt_add = new Type( "add" );
		Type mt_add_result = new Type( "add_result" );
		
		Field mf_x = new Field( "x" );
		Field mf_y = new Field( "y" );
		Field mf_result = new Field( "result" );

		mt_add.putValidator( mf_x, Validator_int.get( 0 ) );
		mt_add.putValidator( mf_y, Validator_int.get( 0 ) );
		mt_add.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
		mt_add.putValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.get( 0 ) );
		
		mt_add_result.putValidator( mf_result, Validator_int.get( 0 ) );
		mt_add_result.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
		mt_add_result.putValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.get( 0 ) );
		
		DefaultValueFactory vf = new DefaultValueFactory();
		vf.addType( mt_add );
		vf.addType( mt_add_result );
		vf.addField( mf_x );
		vf.addField( mf_y );
		vf.addField( mf_result );
		
		msg = new Message( mt_add, vf );
		msg.put( mf_x, 4 );
		msg.put( mf_y, 5 );
		
		return msg;
	}
	
	@Before
	public void setUp() throws Exception
	{
		msg = constructAddMessage();
		mmgr = new MailboxManager(mh, 1000);
		m = new MyMessagizer(mmgr);
		mmgr.setSource(m);
	}
	
//	@Test
//	public void up() throws Exception
//	{
//		assertNull(mh.what);
//		mmgr.up(m);
//		assertSame(mh.what, What.MMGRMESSAGEHANDLERUP);
//	}
	
//	@Test
//	public void down() throws Exception
//	{
//		assertNull(mh.what);
//		mmgr.down(m);
//		assertSame(mh.what, What.MMGRMESSAGEHANDLERDOWN);
//	}
	
//	@Test
//	public void close() throws Exception
//	{
//		assertNull(m.what);
//		mmgr.close(true);
//		assertSame(m.what, What.MYMESSAGIZERCLOSE);
//	}
	
//	@Test
//	public void localAddress() throws Exception
//	{
//		assertNull(m.what);
//		mmgr.localAddress();
//		assertSame(m.what, What.MYMESSAGIZERLOCALADDRESS);
//	}
	
//	@Test
//	public void remoteAddress() throws Exception
//	{
//		assertNull(m.what);
//		mmgr.remoteAddress();
//		assertSame(m.what, What.MYMESSAGIZERREMOTEADDRESS);
//	}
	
//	@Test
//	public void shutdownInput() throws Exception
//	{
//		assertNull(m.what);
//		mmgr.shutdownInput();
//		assertSame(m.what, What.MYMESSAGIZERSHUTDOWNINPUT);
//	}
	
//	@Test
//	public void shutdownOutput() throws Exception
//	{
//		assertNull(m.what);
//		mmgr.shutdownOutput();
//		assertSame(m.what, What.MYMESSAGIZERSHUTDOWNOUTPUT);
//	}
	
//	@Test
//	public void stop() throws Exception
//	{
//		assertNull(m.what);
//		mmgr.stop();
//		assertSame(m.what, What.MYMESSAGIZERSTOP);
//	}
	
	@Test
	public void simplexMessage() throws Exception
	{
		assertNull(m.what);
		
		Message message = constructSimpleMessage();
		mmgr.messagex( null, message );
		
		assertSame(m.what, What.MYMESSAGIZERMESSAGE);
	}
	
	@Test
	public void duplexMessage() throws Exception
	{
		
		assertNull(m.what);
		assertNull(mh.what);
		
		Message message = constructAddMessage();
		mmgr.message(null, message, 1, 0);
		
		assertSame(m.what, What.MYMESSAGIZERMESSAGE);
		
		/* 
		 * Because PlainMailbox will intercept messages ...
		 * The Default messagehandler won't be called.
		 */
		assertNull(mh.what);
		
		
	}
	
	@Test
	public void duplicateRegistration() throws Exception
	{
		Message message = constructAddMessage();
		Mailbox mb = mmgr.message(null, message, 1, 0);

		/*
		 * Check for duplicate registration
		 */
		try 
		{
			mmgr.register(mb);
		}
		catch (IllegalArgumentException e)
		{
			registerFlag = true;
		}
		assertTrue(registerFlag);
		
	}
	
	@Test
	public void testUnregistration() throws Exception
	{
		Message message = constructAddMessage();
		Mailbox mb = mmgr.message(null, message, 1, 0);

		/*
		 * Unregister and then register again ... 
		 */
		mmgr.unregister(mb);
		
		try 
		{
			mmgr.register(mb);
		}
		catch (IllegalArgumentException e)
		{
			registerFlag = true;
		}
		assertFalse(registerFlag);
	
	}
	
	@Test
	public void noMailboxOrReplyToId() throws Exception
	{
		
		assertNull(mh.what);
		assertNull(m.what);
		
		duplexFlag = false;
		Message message = constructAddMessage();
		
		m.messagex(null, message);
		
		assertSame(m.what, What.MYMESSAGIZERMESSAGE);
		/*
		 * Since no mailbox, default message handler receives
		 * messages.
		 */
		assertSame(mh.what, What.MMGRMESSAGEHANDLERMESSAGE);
	}
	
	public enum What {MMGRMESSAGEHANDLERSTARTED, MMGRMESSAGEHANDLERUP, MMGRMESSAGEHANDLERDOWN, 
		MMGRMESSAGEHANDLERSTOPPED, MYMESSAGIZERCLOSE, MYMESSAGIZERLOCALADDRESS, 
		MYMESSAGIZERREMOTEADDRESS, MYMESSAGIZERSHUTDOWNINPUT, MYMESSAGIZERSHUTDOWNOUTPUT, 
		MYMESSAGIZERSTOP, MYMESSAGIZERMESSAGE, MMGRMESSAGEHANDLERMESSAGE, OOB_CONTROL_HANDLER, OOB_NOTIFY_HANDLER, OOB_QUERY_HANDLER};
	
	public class MyMessageHandler implements MessageHandler<MailboxMessageSource>
	{
		public What what;
		
		public Who xsender;
		
		public Message xmsg;
		
		public Object xcontrol;
		
		public Object xvalue;
		
		public Object xquery;
		
		public Object xevent;
		
		public Object xquery_result;
		
		public MyMessageHandler() 
		{
			// nothing to do
		}
		
		public boolean message( Who sender, Message msg ) throws Exception
		{
			what = What.MMGRMESSAGEHANDLERMESSAGE;
			xsender = sender;
			xmsg = msg;
			return true;
		}

		public MailboxMessageSource getSource()
		{
			// ignore.
			return null;
		}

		public void setSource( MailboxMessageSource src )
		{
			// ignore.
		}

		public Object sessionQuery( Object query )
		{
			what = What.OOB_QUERY_HANDLER;
			xquery = query;
			return xquery_result;
		}

		public void sessionControl( Object control, Object value )
		{
			what = What.OOB_CONTROL_HANDLER;
			xcontrol = control;
			xvalue = value;
		}

		public void sessionNotify( Object event )
		{
			what = What.OOB_NOTIFY_HANDLER;
			xevent = event;
		}
	}
	
	public class MyMessagizer implements PacketHandler, MessagizerMessageSource 
	{
		private final MessageHandler handler;
		
		private PacketSource pktSrc;
		
		public What what;
		
		public MyMessagizer( MessageHandler handler )
		{
			this.handler = handler;
		}
		
		@Override
		public String toString()
		{
			return String.format( "myMessagizer/%s", pktSrc );
		}
		
		public void packet( Who sender, FlexBuffer buf ) throws Exception
		{
			// ignore.
		}
		
		
		public void messagex( Who recipient, Message msg ) throws Exception
		{
			// message from mailbox manager.
			
			what = What.MYMESSAGIZERMESSAGE;
			
			// send message back ... 
			if (duplexFlag == true)
				msg.setInReplyTo(msg.getMessageId());
			
			handler.message(null, msg);
		}

		public PacketSource getSource()
		{
			// TODO Auto-generated method stub
			return null;
		}

		public void setSource( PacketSource src )
		{
			// TODO Auto-generated method stub
			
		}

		public Object sessionQuery( Object query )
		{
			// TODO Auto-generated method stub
			return null;
		}

		public void sessionControl( Object control, Object value )
		{
			// TODO Auto-generated method stub
			
		}

		public void sessionNotify( Object event )
		{
			// TODO Auto-generated method stub
			
		}

		public MessageHandler<MessagizerMessageSource> getHandler()
		{
			// TODO Auto-generated method stub
			return null;
		}

		public void setHandler( MessageHandler<MessagizerMessageSource> handler )
		{
			// TODO Auto-generated method stub
			
		}

		public Object transportQuery( Object query ) throws Exception
		{
			// TODO Auto-generated method stub
			return null;
		}

		public void transportControl( Object control, Object value ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public void transportNotify( Object event ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}
	}
}
