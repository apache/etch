package etch.bindings.java.support;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import etch.bindings.java.msg.Validator;

/**
 * Validator for custom type.
 */
public class Validator_custom extends TypeValidator
{
	/**
	 * @param clss the class of the custom type.
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @param subclassOk if true allow subclasses of the
	 * class.
	 * @return an instance of Validator.
	 */
	public static Validator get( Class<?> clss, int nDims, boolean subclassOk )
	{
		checkDims( nDims );
		
		if (nDims > MAX_CACHED)
			return new Validator_custom( clss, nDims, subclassOk );
		
		Key key = new Key( clss, nDims, subclassOk );
		Validator v = validators.get( key );
		
		if (v == null)
		{
			v = new Validator_custom( clss, nDims, subclassOk );
			validators.put( key, v );
		}
		
		return v;
	}
	
	private static class Key
	{
		public Key( Class<?> clss, int dims, boolean subclassOk )
		{
			this.clss = clss;
			this.dims = dims;
			this.subclassOk = subclassOk;
		}
		
		private final Class<?> clss;
		
		private final int dims;
		
		private final boolean subclassOk;
		
		@Override
		public int hashCode()
		{
			return clss.hashCode() ^ (dims * 9131) ^ (subclassOk ? 21357 : 8547);
		}
		
		@Override
		public boolean equals( Object obj )
		{
			if (obj == this)
				return true;
			
			if (obj == null)
				return false;
			
			if (obj.getClass() != getClass())
				return false;
			
			Key other = (Key) obj;
			
			return other.clss.equals( clss)
				&& other.dims == dims
				&& other.subclassOk == subclassOk;
		}
	}
	
	private final static Map<Key, Validator> validators =
		Collections.synchronizedMap( new HashMap<Key, Validator>() );
	
	/**
	 * Constructs the Validator.
	 *
	 * @param clss the class of the custom type.
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_custom( Class<?> clss, int nDims, boolean subclassOk )
	{
		super( clss, clss, nDims,
			subclassOk
				? clss.toString()+"["+nDims+"]*"
				: clss.toString()+"["+nDims+"]",
			subclassOk );
		this.clss = clss;
	}
	
	private final Class<?> clss;

	@Override
	public Validator elementValidator()
	{
		return get( clss, nDims-1, subclassOk );
	}
}
