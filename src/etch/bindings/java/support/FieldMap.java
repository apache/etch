package etch.bindings.java.support;

import etch.bindings.java.msg.Field;

/**
 * FieldMap is a IdNameMap for Field.
 */
public final class FieldMap extends IdNameMap<Field>
{
	@Override
	public Field makeNew( String name )
	{
		return new Field( name );
	}
}
