package etch.bindings.java.transport;

import etch.bindings.java.support.FreePool;
import etch.bindings.java.support.QueuedPool;

/**
 * Class to help construct transport stacks.
 */
public class TransportHelper
{
	///////////////
	// RESOURCES //
	///////////////

	/** The Pool to use to execute queued async receiver messages. */
	public static final String QUEUED_POOL = "QUEUED_POOL";
	
	/** The Pool to use to execute free async receiver messages. */
	public static final String FREE_POOL = "FREE_POOL";
	
	/** Binary transport format */
	public final static String BINARY = "binary";
	
	/** Xml transport format */
	public final static String XML = "xml";

	///////////////
	// UTILITIES //
	///////////////
	
	/**
	 * Initializes standard resources.
	 * @param resources
	 * @return resources initialized with default values for standard items.
	 * @throws Exception
	 */
	public static Resources initResources( Resources resources ) throws Exception
	{
		if (resources == null)
			resources = new Resources();
		
		if (!resources.containsKey( QUEUED_POOL ))
			resources.put( QUEUED_POOL, new QueuedPool() );
		
		if (!resources.containsKey( FREE_POOL ))
			resources.put( FREE_POOL, new FreePool() );
		
		if (!resources.containsKey( Messagizer.FORMAT ))
			resources.put( Messagizer.FORMAT, BINARY );
		
		return resources;
	}
}

