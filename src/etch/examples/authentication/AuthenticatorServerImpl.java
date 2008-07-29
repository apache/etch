package etch.examples.authentication;

import metreos.util.Log;
import etch.bindings.java.support._Etch_AuthException;

/**
 * @author bchampak
 * 
 */
public class AuthenticatorServerImpl implements AuthenticatorServer
{
	/**
	 * @param auth
	 * @param testing
	 */
	public AuthenticatorServerImpl( AuthDb auth, boolean testing )
	{
		this.auth = auth;
		this.testing = testing;
	}

	private final AuthenticatorServer auth;

	private final boolean testing;

	private String user;

	//////////
	// AUTH //
	//////////

	public void login( String name, String pwd ) throws Failure
	{
		Log.report( "login", "name", name );
		
		if (isLoggedIn())
			throw new Failure( "Already logged in" );

		if (!auth.isAuthentic( name, pwd ))
			throw new Failure( "Not authenticated" );

		if (!auth.hasRight( name, CHECK_RIGHT ))
			throw new Failure( "No rights" );

		user = name;
		
		Log.report( "loginSuccess", "name", name );
	}

	public void logout()
	{
		Log.report( "logout", "name", user );
		
		user = null;
	}

	public Boolean isLoggedIn()
	{
		return user != null;
	}

	public Boolean isAdmin()
	{
		return isLoggedIn() && hasRight( user, ADMIN_RIGHT );
	}

	///////////
	// USERS //
	///////////
	
	public Boolean isAuthentic( String name, String pwd )
	{
		Log.report( "isAuthentic", "name", name );
		check( "isAuthentic", isLoggedIn() );
		return auth.isAuthentic( name, pwd );
	}
	
	public String[] getUsers( String pattern, Integer offset, Integer length )
	{
		check( "getUsers", isAdmin() );
	    return auth.getUsers( pattern, offset, length );
	}

	public Boolean hasUser( String name ) throws Failure
	{
		check( "hasUser", isAdmin() );
	    return auth.hasUser( name );
	}

	public void addUser( String name, String pwd ) throws Failure
	{
		check( "addUser", isAdmin() );
		auth.addUser( name, pwd );
	}

	public void changePassword( String name, String pwd ) throws Failure
    {
		check( "changePassword", isAdmin() );
	    auth.changePassword( name, pwd );
    }

	public void removeUser( String name ) throws Failure
    {
		check( "removeUser", isAdmin() );
	    auth.removeUser( name );
    }

	////////////
	// RIGHTS //
	////////////

	public Boolean hasRight( String name, String right )
	{
		Log.report( "hasRight", "name", name, "right", right );
		check( "hasRight", isLoggedIn() );
		return auth.hasRight( name, right );
	}
	
	public String[] getRights( String name ) throws Failure
	{
		check( "getRights", isAdmin() );
	    return auth.getRights( name );
	}

	public void grantRight( String name, String right ) throws Failure
	{
		check( "grantRight", isAdmin() );
		auth.grantRight( name, right );
	}

	public void denyRight( String name, String right ) throws Failure
    {
		check( "denyRight", isAdmin() );
	    auth.denyRight( name, right );
    }

	/////////////////////
	// TESTING UTILITY //
	/////////////////////

	/**
	 * Tests (if testing) if the op is allowed given the result
	 * of the access check (ok).
	 * @param op the name of the method being checked.
	 * @param ok the result of the access check.
	 */
	private void check( String op, boolean ok )
	{
		if (testing && !ok)
			throw new _Etch_AuthException( op );
	}
}
