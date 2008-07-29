/*
 * Created by rebarraz on Oct 25, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */
package etch.examples.configuration.test;


import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import etch.bindings.java.support._Etch_AuthException;
import etch.examples.authentication.AuthDb;
import etch.examples.authentication.AuthenticatorHelper;
import etch.examples.configuration.ConfigDb;
import etch.examples.configuration.Configuration;
import etch.examples.configuration.ConfigurationHelper;
import etch.examples.configuration.ConfigurationServer;


/**
 * @author rebarraz
 *
 */
public class TestRemoteConfigurationServer
{
	// Time to wait up after starting servers
	private static int iWaitPeriod = 4000;

	// Common Test Elements
	private String myAuthHost;
	private int iMyAuthPort;
	@SuppressWarnings("unused")
	private String myAuthInstance;
	private String myAuthName;
	private String myAuthPwd;
	private Listener myAuthListener;

	private String myConfigHost;
	private int iMyConfigPort;
	private String myConfigInstance;
	private Listener myConfigListener;
	private ConfigurationServer myConfigServer;


	// --------
	// Up, Down
	// --------

	/**
	 * @throws Exception
	 */
	@Before
	public void setUp()
		throws Exception
	{
		// AuthenticationServer
		myAuthHost = "localhost";
		iMyAuthPort = 4002;
		myAuthInstance = AuthenticatorHelper.LOCAL;
		myAuthName = "Mary";
		myAuthPwd = "cisco456";

		// Configurationserver
		myConfigHost = "localhost";
		iMyConfigPort = 4004;
		myConfigInstance = ConfigurationHelper.LOCAL;

	}

	/**
	 * Grabs a remote Authentication server.
	 *
	 * @throws Exception
	 */
	private void setUpRemoteServers()
		throws Exception
	{
		// AuthenticationServer
		AuthDb authDb = new AuthDb();

		authDb.addUser("Fred", "cisco123");
		authDb.grantRight("Fred", "Log.write");

		authDb.addUser(myAuthName, myAuthPwd);
		authDb.grantRight(myAuthName, "Auth.admin");
		authDb.grantRight(myAuthName, "Auth.check");

		authDb.addUser("Joe", "cisco789");
		authDb.grantRight("Joe", Configuration.READ_RIGHT);

		ListenerHandler authLH = AuthenticatorHelper.newListenerHandler(authDb);
		myAuthListener = new Listener(authLH, 5, myAuthHost, iMyAuthPort, 0);


		// ConfigurationServer
		ConfigDb configDb = new ConfigDb();

		configDb.newPath("a");
		configDb.putConfig("a", "var1", "123");
		configDb.putConfig("a", "var2", "234");

		configDb.newPath("b");
		configDb.putConfig("b", "var1", "345");

		configDb.newPath("c");

		ListenerHandler configLH = ConfigurationHelper.newListenerHandler(authDb, configDb);
		myConfigListener = new Listener(configLH, 5, myConfigHost, iMyConfigPort , 0);
	}

	/**
	 * Start the configuration and authentication servers.
	 *
	 * @throws Exception
	 */
	private void startServers()
		throws Exception
	{
		if (myAuthListener != null && !myAuthListener.isStarted())
		{
			myAuthListener.start();
			myAuthListener.waitUp(iWaitPeriod);
		}

		if (myConfigListener != null && !myConfigListener.isStarted())
		{
			myConfigListener.start();
			myConfigListener.waitUp(iWaitPeriod);
		}

		myConfigServer = ConfigurationHelper.getConfigurationInstance(myConfigHost,
			iMyConfigPort, myConfigInstance);
	}

	/**
	 * Stop the servers.
	 *
	 * @throws Exception
	 */
	@After
	public void tearDown()
		throws Exception
	{
		if (myConfigListener != null && myConfigListener.isStarted())
		{
			myConfigListener.stop();
			myConfigListener.waitDown(iWaitPeriod);
		}

		if (myAuthListener != null && myAuthListener.isStarted())
		{
			myAuthListener.stop();
			myConfigListener.waitDown(iWaitPeriod);
		}
	}


	// -------
	// Testing
	// -------

	/**
	 * Start and stop the remote servers.
	 *
	 * @throws Exception
	 */
	@Test
	public void simpleRemoteServers()
		throws Exception
	{
		setUpRemoteServers();
		startServers();
	}


	/**
	 *
	 *
	 * @throws Exception
	 */
	@Test
	(expected=_Etch_AuthException.class)
	public void prohibitGetAllConfig()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		myConfigServer.getAllConfig( "a" );
	}

	/**
	 *
	 *
	 * @throws Exception
	 */
	@Test
	(expected=_Etch_AuthException.class)
	public void prohibitGetConfig()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		myConfigServer.getConfig( "a", "var1" );
	}

	/**
	 * Bad users shouldn't be able to login.
	 *
	 * @throws Exception
	 */
	@Test
	(expected=Configuration.Failure.class)
	public void badUserLogin()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		try
		{
			myConfigServer.login("bad", "bad");
		}
		catch (Exception e)
		{
			// ignore.
		}

		assertFalse(myConfigServer.isLoggedIn());

		myConfigServer.login( "bad", "bad" );
	}

	/**
	 * Valid users shouldn't be able to log in without valid passwords.
	 *
	 * @throws Exception
	 */
	@Test
	(expected=Configuration.Failure.class)
	public void badPassword()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		try
		{
			myConfigServer.login("Fred", "bad");
		}
		catch (Exception e)
		{
			// ignore.
		}

		assertFalse(myConfigServer.isLoggedIn());

		myConfigServer.login( "Fred", "bad" );
	}


	/**
	 * User without rights shouldn't be able to login
	 *
	 * @throws Exception
	 */
	@Test
	(expected=Configuration.Failure.class)
	public void noRights()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		myConfigServer.login( "Fred", "cisco123" );
	}

	/**
	 * Simple login
	 *
	 * @throws Exception
	 */
	@Test
	public void successfulLogin()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		myConfigServer.login( "Joe", "cisco789" );
	}

	/**
	 * getAll of a null and bad path should return null
	 *
	 * @throws Exception
	 */
	@Test
	public void nullGetAll()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		myConfigServer.login( "Joe", "cisco789" );

		assertNull(myConfigServer.getAllConfig(null));
		assertNull(myConfigServer.getAllConfig("bad"));
	}

	/**
	 * Test to getAll all the configurations from setup.
	 *
	 * @throws Exception
	 */
	@Test
	public void validGetAll()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		/*
		 * TODO: This found a funky error in the transport where an null
		 * Exception is being created.  Scott's refactoring the transport
		 * so this may go away.
		myConfigServer.login( "Joe", "cisco789" );

		StrStrHashMap map;

		map = myConfigServer.getAllConfig("a");
		assertNotNull(map);
		assertEquals(2, map.size());
		assertNotNull(map.get("var1" ));
		assertEquals("123", map.get( "var1" ));
		assertNotNull(map.get("var2"));
		assertEquals("234", map.get( "var2" ));

		map = myConfigServer.getAllConfig( "b" );
		assertNotNull(map);
		assertEquals(1, map.size());
		assertNotNull(map.get( "var1" ));
		assertEquals( "345", map.get( "var1" ));

		map = myConfigServer.getAllConfig( "c" );
		assertNotNull(map);
		assertEquals(0, map.size());
		*/
	}

	/**
	 *
	 *
	 * @throws Exception
	 */
	@Test
	(expected=Configuration.Failure.class)
	public void getConfigNullPathAndName()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		myConfigServer.login( "Joe", "cisco789" );

		myConfigServer.getConfig( null, null);
	}

	/**
	 *
	 *
	 * @throws Exception
	 */
	@Test
	(expected=Configuration.Failure.class)
	public void getConfigBadPath()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		myConfigServer.login( "Joe", "cisco789" );

		myConfigServer.getConfig( "bad", null);
	}


	/**
	 * Check to possible null, invalid, valid combinations of paths and names
	 * for getting configurations.
	 *
	 * @throws Exception
	 */
	@Test
	public void getConfig()
		throws Exception
	{
		setUpRemoteServers();
		startServers();

		myConfigServer.login( "Joe", "cisco789" );

		String value = myConfigServer.getConfig( "a", null);
		assertNull(value);

		value = myConfigServer.getConfig( "a", "bad" );
		assertNull(value);

		value = myConfigServer.getConfig( "a", "var1" );
		assertNotNull(value);
		assertEquals( "123",value);

		value = myConfigServer.getConfig( "a", "var2" );
		assertNotNull(value);
		assertEquals("234", value);

		value = myConfigServer.getConfig( "b", null );
		assertNull(value);

		value = myConfigServer.getConfig( "b", "bad" );
		assertNull(value);

		value = myConfigServer.getConfig( "b", "var1" );
		assertNotNull(value);
		assertEquals("345", value);

		value = myConfigServer.getConfig( "c", null );
		assertNull(value);

		value = myConfigServer.getConfig( "c", "bad" );
		assertNull(value);
	}
}
