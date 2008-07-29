package etch.examples.locator;

import java.util.ArrayList;
import java.util.List;

import metreos.util.Log;
import metreos.util.URL;

/**
 * @author bchampak
 * 
 */
public class LocatorServerImpl implements LocatorServer
{
	/**
	 * @param registry
	 */
	public LocatorServerImpl( List<Registration> registry )
	{
		this.registry = registry;
	}

	private List<Registration> registry;

	public Registration[] find( URL query, Integer offset, Integer length )
	{
		Log.report( "find", "query", query, "offset", offset, "length", length );
		
		List<Registration> result = new ArrayList<Registration>();

		for (Registration c1 : registry)
			if (match( query, c1.description ))
				result.add( c1 );

		if (offset > 0 || result.size() > length)
			result = result.subList( offset, offset+length );
		
		Log.report( "findResult", "query", query, "offset", offset,
			"length", length, "result", result );
		
		return result.toArray( new Registration[result.size()] );
	}

	/**
	 * @param query a query URL to match against the description URL.
	 * @param description the description URL of a service instance.
	 * @return true if the query matches the description.
	 */
	private boolean match( URL query, URL description )
	{
		// TODO implement this.
		return true;
	}

	public Boolean canRead()
	{
		// TODO implement this.
		return true;
	}

	public Boolean canWrite( URL description )
	{
		// TODO implement this.
		return true;
	}

	public Boolean isLoggedIn()
	{
		// TODO implement this.
		return user != null;
	}
	
	private String user;

	public void login( String name, String pwd ) throws Failure
	{
		if (user != null)
			throw new Failure( "already logged in" );
		
		// TODO implement this.
		
		user = name;
	}

	public void logout()
	{
		if (user != null)
		{
			// TODO implement this.
			unsubscribeAll();
			user = null;
		}
	}

	public void register( URL description, URL contact, Integer lifetime )
	{
		// TODO implement this.
	}

	public void subscribe( URL query )
	{
		// TODO implement this.
	}

	public void unregister( URL description )
	{
		// TODO implement this.
	}

	public void unsubscribe( URL query )
	{
		// TODO implement this.
	}

	public void unsubscribeAll()
	{
		// TODO implement this.
	}
}