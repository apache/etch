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
    /// <summary>
    /// Type denotes the type of a struct or message. When used with a
    /// message it typically denotes an action or event.
    /// </summary>
    public class XType : IdName
    {
        /// <summary>Constructs the Type.</summary>
        /// <param name="id">id of the type.</param>
        /// <param name="name">name of the type.</param>
        public XType( int id, String name )
            : base( id, name )
        {
            // nothing else.
        }

        /// <summary>Constructs the Type, computing the appropriate value for the id.</summary>
        /// <param name="name">name the name of the type.</param>
        public XType( String name )
            : base( name )
        {
            // nothing else.
        }

        public Validator GetValidator( Field key )
        {
            Validator v;
            return validators.TryGetValue(key, out v) ? v : null;
        }

        private readonly FieldMap fieldMap = new FieldMap();

        /// <summary>
        /// Adds the validator to the chain for this key.
        /// </summary>
        /// <param name="key"></param>
        /// <param name="vldtr"></param>
        public void PutValidator( Field key, Validator vldtr )
        {
            CheckNotLocked();

            if ( vldtr == null )
                return;

            if ( fieldMap.Get( key.Id ) == null )
                AddField( key );

            Validator v;
            if (validators.TryGetValue(key, out v))
                validators[key] = new ComboValidator(v, vldtr);
            else
                validators[key] = vldtr;
        }

        /// <summary>
        /// Removes the validator chain for this key
        /// </summary>
        /// <param name="key"></param>
        public void ClearValidator( Field key )
        {
            CheckNotLocked();
            validators.Remove( key );
        }

        public Dictionary<Field, Validator> validators = new Dictionary<Field, Validator>();


        /// <summary>
        /// 
        /// </summary>
        /// <returns>the result type of this message type</returns>
        public XType GetResult()
        {
            return rType;
        }

        public void SetResult( XType rType )
        {
            CheckNotLocked();
            this.rType = rType;
        }

        private XType rType;


        /// <summary>
        /// 
        /// </summary>
        /// <returns>the associated import / export helper</returns>
        public ImportExportHelper GetImportExportHelper()
        {
            return helper;
        }

        /// <summary>
        /// Sets the associated import / export helper.
        /// </summary>
        /// <param name="helper"></param>
        public void SetImportExportHelper( ImportExportHelper helper )
        {
            CheckNotLocked();
            this.helper = helper;
        }

        private ImportExportHelper helper;

       /// <summary>
       /// 
       /// </summary>
        /// <returns>the associated component type for an array of this type</returns>

        public Type GetComponentType()
	    {
            return componentType;
	    }

        /// <summary>
        /// Sets the associated component type for an array of this type.
	    /// This type is not used when de-serializing the array components,
	    /// just when allocating the array itself.
        /// </summary>
        /// <param name="type"></param>

        public void SetComponentType(Type type)
        {
            CheckNotLocked();
            this.componentType = type;
        }

        private Type componentType;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>an object to help dispatch the received message</returns>
        public Object GetStubHelper()
        {
            return stubHelper;
        }

        /// <summary>
        /// Sets an object to help the stub dispatch the received message
        /// </summary>
        /// <param name="stubHelper"></param>
        public void SetStubHelper( Object stubHelper )
        {
            // CheckNotLocked(); called from stub and not from value factory.
            if (this.stubHelper != null)
                throw new Exception("this.stubHelper != null");
            this.stubHelper = stubHelper;
        }

        private Object stubHelper;


        ///<summary>Locks the fields for this type.</summary>
        public void Lock()
        {
            locked = true;
            fieldMap.Lock();
        }

        private void CheckNotLocked()
        {
            if (locked)
                throw new Exception("locked");
        }

        private bool locked;

        
        ///<summary>Adds a field to the set of fields.</summary>
        ///<param name="field"> A field to all </param>
        ///<returns>Returns the argument. If there is a collision with
        /// an id and name, both associated with the same field,
        /// then that field is returned instead of the argument </returns>
        /// <Exception cref="ArgumentException">hrows IllegalArgumentException if there is a
        /// collision in the id or name, or both id and name when not associated 
        /// with the same field.</Exception>
        public Field AddField( Field field )
        {
            return fieldMap.Add( field );
        }

        ///<summary>Translates a field id into the appropriate Field object.</summary>
        ///<param name="id">field id</param>
        ///<returns>id translated into the appropriate Field.</returns>
        public Field GetField( int id )
        {
            return fieldMap.Get( id );
        }

       
        ///<summary>Translates a field name into the appropriate Field.</summary>
        ///<param name="name">A field name</param>
        ///<returns>name translated into the appropriate Field.</returns>
        public Field GetField( string name )
        {
            return fieldMap.Get( name );
        }

        ///<returns>a set of all the fields.</returns>
        public List<Field> GetFields()
        {
            return fieldMap.Values();
        }

        /// <summary>
        /// Every type ( => message) carries a timeout
        /// </summary>
        private int timeout;

        public int Timeout
        {
            get
            {
                return timeout;
            }
            set
            {
                CheckNotLocked();
                timeout = value;
            }
        }

        /// <summary>
        /// Field containing result value
        /// </summary>
        private Field responseField;

        public Field ResponseField
        {
            get
            {
                return responseField;
            }
            set
            {
                CheckNotLocked();
                responseField = value;
            }
        }

        ///<summary>Checks whether this type is assignment compatible with other. This
        ///means that other is a subclass of this.</summary>
        ///<param name="other"></param>
        ///<returns>true if this type is assignable from other</returns>
        
        public bool IsAssignableFrom( XType other )
        {
            return other != null && ( this.Equals( other ) || IsAssignableFrom( other.SuperType() ) );
        }

        public void CheckIsAssignableFrom( XType other )
        {
            if ( !IsAssignableFrom( other ) )
                throw new ArgumentOutOfRangeException();
        }

        public XType SuperType()
        {
            return superType;
        }

        ///<summary>Sets the super type of this type. If struct A extends B, then
        ///B is the super type of A.</summary>
        ///<param name="superType"></param>
        public void SetSuperType( XType superType )
        {
            CheckNotLocked();
            this.superType = superType;
        }

        private XType superType;

       /// <summary>
       /// Gets AsyncMode for this Type
       /// </summary>
       /// <returns>AsyncMode</returns>
        public AsyncMode GetAsyncMode()
        {
            return asyncMode;
        }

        /// <summary>
        /// Sets the AysncMode
        /// </summary>
        /// <param name="mode"></param>
        public void SetAsyncMode(AsyncMode mode)
        {
            CheckNotLocked();
            asyncMode = mode;
        }

        private AsyncMode asyncMode;

        /// <summary>
        /// Gets the message direction.
        /// </summary>
        /// <returns>the message direction</returns>
        public Direction GetDirection()
        {
            return direction;
        }

        /// <summary>
        /// Sets the message direction.
        /// </summary>
        /// <param name="direction"></param>
        public void SetDirection(Direction direction)
        {
            CheckNotLocked();
            this.direction = direction;
        }

        private Direction direction;
    }
}
