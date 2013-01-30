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
using System.Text;

namespace Org.Apache.Etch.Bindings.Csharp.Msg
{

    ///A typed map of key/value pairs, where the type is a Type, each key is a Field, 
    ///and each value is of arbitrary type chosen from the basic java types boolean, byte, 
    ///short, int, long, float, double, String, an array of those, the extended types ArrayValue 
    ///and StructValue, and specific types supported by ValueFactory. 
    ///StructValue is not protected against concurrent access.
    public class StructValue 
    {
        ///<summary>Constructs the StructValue with default length.</summary>
        ///<param name="type">the type of the struct (or the action or event if
        ///this is a message).</param>
        ///<param name="vf">the value factory for this struct</param>
        public StructValue(XType type, ValueFactory vf)
            : this(type, vf, 0)
        {
            // nothing to do.
        }

        ///<summary>Constructs the StructValue.</summary>
        ///<param name="type">the type of the struct (or the action or event if
        ///this is a message).</param>
        ///<param name="length">the expected number of name/value pairs.</param>
        public StructValue(XType type, ValueFactory vf, int length)
        {
            if (type == null)
                throw new ArgumentNullException("type == null");

            if (vf == null)
                throw new ArgumentNullException("vf == null");

            this.type = type;
            this.fields = new Dictionary<Field, Object>(length > 0 ? (length * 4 + 2) / 3 : DEFAULT_SIZE);
            this.level = vf.GetLevel();
        }

        private const int DEFAULT_SIZE = 8;

        private readonly XType type;

        private readonly Dictionary<Field, Object> fields;

        private readonly Validator.Level level;

        /// <summary>
        /// The struct type, or message action or event.
        /// </summary>
        public XType GetXType
        {
            get
            {
                return type;
            }
        }

        /// <summary>
        /// The validation level.
        /// </summary>
        public Validator.Level Level
        {
            get
            {
                return level;
            }
        }

        /// <summary>Compares the type of this struct to another type.</summary>
        /// <param name="otherType">otherType the type to compare this type to.</param>
        /// <returns>true if this struct is of the specified type.</returns>
        public bool IsType(XType otherType)
        {
            return type.Equals(otherType);
        }

        /// <summary>Checks a struct for having the expected type.</summary>
        /// <param name="expectedType">expectedType the expected type of this struct.</param>
        /// Exception:
        ///         throws IllegalArgumentException if the type is not as expected.
        public void CheckType(XType expectedType)
        {
            if(!IsType(expectedType))
                throw new ArgumentException();
        }

        public Dictionary<Field, Object>.Enumerator GetEnumerator()
        {
            return fields.GetEnumerator();
        }

        public Dictionary<Field, Object>.KeyCollection Keys
        {
            get
            {
                return fields.Keys;
            }
        }

        public bool ContainsKey(Field key)
        {
            return fields.ContainsKey(key);
        }

        /// <summary>Defined to get the Object return type</summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public Object Get(Field key)
        {
            object value;
            return fields.TryGetValue(key, out value) ? value : null;
        }
      
        /// <summary>
        /// Overriding Dictionary's Add(key, value) function
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        public void Add(Field key, Object value)
        {
            if (value == null)
            {
                fields.Remove(key);
                return;
            }

            if (level != Validator.Level.NONE)
            {
                Validator v = type.GetValidator(key);

                if (v == null && level == Validator.Level.FULL)
                    throw new ArgumentException(String.Format(
                        "validator missing for type {0} field {1}", type, key));

                if (v != null && !v.Validate(value))
                    throw new ArgumentException(String.Format(
                        "validator {0} failed for type {1} field {2} : value {3}",
                        v, type, key, value));
            }

            fields[key] = value;
        }

        public Object this[Field key]
        {
            get
            {
                return Get(key);
            }
            set
            {
                Add(key, value);
            }
        }

        public void Remove(Field key)
        {
            fields.Remove(key);
        }

        public int Count
        {
            get
            {
                return fields.Count;
            }
        }

        /// Override
        public override string ToString()
        {
            return string.Format( "{0}: {1}", type, PrintElementsOfMap() );
        }

        /// <summary>
        /// Utility function to print the elements of a 
        /// map like the way Java does (for hashmap). 
        /// </summary>
        /// <returns></returns>
        private string PrintElementsOfMap()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append( "{" );
            int i = 0;
            foreach ( KeyValuePair<Field, Object> me in this )
            {
                if ( i != 0 )
                    sb.Append( ", " );
                sb.Append( me.Key );
                sb.Append( "=" );
                sb.Append( me.Value );
                i++;
            }

            sb.Append( "}" );

            return sb.ToString();
        }

    }
}