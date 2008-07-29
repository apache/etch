package etch.bindings.java.transport.test;

import static org.junit.Assert.assertTrue;
import metreos.core.Who;

import org.junit.Before;
import org.junit.Test;

import etch.bindings.java.msg.Message;
import etch.bindings.java.support.Mailbox;
import etch.bindings.java.transport.MailboxManager;
import etch.bindings.java.transport.MailboxMessageSource;
import etch.bindings.java.transport.MessageHandler;
import etch.bindings.java.transport.PlainMailbox;

public class TestPlainMailbox 
{
	private MyMessageHandler mh = new MyMessageHandler();
	private MailboxManager mgr = new MailboxManager(mh, 0);
	private Long dummyMsgId = new Long(1);
	private Mailbox mb = new PlainMailbox(mgr, dummyMsgId, 0, 0, 1);
	
	private boolean registerFlag = true;
	
	@Before
	public void setUp() throws Exception 
	{
		/*mgr.register(mb);*/
	}

	@Test
	public void close() throws Exception
	{
		/*
		 * The idea is to check whether the mailboxes 
		 * are getting unregistered properly
		 * 
		 * First register
		 */
		try 
		{
			mgr.register(mb);
		}
		catch (IllegalArgumentException e)
		{
			registerFlag = false;
		}
		
		assertTrue(registerFlag);
		
		mb.closeDelivery();
		
		try 
		{
			mgr.register(mb);
		}
		catch (IllegalArgumentException e)
		{
			registerFlag = false;
		}
		
		/*
		 * meaning that the mailboxes are unregistered successfully
		 */
		assertTrue(registerFlag);
		
	}
	
	public class MyMessageHandler implements MessageHandler<MailboxMessageSource>
	{
//		public What what;
		
		public MyMessageHandler() 
		{
			// nothing to do
		}
		
		public boolean message( Who sender, Message msg ) throws Exception
		{
			/*System.out.println("MyMessageHandler.message() ... ");*/
			return true;
		}

		public MailboxMessageSource getSource()
		{
			// TODO Auto-generated method stub
			return null;
		}

		public void setSource( MailboxMessageSource src )
		{
			// TODO Auto-generated method stub
			
		}

		public void sessionControl( Object control, Object value )
		{
			// TODO Auto-generated method stub
			
		}

		public void sessionNotify( Object event )
		{
			// TODO Auto-generated method stub
			
		}

		public Object sessionQuery( Object query )
		{
			// TODO Auto-generated method stub
			return null;
		}
	}
	
	
	/*public class MyMailboxManager extends MailboxManager implements MessageHandler<MessagizerMessageSource>, MailboxMessageSource
	{
		public MyMailboxManager() 
		{
			super(null, 0);
		}
		
		public void started( MessagizerMessageSource src )
		{
			System.out.println("MyMailboxManager.started() ... ");
		}

		public void up( MessagizerMessageSource src ) throws Exception
		{
			System.out.println("MyMailboxManager.up() ... ");
		}
		
		public void message( MessagizerMessageSource src, Who sender,
			Message msg ) throws Exception
		{
			System.out.println("MyMailboxManager.message() ... ");
		}
		
		public void down( MessagizerMessageSource src ) throws Exception
		{
			System.out.println("MyMailboxManager.down() ... ");
		}
		
		public void stopped( MessagizerMessageSource src )
		{
			System.out.println("MyMailboxManager.stopped() ... ");
		}

		public void exception( MessagizerMessageSource src, String what, Exception e )
		{
			System.out.println("MyMailboxManager.exception() ... ");
		}
		
		public void message( Who recipient, Message msg ) throws Exception
		{
			System.out.println("MyMailboxManager.message() ''simple send'' ... ");
		}

		public Mailbox message( Who recipient, Message msg, int maxMessages,
			int lifetime ) throws Exception
		{
			System.out.println("MyMailboxManager.message() ''expect return''... ");
			return null;
		}
		
		public void close( boolean reset ) throws Exception
		{
			System.out.println("MyMailboxManager.close() ...");
		}

		public SocketAddress localAddress() throws Exception
		{
			System.out.println("MyMailboxManager.localAddress() ...");
			return null;
		}

		public SocketAddress remoteAddress() throws Exception
		{
			System.out.println("MyMailboxManager.remoteAddress() ...");
			return null;
		}

		public void shutdownInput() throws Exception
		{
			System.out.println("MyMailboxManager.shutdownInput() ...");
		}

		public void shutdownOutput() throws Exception
		{
			System.out.println("MyMailboxManager.shutdownOutput() ...");
		}

		public void stop() throws Exception
		{
			System.out.println("MyMailboxManager.stop() ...");
		}
		
		public void register( Mailbox mb )
		{
			System.out.println("MyMailboxManager.register() ... ");
		}

		*//**
		 * Removes the mailbox from the set of mailbox receiving responses
		 * to messages.
		 * @param mb
		 *//*
		public void unregister( Mailbox mb )
		{
			System.out.println("MyMailboxManager.unregister() ... ");
		}
		
		private Mailbox getMailbox( Long msgid )
		{
			System.out.println("MyMailboxManager.getMailbox() ... ");
			return null;
		}
	}*/
}
