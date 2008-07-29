package etch.examples.log.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import etch.examples.authentication.AuthDb;
import etch.examples.authentication.Authenticator.Failure;
import etch.examples.configuration.Configuration;
import etch.examples.log.Log;
import etch.examples.log.LogHelper;
import etch.examples.log.LogServer;
import etch.examples.log.LogSink;

/**
 * JUnit Tests for testing RemoteLogServer
 *
 *
 * @author gsandhir
 */


public class TestRemoteLogServer
{

	private static int iWaitPeriod = 4000;
	private String myHost;
	private int iMyPort;
	private int iMyDelay;
	private String myInstance;
	private AuthDb myAuthDb;
	private ListenerHandler myListenerHandler;
	private Listener myListener;
	private LogServer logServer;

	/**
	 * Setup Common Data.
	 *
	 * @throws Exception
	 */
	@Before
	public void setupCommonData()
		throws Exception
	{
		myHost = "localhost";
		iMyPort = 4006;
		iMyDelay = 0;
		myInstance = LogHelper.LOCAL;

	}

	/**
	 * Stop the server if needed.
	 *
	 * @throws Exception
	 */
	@After
	public void teardownServer()
		throws Exception
	{
		if (myListener != null && myListener.isStarted())
		{
			myListener.stop();
			myListener.waitDown(iWaitPeriod);
		}
	}

	/**
	 *  Tests to make sure a user can login successfully.
	 *
	 * @throws Exception
	 */
	@Test
	public void successfulLogin()
		throws Exception
	{
		startServer();

		logServer.login( "Fred", "cisco123" );

		assertTrue(logServer.isLoggedIn());
	}

	/**
	 *  Test to set log level
	 *
	 * @throws Exception
	 */

	@Test
	public void setLogLevel()
		throws Exception
	{
		startServer();

		logServer.login( "Fred", "cisco123" );

		logServer.setLevel( Log.WARNING );

		assertEquals(Log.WARNING, logServer.getLevel());
	}

	/**
	 *  Test to print log
	 *
	 * @throws Exception
	 */

	@Test
	public void print()
		throws Exception
	{
		startServer();

		logServer.login( "Fred", "cisco123" );

		logServer.log( Log.WARNING, "warning msg 1" );
	}

	/**
	 *  Setup Authentication DB
	 *
	 * @throws Exception
	 */

	private AuthDb setupAuthDb() throws Failure
	{
		AuthDb auth = new AuthDb();

		auth.addUser( "Fred", "cisco123" );
		auth.grantRight( "Fred", "Log.write" );


		auth.addUser( "Mary", "cisco456" );
		auth.grantRight( "Mary", "Auth.admin" );
		auth.grantRight( "Mary", "Auth.check" );

		auth.addUser( "Joe", "cisco789" );
		auth.grantRight( "Joe", Configuration.READ_RIGHT );

		return auth;
	}

	/**
	 *  Test to setup LogSink
	 *
	 * @throws Exception
	 */

	private LogSink setupLogSink()
	{

		LogSink lsink = new LogSink()
		{
			public void write( int level, String msg )
			{
				metreos.util.Log.report( "MESSAGE", "level", level, "msg", msg );
			}
		};
		return lsink;
	}

	/**
	 * Call this method after you have instantiated and filled myUsers.  This
	 * will fill the AuthDb and start LogServer listening.
	 *
	 * @throws Exception
	 */
	private void startServer()
		throws Exception
	{
		myListenerHandler = LogHelper.newListenerHandler(setupAuthDb(),setupLogSink());
		myListener = new Listener(myListenerHandler, 3, myHost, iMyPort, iMyDelay);
		myListener.start();
		myListener.waitUp(iWaitPeriod);

		logServer = LogHelper.getLogInstance(myHost, iMyPort, myInstance);
	}

}
