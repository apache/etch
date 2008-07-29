package etch.bindings.java.transport;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import metreos.util.URL;
import etch.bindings.java.msg.TaggedDataInput;
import etch.bindings.java.msg.TaggedDataOutput;
import etch.bindings.java.msg.ValueFactory;

/**
 * A factory used to construct tagged data processors for messagizer.
 */
abstract public class FormatFactory
{
	/**
	 * @param vf
	 * @param uri
	 * @return the tagged data input with the specified value factory and
	 * initialized by terms from the uri.
	 */
	abstract public TaggedDataInput newTaggedDataInput( ValueFactory vf, URL uri );

	/**
	 * @param vf
	 * @param uri
	 * @return the tagged data output with the specified value factory and
	 * initialized by terms from the uri.
	 */
	abstract public TaggedDataOutput newTaggedDataOutput( ValueFactory vf, URL uri );
	
	private final static Map<String, FormatFactory> formatFactories =
		Collections.synchronizedMap( new HashMap<String, FormatFactory>() );
	
	/**
	 * Gets the named format factory.
	 * @param name the name of a configured format factory.
	 * @return the named format factory, or null if it isn't defined.
	 */
	public static FormatFactory get( String name )
	{
		return formatFactories.get( name );
	}
	
	/**
	 * Puts a named format factory.
	 * @param name the uri scheme of this format factory.
	 * @param ff the format factory to associate with the name.
	 */
	public static void put( String name, FormatFactory ff )
	{
		formatFactories.put( name, ff );
	}
	
	static
	{
		put( "binary", new BinaryTaggedDataFormatFactory() );
	}
}
