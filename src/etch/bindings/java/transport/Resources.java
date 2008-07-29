package etch.bindings.java.transport;

import java.util.HashMap;

/**
 * A HashMap of string to object.
 */
public class Resources extends HashMap<String, Object>
{
	private static final long serialVersionUID = 1875926663269692773L;

	public Resources()
	{
		// nothing to do.
	}
	
	public Resources( Resources r )
	{
		super( r );
	}
}