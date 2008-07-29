/*
 * $Id$
 * 
 * Created by Badri Narayanan Champakesan on Aug 2, 2007.
 * 
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.examples.log;

import etch.examples.authentication.AuthenticatorServer;

/**
 * @author bchampak
 */
public class LogServerImpl implements LogServer
{
	/**
	 * @param auth
	 * @param testing
	 * @param lSink
	 */
	public LogServerImpl( AuthenticatorServer auth, boolean testing,
		LogSink lSink )
	{
		this.auth = auth;
		this.testing = testing;
		this.lSink = lSink;
	}

	@SuppressWarnings("unused")
	private boolean testing;

	private final AuthenticatorServer auth;

	private final LogSink lSink;

	private int curLevel;

	private String user;

	//////////
	// AUTH //
	//////////

	public void login( String name, String pwd ) throws Failure
	{
		if (isLoggedIn())
			throw new Failure( "Already logged in" );
		if (!auth.isAuthentic( name, pwd ))
			throw new Failure( "Not authenticated" );
		if (!auth.hasRight( name, Log.WRITE_RIGHT ))
			throw new Failure( "No rights" );

		user = name;
	}

	public void logout()
	{
		user = null;
	}

	public Boolean isLoggedIn()
	{
		return user != null;
	}

	/////////
	// LOG //
	/////////

	public Integer getLevel()
	{
		return curLevel;
	}

	public void setLevel( Integer level ) throws Failure
	{
		if (level < DEBUG && level > ERROR)
			throw new Failure( "Undefined Level" );
		this.curLevel = level;
	}

	public void log( Integer level, String msg )
	{
		if (level >= this.curLevel)
			lSink.write( level, msg );
	}
}