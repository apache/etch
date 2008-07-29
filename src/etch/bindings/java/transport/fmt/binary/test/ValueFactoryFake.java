
package etch.bindings.java.transport.fmt.binary.test;

import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.support.TypeMap;

/**
 * A fake implementation of DefaultValueFactory for testing.
 */
public class ValueFactoryFake extends DefaultValueFactory
{
	private final static TypeMap types = new TypeMap();
	
	private final static Class2TypeMap class2type = new Class2TypeMap();

	static
	{
		DefaultValueFactory.init( types, class2type );
	}

	/**
	 * Constructs the ValueFactoryFake.
	 */
	public ValueFactoryFake()
	{
		super( types, class2type );
	}
}
