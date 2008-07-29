package etch.examples.distmap;

import java.util.Map;

/**
 * @author jan
 * 
 */
public class ImplDistributedHashTableServer implements DistributedHashTableServer
{

	/**
	 * @param client a DistributedHashTableClient instance for server implementation
	 * to use to call back to the client.
	 * 
	 * @param map hash table
	 */
	public ImplDistributedHashTableServer( Map<String, Object> map )
	{
		this.map = map;
	}
		
	private final Map<String, Object> map;

	/**
	 * Gets all entries in the hash table.
	 * 
	 * @return an array of entries
	 */
	public Entry[] getAll()
	{
		synchronized (map)
		{
			Entry[] list = new Entry[map.size()];
			int index = 0;
			for (Map.Entry<String, Object> me : map.entrySet())
				list[index++] = new Entry( me.getKey(), me.getValue() );
			return list;
		}
	}

	/**
	 * Gets the value to which key is mapped
	 * 
	 * @param key key associated with value
	 * @return the value to which the key is mapped
	 */
	public Object getObject( String key )
	{
		return map.get( key );
	}

	/**
	 * Adds a key-value mapping to the map
	 * 
	 * @param key the key
	 * @param value the value
	 */
	public Object putObject( String key, Object value )
	{
		return map.put( key, value );
	}

	/**
	 * Removes a key-value mapping from the map
	 * 
	 * @param key the key
	 * @return the value associated with key
	 */
	public Object removeObject( String key )
	{
		return map.remove( key );
	}

	/**
	 * Get the number of key-value mappings in this map
	 * 
	 * @return the number of key-value mappings in this map
	 */
	public Integer size()
	{
		return map.size();
	}
}
