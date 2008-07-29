/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;

/**
 * An array of values, where each value is of arbitrary type
 * chosen from the basic java types boolean, byte, short, int,
 * long, float, double, String, an array of those, the extended
 * types ArrayValue and StructValue, and specific types supported
 * by ValueFactory.
 * 
 * ArrayValue is not protected against concurrent access.
 */
public final class ArrayValue extends ArrayList<Object>
	implements Iterable<Object>
{
	/**
	 * Bogus serial version id.
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Constructs the ArrayValue.
	 * @param dim 
	 * @param customStructType 
	 * @param typeCode 
	 */
	public ArrayValue( byte typeCode, Type customStructType, int dim )
	{
		this.typeCode = typeCode;
		this.customStructType = customStructType;
		this.dim = dim;
	}
	
	/**
	 * Constructs an array value with no type info. This is used perhaps
	 * by extern struct serializers.
	 */
	public ArrayValue()
	{
		this.typeCode = 0;
		this.customStructType = null;
		this.dim = 0;
	}
	
	private final byte typeCode;
	
	private final Type customStructType;
	
	private final int dim;
	
	/**
	 * @return the TypeCode for this array value.
	 * For example, if the array is int[][], then
	 * the type would be TypeCode.INT4.
	 */
	public byte typeCode()
	{
		return typeCode;
	}
	
	/**
	 * @return a struct type if a custom type code.
	 */
	public Type customStructType()
	{
		return customStructType;
	}
	
	/**
	 * @return the dimsionality of the array.
	 * For example, if the array is int[][], the
	 * dimensionality would be 2.
	 */
	public int dim()
	{
		return dim;
	}
	
	/**
	 * Adds many values to the array value.
	 * @param values
	 * @return this array value.
	 */
	public ArrayValue addMany( Object... values )
	{
		for (Object v: values)
			add( v );
		return this;
	}
	
//	/**
//	 * Reads an array and all of its values.
//	 * @param tdi the tagged data input to read from.
//	 * @return an array value read from the tagged data input.
//	 * @throws IOException if there is a problem reading from the tagged data input.
//	 */
//	public static ArrayValue readx( TaggedDataInput tdi )
//		throws IOException
//	{
//		ArrayValue array = tdi.startArray();
//		array.readValues( tdi );
//		tdi.endArray( array );
//		return array;
//	}

	/**
	 * Writes an array and all of its values.
	 * @param tdo the tagged data output to write to.
	 * @throws IOException if there is a problem writing to the tagged data output.
	 */
	public void write( TaggedDataOutput tdo ) throws IOException
	{
		tdo.startArray( this );
		writeValues( tdo );
		tdo.endArray( this );
	}
	
//	/**
//	 * Reads the values of the array. Does not clear the array first.
//	 * @param tdi the tagged data input to read from.
//	 * @throws IOException if there is a problem reading from the tagged data input.
//	 */
//	private void readValues( TaggedDataInput tdi )
//		throws IOException
//	{
//		ArrayElement ae = new ArrayElement();
//		while (tdi.readArrayElement( ae ))
//			add( ae.value );
//	}
	
	/**
	 * Writes the values of the array.
	 * @param tdo the tagged data output to write to.
	 * @throws IOException if there is a problem writing to the tagged data output.
	 */
	private void writeValues( TaggedDataOutput tdo )
		throws IOException
	{
		for (Object value: this)
			tdo.writeArrayElement( value );
	}
	
	/**
	 * Compares two array values for equality with some help.
	 * @param obj an object to compare to this array value.
	 * @param helper implementation to help with comparing
	 * comformant but not identical objects (e.g., small
	 * integer represented as Byte vs. Integer).
	 * @return true if the object is an array value and
	 * compares equal with this one (relative to helper).
	 */
	public boolean equals( Object obj, EqualsHelper helper )
	{
		if (obj == this)
			return true;
		
		if (obj == null)
			return false;
		
		if (getClass() != obj.getClass())
			return false;
		
		ArrayValue other = (ArrayValue) obj;
		
		if (size() != other.size())
			return false;

		Iterator<Object> vals = iterator();
		Iterator<Object> otherVals = other.iterator();
		while (vals.hasNext())
		{
			Object a = vals.next();
			Object b = otherVals.next();
			if (helper != null)
			{
				if (!helper.equals( a, b ))
					return false;
			}
			else if (!objEquals( a, b ))
			{
				return false;
			}
		}
		
		return true;
	}

	/**
	 * Compares two objects for equality taking into account the fact
	 * that one or both might be null.
	 * @param a an object to compare.
	 * @param b an object to compare.
	 * @return true if both are null or if both are not null and a.equals( b ).
	 */
	private static boolean objEquals( Object a, Object b )
	{
		if (a == null && b == null)
			return true;
		
		if (a == null || b == null)
			return false;
		
		return a.equals( b );
	}
}