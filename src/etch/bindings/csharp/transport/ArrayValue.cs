/// $Id$
/// Created by Champakesan, Badri Narayanan on Jun 11, 2007.
/// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using System.Collections.Generic;
using Etch.Msg;
using System.Collections;

/// <summary>An array of values, where each value is of arbitrary type
/// chosen from the basic java types boolean, byte, short, int,
/// long, float, double, String, an array of those, the extended
/// types ArrayValue and StructValue, and specific types supported
/// by ValueFactory. 
/// 
/// ArrayValue is not protected against concurrent access. </summary>

namespace Etch.Transport
{

    public class ArrayValue : IEnumerable
    {
        /// <summary>Constructs the ArrayValue.</summary>
        public ArrayValue( Object array, sbyte typecode, XType customStructType, int dim )
        {
            if ( array == null )
                throw new NullReferenceException( "array == null" );

            if ( !( ( array.GetType() ).IsArray ) )
                throw new ArgumentException( "object is not of type array" );

            this.array = array;
            this.typecode = typecode;
		    this.CustomStructType = customStructType;
		    this.Dim = dim;
        }

        /// <summary>
        /// Constructs the arrayvalue without any type information
        /// </summary>
        public ArrayValue( Object array ) : this( array, ( sbyte ) 0, null, 0 )
        {
        }

        private Object array;
        private sbyte typecode;
        private XType customStructType;
        private int dim;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the TypeCode for this array value.
        /// For example, if the array is int[][], then
        /// the type would be TypeCode.INT4.
        /// </returns>
        public sbyte Type
        {
            get
            {
                return typecode;
            }
            set
            {
                typecode = value;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <returns>a struct type if a custom type code.</returns>
        public XType CustomStructType
        {
            get
            {
                return customStructType;
            }
            private set
            {
                customStructType = value;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the dimsionality of the array.
        /// For example, if the array is int[][], the
        /// dimensionality would be 2.</returns>
        public int Dim
        {
            get
            {
                return dim;
            }
            private set
            {
                dim = value;
            }
        }

        public int Size()
        {
            return ( ( Array ) array ).Length;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="index"></param>
        /// <returns>the element at the specified index</returns>
        public Object Get( int index )
        {
            return ( ( Array ) array ).GetValue( index );
        }

        /// <summary>
        /// Reallocates the array so that it is only as long as needed.
        /// </summary>
        public void Compact()
        {
            if ( addIndex == Size() )
                return;

            Object narray = Array.CreateInstance( array.GetType().GetElementType(), addIndex );
            Array.Copy( (Array)array, (Array)narray, addIndex );
            array = narray;
        }

        /// <summary>
        /// Adds the value to the end of the array, making more space
        /// if needed
        /// </summary>
        /// <param name="value"></param>
        public void Add( Object value )
        {
            int n = Size();

            if ( addIndex >= n )
            {
                if ( n == 0 )
                    n = 8;
                else
                    n *= 2;

                Object narray = Array.CreateInstance( array.GetType().GetElementType(), n );
                Array.Copy( (Array)array, (Array)narray, ( ( Array ) array ).Length );
                array = narray;
            }
            ( ( Array ) array ).SetValue( value, addIndex++ );
        }

        private int addIndex;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the array</returns>
        public Object GetArray()
        {
            return array;
        }


        /// <summary>Adds many values to the array value.</summary>
        /// <param name="values"></param>
        /// <returns> current object</returns>
       // public ArrayValue AddMany( params Object[] values )
       // {
       //     foreach (Object v in values)
       //         Add( v );
       //     return this;
       // }

       // /// <summary>Writes an array and all of its values.</summary>
       // /// <param name="tdo">tdo the tagged data output to write to.</param> 
       // /// <throws>IOException if there is a problem writing to the tagged data output.</throws>
       // public void Write(TaggedDataOutput tdo)
       // {
       //     tdo.StartArray(this);
       //     WriteValues(tdo);
       //     tdo.EndArray(this);
       // }

       ///// <summary>Writes the values of the array.</summary>
       ///// <param name="tdo">tdo the tagged data output to write to.</param> 
       // /// <throws>IOException if there is a problem writing to the tagged data output.</throws>
       // private void WriteValues(TaggedDataOutput tdo)
       // {
       //     foreach(Object value in this) // for (Object value: this)
       //         tdo.WriteArrayElement(value);
       // }

       ///// <summary>Compares two array values for equality with some help.</summary>
       ///// <param name="obj">obj an object to compare to this array value.</param> 
       // /// <param name="helper">helper implementation to help with comparing comformant but not identical objects (e.g., small
       ///// integer represented as Byte vs. Integer).</param> 
       // /// <returns>true if the object is an array value and compares equalwith this one (relative to helper).</returns>
       // public bool Equals(Object obj, Equals helper) //override removed
       // {
       //     if(obj == null)
       //         return false;

       //     if(this.GetType() != obj.GetType())
       //         return false;

       //     ArrayValue other = (ArrayValue) obj;

       //     if(this.Count != other.Count)
       //         return false;

       //     Enumerator vals = GetEnumerator();
       //     Enumerator otherVals = other.GetEnumerator();
       //     while(vals.MoveNext())
       //     {
       //         otherVals.MoveNext();
       //         Object a = vals.Current;
       //         Object b = otherVals.Current;
       //         if(helper != null)
       //         {
       //             if(!helper(a, b))
       //                 return false;
       //         }
       //         else if(!ObjEquals(a, b))
       //         {
       //             return false;
       //         }
       //     } 

       //     return true;
       // }

       ///// <summary>Compares two objects for equality taking into account the fact that one or both might be null.
       ///// </summary>
       ///// <param name="a"> a an object to compare.</param>
       // /// <param name="b"> b an object to compare.</param>
       // /// <returns>true if both are null or if both are not null and a.equals( b ).</returns>
       // private static bool ObjEquals(Object a, Object b)
       // {
       //     if(a == null && b == null)
       //         return true;

       //     if(a == null || b == null)
       //         return false;

       //     return a.Equals(b);
       // }

       // public override string ToString()
       // {
       //     String stringBuilder = null;
       //     foreach ( Object value in this )
       //         stringBuilder = stringBuilder + " [ " + value + " ] " ;

       //     return stringBuilder;
       // }

        //#region IEnumerable<object> Members

        //public IEnumerator<object> GetEnumerator()
        //{
        //    return ( ( Array ) array ).GetEnumerator();
        //}

        //#endregion

        //#region IEnumerable<object> Members

        //public IEnumerator<object> GetEnumerator()
        //{
        //    return ( ( Array ) array ).GetEnumerator();
        //}

        //#endregion

        #region IEnumerable Members

        public IEnumerator GetEnumerator()
        {
            return ( ( Array ) array ).GetEnumerator();
        }

        #endregion
    }
}