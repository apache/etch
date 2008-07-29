/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * Type denotes the type of a struct or message. When used with a
 * message it typically denotes an action or event.
 */
public final class Type extends IdName
{
	/**
	 * Constructs the Type.
	 * @param id the id of the type.
	 * @param name the name of the type.
	 */
	public Type( int id, String name )
	{
		super( id, name );
	}

	/**
	 * Constructs the Type, computing the appropriate value
	 * for the id.
	 * @param name the name of the type.
	 */
	public Type( String name )
	{
		super( name );
	}

	/**
	 * @param key
	 * @return the validator for this key of this type.
	 */
	public Validator getValidator( Field key )
    {
		if (!runValidators)
			return null;
		
	    return validators.get( key );
    }
	
	/**
	 * @return the current setting of runValidators.
	 */
	public static boolean getRunValidators()
	{
		return runValidators;
	}
	
	/**
	 * Enables or disables type validation for messages.
	 * @param value
	 */
	public static void setRunValidators( boolean value )
	{
		runValidators = value;
	}
	
	private static boolean runValidators = true;
	
	/**
	 * Adds the validator to the chain for this key.
	 * @param key
	 * @param vldtr
	 */
	public void putValidator( Field key, Validator vldtr )
	{
		if (vldtr == null)
			return;
		
		synchronized( validators )
		{
			Validator v = validators.get( key );
			if (v != null)
				validators.put( key, new ComboValidator( v, vldtr ) );
			else
				validators.put( key, vldtr );
		}
	}

	/**
	 * Removes the validator chain for this key.
	 * @param key
	 */
	public void clearValidator( Field key )
    {
	    validators.remove( key );
    }
	
	private final Map<Field, Validator> validators =
		Collections.synchronizedMap( new HashMap<Field, Validator>() );
}
