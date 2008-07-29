///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 25, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Etch.Msg;

namespace Etch.Support
{

    /// <summary>Map by id and name of IdNames (or subclasses thereof).</summary>
    /// <typeparam name="T">the specific subclass of IdName that is in the map.</typeparam>
    public abstract class IdNameMap<T> where T : IdName
    {

        public IdNameMap()
        {
            // nothing to do
        }

        /// <summary>Constructs the IdNameMap.</summary>
        /// <param name="maxAutoCount">maxAutoCount the maximum number of automatically declared IdNames 
        /// allowed. Set to 0 if you don't want any, but this may prevent working with a slightly different
        /// version of a service.</param>
        public IdNameMap( int maxAutoCount )
        {
            this.maxAutoCount = maxAutoCount;
        }
    	
        private int maxAutoCount;
    	
        /// <summary>Gets the IdName subclass which corresponds to the specified id, or creates it if it isn't found 
        /// and if autoCount less than maxAutoCount. If created, the IdName is given the name which is 
        /// id.toString().</summary>
        /// <param name="id">id the id of an IdName.</param>
        /// <returns>the IdName subclass found or created.</returns>
        public T Get( int? id ) 
        {
            try
            {
                return byId[ id ];
            }
            catch ( KeyNotFoundException )
            {
                return null;
            }
        }

       public T GetById( int? id )
       {
           try
           {
               return byId[id];
           }catch{ return null; }
       }

        /// <summary>Gets the IdName subclass which corresponds to the specified name, or creates it if it isn't 
        /// found and if autoCount maxAutoCount. If created, the IdName is given the id which is 
        /// IdName.hash( name ).</summary>
        /// <param name="name">name the name of an IdName.</param>
        /// <returns>the IdName subclass found or created.</returns>
        public T Get( String name ) 
        {
            T t;
            try
            {
                t = byName[ name ];
            }
            catch ( KeyNotFoundException )
            {
                t = null;
            }

            if ( t == null && !locked )
                t = Add( MakeNew( name ) );
            return t;
        }

        public T GetByName(String name)
        {
            try
            {
                return byName[name];
            }
            catch { return null; }
        }
    	
        /// <summary>
        /// Adds the IdName subclass to the map. 
        /// </summary>
        /// <param name="t">the IdName subclass to add.</param>
        /// <returns>the IdName from the map</returns>
        /// Exception:
        ///             throws Exception (state) if there is a collision with id or name, or a collision with 
        ///             id and name where they are not associated with the same object.
        public T Add( T t ) 
        {
            if ( locked )
                throw new Exception( "state exception -- locked" );

            if ( byId.ContainsKey( t.Id ) )
            {
                if ( ( t.Name == "msg" ) || ( t.Name == "result" ) )
                    return t;

                throw new ArgumentException( "id collision" );
            }

            if ( byName.ContainsKey( t.Name ) )
            {
                if ( ( t.Name == "msg" ) || ( t.Name == "result" ) )
                    return t;
            
                throw new ArgumentException( "name collision" );
            }
            byId.Add ( t.Id, t);  
            byName.Add(t.Name, t);
            return t;
        }

        /// <summary></summary>
        /// <returns>a snapshot collection of all the values.</returns>
        public List<T> Values() 
        {
            return new List<T>( byId.Values );
        }

        /// <summary>
        /// Locks the map, preventing further changes
        /// </summary>
        public void Lock()
        {
            locked = true;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the number of values in the map</returns>
        public int Size()
        {
            return byId.Count;
        }

        /// <summary>Makes a new subclass of IdName to put in this map.</summary>
        /// <param name="name">name the name of the new subclass of IdName.</param>
        /// <returns>a newly constructed subclass of IdName to put in this map.</returns>
        abstract public T MakeNew( String name );

        private Dictionary<int?, T> byId = new Dictionary<int?, T>();

        private Dictionary<String, T> byName = new Dictionary<String, T>();

        private Boolean locked;
}
}
