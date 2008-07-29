/* $Id$
 *
 * Created by Badri Narayanan Champakesan on Aug 2, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.examples.log.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

import etch.examples.authentication.AuthDb;
import etch.examples.authentication.AuthenticatorServer;
import etch.examples.log.Log;
import etch.examples.log.LogServer;
import etch.examples.log.LogServerImpl;
import etch.examples.log.LogSink;
import etch.examples.log.Log.Failure;

/**
 * @author bchampak
 *
 */
public class TestLogServerImpl implements LogSink
{
	private LogServer setupLogServer() throws etch.examples.authentication.Authenticator.Failure
	{
		AuthenticatorServer auth = new AuthDb();

		auth.addUser("Fred", "cisco123");
		auth.grantRight("Fred", "Log.write");
		
		auth.addUser("Mary", "cisco456");
		auth.grantRight("Mary", "Auth.check");
		
		return new LogServerImpl( auth, true, this );
	}
	
	private int level;
	private String msg;
	
	/**
	 * @throws Exception
	 */
	@Test 
	public void login1() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.login("Fred", "cisco123");
//		assertTrue(auth.hasRight("Fred", "Log.write"));
        // this passes because Fred has rights to write into Log server.
	}
	
	/**
	 * @throws Exception
	 */
	@Test 
	public void login2() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.login("Fred", "cisco123");
		assertTrue(logsi.isLoggedIn());
//		assertFalse(auth.hasRight("Fred", "Horse.Ride"));
        // this fails because something being tried.
	}
	
	/**
	 * @throws Exception
	 */
	@Test (expected = Failure.class)
	public void login3() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.login("Fred", "cisco120");
		assertFalse(logsi.isLoggedIn());
		// this fails because Fred has got his password incorrect
	}
	
	/**
	 * @throws Exception
	 */
	@Test 
	public void login4() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.login("Fred", "cisco123");
		assertTrue(logsi.isLoggedIn());
		logsi.logout();
		// this passes because Fred can log into logserver
	}
	
	/**
	 * @throws Exception
	 */
	@Test( expected = Failure.class )
	public void login5() throws Exception{
		LogServer ls = setupLogServer();
		ls.login("Mary", "cisco456" );
		assertTrue(ls.isLoggedIn());
		ls.logout();
		// this passes because Mary has rights to check the Log server.
	}
	
	/**
	 * @throws Exception
	 */
	@Test ( expected = Failure.class )
	public void login6() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.login("Mary", "cisco450" );
		assertFalse(logsi.isLoggedIn());
		// this fails because Mary has got her password incorrect
	}
	
	/**
	 * @throws Exception
	 */
	@Test ( expected = Failure.class )
	public void login7() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.login("Joe", "cisco789" );
		assertFalse(logsi.isLoggedIn());
		// this fails because Joe has no rights to log into Log server
	}
	
	/**
	 * @throws Exception
	 */
	@Test
	public void getLevel1() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.setLevel( Log.INFO );
		assertEquals(3, logsi.getLevel());
	}
	
	/**
	 * Tests if the log level & msg compare with the ones that are
	 * being actually set 
	 * @throws Exception 
	 */
	@Test
	public void getLevel2() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.log( Log.INFO, "X" );
		assertEquals( this.level, Log.INFO );
		assertEquals( this.msg, "X" );
	}
	
	/**
	 * @throws Exception
	 */
	@Test
	public void getLevel3() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.log( Log.ERROR, "B" );
		assertEquals( this.level, Log.ERROR );
	}
	
	/**
	 * this.level will not be assigned a value if it is found to be less than a 
	 * threshold value say, INFO which is equal to 3
	 * @throws Exception
	 */
	@Test ( expected = Failure.class )
	public void getLevel4() throws Exception{
		LogServer logsi = setupLogServer();
		logsi.log( Log.DEBUG, "B" );
		if(( this.level < Log.INFO )) 
			throw new Failure("Level less than INFO");
	}
		
	public void write( int level, String msg ){
			this.level = level;
			this.msg = msg;
	}
}
