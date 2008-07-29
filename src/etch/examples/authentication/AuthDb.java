package etch.examples.authentication;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

/**
 * A fake AuthenticatorServer impl used for local authentication db.
 */
public class AuthDb implements AuthenticatorServer
{
	///////////
	// USERS //
	///////////
	
	public Boolean isAuthentic( String name, String pwd )
	{
		if (name == null)
			return false;
		
		if (pwd == null)
			return false;
		
		String userPwd = users.get( name );
		if (userPwd == null)
			return false;
		
		return userPwd.equals( pwd );
	}
	
	public String[] getUsers( String pattern, Integer offset, Integer length )
	{
		// TODO this needs to not be java specific.
		Pattern p = Pattern.compile( pattern );
		List<String> list = new ArrayList<String>();
		Iterator<String> i = users.keySet().iterator();
		int j = 0;
		while (i.hasNext() && list.size() < length)
		{
			String name = i.next();
			if (p.matcher( name ).matches())
			{
				if (j >= offset)
					list.add( name );
				j++;
			}
		}
		return list.toArray( new String[list.size()] );
	}
	
	public Boolean hasUser( String name ) throws Failure
	{
		checkNotNull( "name", name );
		return userExists( name );
	}

	public void addUser( String name, String pwd ) throws Failure
	{
		checkNotNull( "name", name );
		checkNotNull( "pwd", pwd );
		checkValidName( name );
		
		if (userExists( name ))
			throw new Failure( "user already exists: " + name );
		
		users.put( name, pwd );
		removeRights( name );
	}

	public void changePassword( String name, String pwd ) throws Failure
	{
		checkNotNull( "name", name );
		checkNotNull( "pwd", pwd );
		checkUserExists( name );
		
		users.put( name, pwd );
	}

	public void removeUser( String name ) throws Failure
	{
		checkNotNull( "name", name );
		checkUserExists( name );
		
		users.remove( name );
		removeRights( name );
	}

	private boolean userExists( String name )
	{
		return users.containsKey( name );
	}
	
	private void checkUserExists( String name ) throws Failure
	{
		if (!userExists( name ))
			throw new Failure( "user does not exist: " + name );
	}

	private void checkValidName( String name ) throws Failure
    {
		if (name.length() == 0)
			throw new Failure( "cannot have a blank name" );
		
	    if (name.indexOf( RIGHT_SEP ) >= 0)
	    	throw new Failure( "invalid character in name: " + name );
    }

	private final Map<String, String> users = new HashMap<String, String>();

	////////////
	// RIGHTS //
	////////////

	public Boolean hasRight( String name, String right )
	{
		return name != null && right != null && userExists( name )
		    && rights.contains( getRightsKey( name, right ) );
	}
	
	public String[] getRights( String name ) throws Failure
	{
		List<String> list = new ArrayList<String>();
	    String prefix = getRightsKey( name, "" );
	    Iterator<String> i = rights.iterator();
	    while (i.hasNext())
	    {
	    	String right = i.next();
	    	if (right.startsWith( prefix ))
	    		list.add( right.substring( prefix.length() ) );
	    }
	    return list.toArray( new String[list.size()] );
	}

	public void grantRight( String name, String right ) throws Failure
	{
		checkNotNull( "name", name );
		checkNotNull( "right", right );
		checkUserExists( name );
		checkValidRight( right );
		
		rights.add( getRightsKey( name, right ) );
	}
	
	public void denyRight( String name, String right ) throws Failure
	{
		checkNotNull( "name", name );
		checkNotNull( "right", right );
		checkUserExists( name );
		
	    rights.remove( getRightsKey( name, right ) );
	}
	
	private void removeRights( String name )
    {
	    String prefix = getRightsKey( name, "" );
	    Iterator<String> i = rights.iterator();
	    while (i.hasNext())
	    {
	    	String right = i.next();
	    	if (right.startsWith( prefix ))
	    		i.remove();
	    }
    }

	private void checkValidRight( String right ) throws Failure
    {
		if (right.length() == 0)
			throw new Failure( "cannot have a blank right" );
		
	    if (right.indexOf( RIGHT_SEP ) >= 0)
	    	throw new Failure( "invalid character in right: " + right );
    }

	private String getRightsKey( String name, String right )
	{
		return name + RIGHT_SEP + right;
	}

	private final Set<String> rights = new HashSet<String>();
	
	private final static char RIGHT_SEP = '@';

	//////////
	// AUTH //
	//////////

	public void login( String name, String pwd ) throws Failure
	{
		throw new UnsupportedOperationException( "AuthDb does not support login" );
	}

	public void logout()
	{
		throw new UnsupportedOperationException( "AuthDb does not support logout" );
	}

	public Boolean isLoggedIn()
	{
		throw new UnsupportedOperationException( "AuthDb does not support isLoggedIn" );
	}

	public Boolean isAdmin()
	{
		throw new UnsupportedOperationException( "AuthDb does not support isAdmin" );
	}

	/////////////
	// UTILITY //
	/////////////
	
	private void checkNotNull( String name, Object obj ) throws Failure
	{
		if (obj == null)
			throw new Failure( "value should not be null: " + name );
	}
}
