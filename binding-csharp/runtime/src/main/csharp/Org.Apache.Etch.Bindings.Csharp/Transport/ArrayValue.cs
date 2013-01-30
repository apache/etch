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
using System.Collections;
using Org.Apache.Etch.Bindings.Csharp.Msg;

/// <summary>An array of values, where each value is of arbitrary type
/// chosen from the basic java types boolean, byte, short, int,
/// long, float, double, String, an array of those, the extended
/// types ArrayValue and StructValue, and specific types supported
/// by ValueFactory. 
/// 
/// ArrayValue is not protected against concurrent access. </summary>

namespace Org.Apache.Etch.Bindings.Csharp.Transport
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


        #region IEnumerable Members

        public IEnumerator GetEnumerator()
        {
            return ( ( Array ) array ).GetEnumerator();
        }

        #endregion
    }
}