///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 11, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.


using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace Etch.Msg
{

    ///A typed map of key/value pairs, where the type is a Type, each key is a Field, 
    ///and each value is of arbitrary type chosen from the basic java types boolean, byte, 
    ///short, int, long, float, double, String, an array of those, the extended types ArrayValue 
    ///and StructValue, and specific types supported by ValueFactory. 
    ///StructValue is not protected against concurrent access.
    public class StructValue : Dictionary<Field, Object> 
    {
        ///<summary> Constructs the StructValue. </summary>
        ///<param name="type"> type the type of the struct (or the action or event if this is a message).</param>
        public StructValue(XType type)
        {
            if ( type == null )
                throw new ArgumentNullException( "type == null" );

            this.type = type;
        }

        /// <summary> Constructs the StructValue. </summary>
        /// <param name="type"> type the type of the struct (or the action or event if this is a message).</param>
        /// <param name="length"> the expected number of name/value pairs.</param>
        public StructValue(XType type, int length): base(length)
        {
            if (type == null)
                throw new ArgumentNullException("type == null");

            this.type = type;
        }



        private readonly XType type;

       /// <returns>the struct type.</returns>
        public XType GetXType
        {
            get
            {
                return type;
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

        public Enumerator Iterator()
        {
            return this.GetEnumerator();
        }

        /// <summary>Defined to get the Object return type</summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public Object Get(Field key)
        {
            try
            {
                return this[key];
            }
            catch(KeyNotFoundException) 
            { return null; }
        }

      
        /// <summary>
        /// Overriding Dictionary's Add(key, value) function
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        public new void Add( Field key, Object value )
        {
            if ( value == null )
            {
                Remove( key );
                return;
            }

            // TODO disable this, we don't need it except as an
            // extra safety net during development.
            if ( true )
            {
                Validator v = type.GetValidator( key );
                if ( v == null || !v.Validate( value ) )
                    throw new ArgumentException( String.Format( "validator {0} failed for type {1} field {2} : value {3}", v, type, key, value ) );
            }

            //base.Add( key, value );
            this[ key ] = value;
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