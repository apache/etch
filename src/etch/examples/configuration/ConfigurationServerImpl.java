package etch.examples.configuration;

import etch.bindings.java.support._Etch_AuthException;
import etch.bindings.java.util.StrStrHashMap;
import etch.examples.authentication.AuthenticatorServer;

/**
 * @author bchampak
 */
public class ConfigurationServerImpl implements ConfigurationServer
{
	/**
	 * @param auth
	 * @param config
	 * @param testing
	 */
	public ConfigurationServerImpl( AuthenticatorServer auth,
	    ConfigurationServer config, boolean testing )
	{
		this.auth = auth;
		this.config = config;
		this.testing = testing;
	}

	private final ConfigurationServer config;

	private final AuthenticatorServer auth;

	private final boolean testing;

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
		if (!auth.hasRight( name, Configuration.READ_RIGHT ))
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
	
	///////////////////
	// CONFIGURATION //
	///////////////////

	public StrStrHashMap getAllConfig( String path )
	{
		check( "getAllConfig", isLoggedIn() );
		return config.getAllConfig( path );
	}

	public String getConfig( String path, String name ) throws Failure
	{
		check( "getConfig", isLoggedIn() );
		return config.getConfig( path, name );
	}

	/////////////////////
	// TESTING UTILITY //
	/////////////////////

	/**
	 * Tests (if testing) if the op is allowed given the result
	 * of the access check (ok).
	 * @param op the name of the method being checked.
	 * @param authorizationCheckResult the result of the authorization check.
	 */
	private void check( String op, boolean authorizationCheckResult )
	{
		if (testing && !authorizationCheckResult)
			throw new _Etch_AuthException( op );
	}
}
