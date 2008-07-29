using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace etch.examples.distmap
{
    public class DistributedHashTableServerImpl : DistributedHashTableServer
    {
        Dictionary<String, Object> _map;

        /// <summary>
        /// Constructs the server with the shared map.
        /// </summary>
        /// <param name="map"></param>
        public DistributedHashTableServerImpl( Dictionary<String, Object> map )
        {
            _map = map;
        }

        /// <summary>
        /// Gets all the entries in the hash table
        /// </summary>
        /// <returns>an array of entries</returns>
        public etch.examples.distmap.ConstsDistributedHashTable.Entry[] getAll()
        {
            lock ( _map )
            {
                etch.examples.distmap.ConstsDistributedHashTable.Entry[] list = 
                    new etch.examples.distmap.ConstsDistributedHashTable.Entry[ _map.Count ];

                int index = 0;

                foreach ( String s in _map.Keys )
                {
                    list[ index++ ] = new etch.examples.distmap.ConstsDistributedHashTable.Entry( s, _map[ s ] );
                }
                return list;
            }
        }

        /// <summary>
        /// Gets the value to which key is mapped
        /// </summary>
        /// <param name="key">key associated with value</param>
        /// <returns>the value to which the key is mapped</returns>
        public Object getObject( String key )
        {
            return _map[ key ];
        }

        /// <summary>
        /// Adds a key-value mapping to the map
        /// </summary>
        /// <param name="key">the key</param>
        /// <param name="value">the value</param>
        /// <returns>the value of the key, or null if this is a new entry</returns>
        public Object putObject( String key, Object value )
        {
            if ( _map.ContainsKey( key ) )
                return _map[ key ];
            else
            {
                _map.Add( key, value );
                return null;
            }
        }

        /// <summary>
        /// Removes a key-value mapping from the map
        /// </summary>
        /// <param name="key">the key</param>
        /// <returns>value associated with the key</returns>
        public Object removeObject( String key )
        {
            // c# map returns bool indicating whether 
            // the operation succeeded or not. 
            Object value = _map[ key ];
            if ( _map.Remove( key ) )
                return value;
            else
                return null;

        }

        /// <summary>
        /// Get the number of key-value mappings in this map
        /// </summary>
        /// <returns>the number of key-value mappings in this map</returns>
        public int? size()
        {
            return _map.Count;
        }
    }
}
