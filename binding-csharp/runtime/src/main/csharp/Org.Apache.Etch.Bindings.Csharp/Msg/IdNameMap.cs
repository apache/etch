// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.Collections.Generic;

namespace Org.Apache.Etch.Bindings.Csharp.Msg
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
        public T Get( int id ) 
        {
            T t;

            if (byId.TryGetValue(id, out t))
                return t;

            return null;
        }

       public T GetById( int id )
       {
           T t;

           if (byId.TryGetValue(id, out t))
               return t;

           return null;
       }

        /// <summary>Gets the IdName subclass which corresponds to the specified name, or creates it if it isn't 
        /// found and if autoCount maxAutoCount. If created, the IdName is given the id which is 
        /// IdName.hash( name ).</summary>
        /// <param name="name">name the name of an IdName.</param>
        /// <returns>the IdName subclass found or created.</returns>
        public T Get( String name )
        {
            T t;

            if (byName.TryGetValue(name, out t))
                return t;

            if (!locked)
                return Add( MakeNew( name ) );

            return null;
        }

        public T GetByName(String name)
        {
            T t;

            if (byName.TryGetValue(name, out t))
                return t;

            if (!locked)
                return Add(MakeNew(name));

            return null;
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

        /// <summary>
        /// Adds all the IdNames to this IdNameMap
        /// </summary>
        /// <param name="ts"></param>
        public void AddAll( IdNameMap<T> ts )
        {
            if ( locked )
                throw new Exception( "locked" );

            foreach ( T t in ts.byId.Values )
                Add( t );
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

        private Dictionary<int, T> byId = new Dictionary<int, T>();

        private Dictionary<String, T> byName = new Dictionary<String, T>();

        private Boolean locked;
}
}
