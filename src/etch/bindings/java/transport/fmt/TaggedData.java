/* $Id$
 *
 * Created by wert on Apr 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt;

import java.lang.reflect.Array;

import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.ValueFactory;

/**
 * Common interface for various types of tagged data input and output.
 */
abstract public class TaggedData
{
	/**
	 * Constructs the TaggedData.
	 *
	 * @param vf
	 */
	public TaggedData( ValueFactory vf )
	{
		this.vf = vf;
	}
	
	/**
	 * The value factory to use for tagged input and output.
	 */
	protected final ValueFactory vf;

	/**
	 * @return the value factory to use for tagged input and output.
	 */
	public ValueFactory getValueFactory()
	{
		return vf;
	}

	/**
	 * @param value
	 * @return an ArrayValue which models the given array.
	 */
	protected ArrayValue toArrayValue( Object value )
	{
		Class<?> c = value.getClass();
//		System.out.printf( "toArrayValue: class is %s\n", c );
		int dim = 0;
		while (c.isArray())
		{
			dim++;
			c = c.getComponentType();
		}
//		System.out.printf( "toArrayValue: base class is %s dim %d\n", c, dim );
		
		// now we want the type code for c, and if the type code is custom,
		// we'll also want the struct type code.
		
		byte typeCode = getNativeTypeCode( c );
		Type customStructType;
		if (typeCode == TypeCode.CUSTOM)
			customStructType = getCustomStructType( c );
		else
			customStructType = null;

//		System.out.printf( "toArrayValue: type %d customStructType %d dim %d\n", type, customStructType, dim );
		ArrayValue array = new ArrayValue( typeCode, customStructType, dim );
		int n = Array.getLength( value );
		for (int i = 0; i < n; i++)
			array.add( Array.get( value, i ) );
		return array;
	}
	
	protected Object fromArrayValue( ArrayValue av )
	{
		if (av == null)
			return null;
		
		if (av.typeCode() == 0)
			return av;
		
//		System.out.printf( "fromArrayValue %d %s %d\n", av.type(), av.customStructType(), av.dim() );

		Class<?> clss = getComponentType( av.typeCode(), av.customStructType(), av.dim()-1 );
		if (clss == null)
			return av;

		int k = av.size();
		Object v = Array.newInstance( clss, k );
		for (int i = 0; i < k; i++)
		{
			Object o = av.get( i );
//			System.out.printf( "%s[%d] = %s\n", v.getClass(), i, o.getClass() );
			Array.set( v, i, o );
		}
		return v;
	}
	
	private Class<?> getComponentType( byte type, Type customStructType, int dim )
	{
		Class<?> c;
		if (type == TypeCode.CUSTOM)
			c = getCustomType( customStructType );
		else
			c = getNativeType( type );
		
//		System.out.printf( "getComponentType class %s\n", c );
		
		if (c == null)
			return null;
		
		if (dim == 0)
			return c;
		
		int[] dims = new int[dim];
		Object o = Array.newInstance( c, dims );
//		System.out.printf( "object = %s\n", o );
		c = o.getClass();
//		System.out.printf( "class = %s\n", c );
		return c;
	}

	/**
	 * @param value
	 * @return return a type code for the specified value.
	 */
	abstract public byte checkValue( Object value );
	
	/**
	 * Returns the type code for the specified class. This
	 * is needed when we have an array and we have determine
	 * the base type and now we're fixing to serialize it.
	 * We use the base type code to reconstitute it on the
	 * other side. So we don't return a perfect code the way
	 * checkValue does, but rather just something that let's
	 * us recreate the appropriate array type on import.
	 * @param c
	 * @return a type code for the specified class.
	 */
	abstract public byte getNativeTypeCode( Class<?> c );
	
	/**
	 * @param c
	 * @return a struct type for the specified custom class.
	 */
	abstract public Type getCustomStructType( Class<?> c );

	/**
	 * @param type
	 * @return the class for a specified (native) type code.
	 */
	abstract public Class<?> getNativeType( byte type );

	/**
	 * @param type
	 * @return the class for a specified type.
	 */
	abstract public Class<?> getCustomType( Type type );
}
