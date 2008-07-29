/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

/**
 * An IdName is a base class for Field or Type. It is used
 * to bind together a type or field name with the associated id. The id
 * is used for certain operations, such as the key in a Map, comparisons,
 * and binary encoding on the wire, while the name is used for display.
 * 
 * @see Field
 * @see Type
 */
public class IdName
{
	/**
	 * Constructs the IdName.
	 * @param id the id for the name (normally computed using
	 * {@link #hash(String)})
	 * @param name the name of the item.
	 */
	public IdName( Integer id, String name )
	{
		this.id = id;
		this.name = name;
	}
	
	/**
	 * Constructs the IdName by computing the appropriate id given
	 * the name.
	 * @param name the name of the item.
	 * @see #hash(String)
	 */
	public IdName( String name )
	{
		this( hash( name ), name );
	}
	
	private final Integer id;
	
	private final String name;
	
	/**
	 * @return the id of the item.
	 */
	public final Integer getId()
	{
		return id;
	}
	
	/**
	 * @return the name of the item.
	 */
	public final String getName()
	{
		return name;
	}
	
	@Override
	public final String toString()
	{
		return name;
	}

	@Override
	public final boolean equals( Object obj )
	{
		if (obj == this)
			return true;
		
		if (obj == null)
			return false;
		
		if (obj.getClass() != getClass())
			return false;
		
		IdName other = (IdName) obj;
		
		return id.equals( other.id );
	}

	@Override
	public final int hashCode()
	{
		return id.hashCode();
	}

    /**
     * Computes the hash value of the name to be used as the id when
     * constructing an IdName.
     * @param name the name of the type or field.
     * @return a hash of name in the unicode character encoding which is
     * very likely to be unique over reasonable name spaces. Collisions
     * should be very unlikely as they will force the user to change the
     * name.
     */
    public static int hash( String name )
    {
    	// combination of djb2 and sdbm hash algorithm:
        int hash = 5381;
        int n = name.length();
        for (int i = 0; i < n; i++)
        {
        	char c = name.charAt( i );
        	int h6 = hash << 6;
        	hash = (h6 << 10) + h6 - hash + c;
        }
        return hash;
    }
    
//    /**
//     * Prints the hash of one or more names.
//     * @param args one or more names whose hash is to be printed.
//     */
//    public static void main( String[] args )
//    {
//    	if (args.length == 0)
//    	{
//    		System.out.println( "Prints the hash of one or more names" );
//    		System.out.println( "Usage: java etch.bindings.java.msg.IdName name ..." );
//    		System.exit( 1 );
//    	}
//    	
//    	for (String arg: args)
//    		System.out.printf( "%s: %d\n", arg, hash( arg ) );
//    }
}
