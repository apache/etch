package etch.bindings.java.support;

import etch.bindings.java.msg.Type;

/**
 * TypeMap is a IdNameMap for Type.
 */
public final class TypeMap extends IdNameMap<Type>
{
	@Override
	public Type makeNew( String name )
	{
		return new Type( name );
	}
}
