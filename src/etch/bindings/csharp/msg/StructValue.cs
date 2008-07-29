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
        /// <summary> Constructs the StructValue. </summary>
        /// <param name="type"> type the type of the struct (or the action or event if this is a message).</param>
        public StructValue(XType type)
        {
            if ( type == null )
                throw new ArgumentNullException( "type == null" );

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

       /// <summary>Writes a struct to the tagged data output.</summary>
       /// <param name="tdo">tdo the tagged data output to write to.</param> 
       /// Exception:
       ///          throws IOException if there is a problem with the tagged data output.
        //public void WriteStruct(TaggedDataOutput tdo)
        //{
        //    tdo.StartStruct(this);
        //    WriteKeysAndValues(tdo);
        //    tdo.EndStruct(this);
        //}


        ///// <summary>Writes the key / value pairs.</summary>
        ///// <param name="tdo">tdo the tagged data output to write to.</param> 
        ///// Exception:
        /////         throws IOException if there are problems with the tagged data output.
        //public void WriteKeysAndValues(TaggedDataOutput tdo)
        //{
        //    foreach(KeyValuePair<Field, Object> me in this) 
        //        tdo.WriteStructElement(me.Key , me.Value); //me.getKey();me.getValue
        //}

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

       /// <param name="obj">obj an object to compare to this struct value. </param>
        /// <param name="helper">helper implementation to help with comparing comformant but not identical 
        /// objects (e.g., 23 represented as Byte vs. Integer).</param>
        /// <returns>true if the object is a struct value and compares equal with this one (relative to helper).
        /// </returns>
        //public bool Equals(Object obj, Equals helper)
        //{
        //    if ( obj == null )
        //        return false;

        //    if ( GetType() != obj.GetType() )
        //        return false;

        //    StructValue other = ( StructValue ) obj;

        //    if ( this.type != other.type )
        //        return false;

        //    Dictionary<Field, Object>.KeyCollection keySet = Keys;
        //    Dictionary<Field, Object>.KeyCollection otherKeySet = other.Keys;

        //    // Check for equal keysets

        //    foreach ( Field f in otherKeySet )
        //    {
        //        if ( ! this.ContainsKey( f ) )
        //            return false;
        //    }

        //    foreach ( Field f in keySet )
        //    {
        //        if ( ! other.ContainsKey( f ) )
        //            return false;
        //    }
   

        //    foreach ( Field mf in keySet )
        //    {
        //        Object a = Get( mf );
        //        Object b = other.Get( mf );
        //        if ( helper != null )
        //        {
        //            if ( !helper( a, b ) )
        //                return false;
        //        }
        //        else if ( !ObjEquals( a, b ) )
        //        {
        //            return false;
        //        }
        //    }

        //    return true;
        //}

        ///// <summary>Compares two objects for equality taking into account the fact 
        ///// that one or both might be null.</summary>
        ///// <param name="a">a an object to compare.</param> 
        ///// <param name="b">b an object to compare. </param>
        ///// <returns>true if both are null or if both are not null and a.equals( b ).</returns>
        //private static bool ObjEquals(Object a, Object b)
        //{
        //    if(a == null && b == null)
        //        return true;

        //    if(a == null || b == null)
        //        return false;

        //    return a.Equals(b);
        //}
    }
}