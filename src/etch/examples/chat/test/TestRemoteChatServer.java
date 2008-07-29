package etch.examples.chat.test;

// import static org.junit.Assert.*; 

import org.junit.After;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import etch.examples.chat.ChatClient;
import etch.examples.chat.ChatHelper;
import etch.examples.chat.ImplChatClient;
import etch.examples.chat.MainChatListener;
import etch.examples.chat.RemoteChatServer;
import etch.examples.chat.Chat.Failure;
import etch.util.Log;
import etch.util.core.io.Transport;

/**
 * Tests RemoteChatServer.
 */
public class TestRemoteChatServer
{
	final String user = "user1";
	final String nullUser = null;
	final String badUser = "baduser";
	final String badPass = "badPass"; 
	RemoteChatServer cs;

	/**
	 * Start remote chat listener
	 * @throws Exception
	 */
	@BeforeClass
	public static void startRemoteChatListener() throws Exception 
	{
		Log.addSink(null);
		Log.report("StartRemoteChatListener");
		
		String uri = "tcp://localhost:4005";
		MainChatListener implFactory = new MainChatListener(); 
		listener = ChatHelper.newListener( uri, null, implFactory );
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		Log.report( "StartRemoteChatListener",
			"status", "up",
			"listener", listener );
	}
	
	/**
	 * @throws Exception
	 */
	@AfterClass
	public static void stopRemoteChatListener() throws Exception
	{
		if (listener != null)
		{
			listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
			listener = null;
		}
	}
	
	private static Transport<?> listener;
	
	/**
	 * testLogin1: Login user
	 * @throws Exception
	 */
	@Test
	public void testLogin1() throws Exception
	{		
		Log.report("TestRemoteChatServer.login: testLogin1 - Login user");
		cs = setupAndConnectUser(user);
		cs.login(user,user);
	}
	
	/**
	 * testLogin2: User is already logged in
	 * @throws Exception
	 */
	@Test(expected = Failure.class)
	public void testLogin2() throws Exception
	{
		Log.report("TestRemoteChatServer.login: testLogin2 - Login user. User is already logged in");
		cs = setupAndConnectUser(user);
		cs.login(user,user);
		cs.login(user,user);
	}
	
	/**
	 * testLogin3: Login - user is null
	 * @throws Exception
	 */
	@Test(expected = Failure.class)
	public void testLogin3() throws Exception
	{		
		Log.report("TestRemoteChatServer.login: testLogin3 - Login user. User is null");
		cs = setupAndConnectUser(nullUser);
		cs.login(nullUser,nullUser);
	}
	
	/**
	 * testLogin4: Login - bad password
	 * @throws Exception
	 */
	@Test(expected = Failure.class)
	public void testLogin4() throws Exception
	{		
		Log.report("TestRemoteChatServer.login: testLogin4 - Login user. Bad password");
		cs = setupAndConnectUser(user);
		cs.login(user,badPass);
	}
	
	/**
	 * testLogin5: Login - bad username
	 * @throws Exception
	 */
	@Test(expected = Failure.class)
	public void testLogin5() throws Exception
	{		
		Log.report("TestRemoteChatServer.login: testLogin5 - Login user. Bad username");
		cs = setupAndConnectUser(user);
		cs.login(user,badUser);
	}
	
	/**
	 * Logout out user and close TCP conection
	 * @throws Exception 
	 */
	@After
	public void cleanup() throws Exception {
		if (cs != null)
		{
			cs.logout();
			cs._stop();
		}
	}
	
	/**
	 * @param user
	 * @return chat server
	 * @throws Exception
	 */
	public RemoteChatServer setupAndConnectUser (final String user) throws Exception
	{
		String uri = "tcp://localhost:4005";
		
//		ChatClientImpl client = new ChatClientImpl(user, false);
		RemoteChatServer server = ChatHelper.newServer( uri, null,
			new ChatHelper.ChatClientFactory()
			{
				public ChatClient newChatClient( RemoteChatServer server )
					throws Exception
				{
					return new ImplChatClient( server, user, false );
				}
			} );
//		client._setServer(server);
	
		server._startAndWaitUp( 4000 );
		
		return server;
	}
}
