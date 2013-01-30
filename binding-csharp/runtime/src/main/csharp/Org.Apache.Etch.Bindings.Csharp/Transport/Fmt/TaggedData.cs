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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Fmt
{
    /// <summary>
    /// Common interface for various types of tagged data input and output.
    /// </summary>
    abstract public class TaggedData
    {
        public TaggedData()
        { 
        }

        /// <summary>
        /// Constructs the TaggedData.
        /// </summary>
        /// <param name="vf"></param>
        public TaggedData( ValueFactory vf )
        {
            this.vf = vf;
        }

        /// <summary>
        /// The value factory to use for tagged input and output.
        /// </summary>
        protected readonly ValueFactory vf;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the value factory to use for tagged input and output.</returns>
        public ValueFactory GetValueFactory()
        {
            return vf;
        }


        protected ArrayValue ToArrayValue( Object value, Validator v )
        {

            Type c = value.GetType();
            int dim = 0;

            while ( c.IsArray )
            {
                dim++;
                c = c.GetElementType();
            }
            
            // now we want the type code for c, and if the type code is custom,
            // we'll also want the struct type code.

            sbyte typeCode = GetNativeTypeCode( c );

            XType customStructType;
            if (typeCode == TypeCode.CUSTOM || typeCode == TypeCode.STRUCT)
            {
                customStructType = GetCustomStructType(c);

                if (customStructType == null && c == typeof(StructValue))
			    {
				    Validator_StructValue x = FindStructValueValidator( v );
                    if (x != null)
                        customStructType = x.GetXType();
			    }
			
			if (customStructType == null )
				throw new ArgumentException(" In tagged Data" );
   
            }
               
            else
                customStructType = null;

            return new ArrayValue( value, typeCode, customStructType, dim );
        }

        private Validator_StructValue FindStructValueValidator( Validator v )
	    {
		    if (v is Validator_StructValue)
			return (Validator_StructValue) v;
		
		    if (v is ComboValidator)
		    {
			    ComboValidator x = (ComboValidator) v;
			    Validator_StructValue y = FindStructValueValidator( x.A() );
			
			    if (y != null)
				    return y;
			
		    	 return FindStructValueValidator( x.B() );
		    }
		
		    return null;
	    }

        protected Object FromArrayValue( ArrayValue av )
        {
            return av.GetArray();
        }

        protected Object AllocateArrayValue( sbyte typeCode, XType customStructType,
            int dim, int length )
        {
            Type clss = GetComponentType( typeCode, customStructType, dim );
            if ( clss == null )
                throw new ArgumentException(String.Format(
                    "could not get array for {0}, {1}, {2}",
                    typeCode, customStructType, dim));
            
            return Array.CreateInstance( clss, length );
        }

        private Type GetComponentType( sbyte typeCode, XType customStructType, int dim )
        {
            Type c;
            if (typeCode == TypeCode.CUSTOM || typeCode == TypeCode.STRUCT)
            {
                //    c = GetCustomType( customStructType );
                c = customStructType.GetComponentType();
                if (c == null)
                    c = typeof (StructValue);
            }
                
            else
                c = GetNativeType(typeCode);
            //Console.WriteLine( "c = " + c );
            if ( c == null )
                return null;

            if ( dim == 0 )
                return c;

            int[] dims;

            while ( dim > 0 )
            {
                dims = new int[ dim ];
                if ( dim > 1 )
                    c = c.MakeArrayType();
                dim--;
            }

            //Object o = Array.CreateInstance( c, 1 );

            //c = ( ( Array ) o ).GetType();
            //Console.WriteLine( "type= "+c );
            return c;
        }

        /// <summary>
        /// Returns the type code for the specified class. This
	    /// is needed when we have an array and we have determine
	    /// the base type and now we're fixing to serialize it.
	    /// We use the base type code to reconstitute it on the
	    /// other side. So we don't return a perfect code the way
	    /// checkValue does, but rather just something that let's
	    /// us recreate the appropriate array type on import.
        /// </summary>
        /// <param name="c"></param>
        /// <returns>a type code for the specified class.</returns>
        abstract public sbyte GetNativeTypeCode( Type c );

        /// <summary>
        /// 
        /// </summary>
        /// <param name="c"></param>
        /// <returns>a struct type for the specified custom class.</returns>
        abstract public XType GetCustomStructType( Type c );

        /// <summary>
        /// 
        /// </summary>
        /// <param name="type"></param>
        /// <returns>the class for a specified (native) type code.</returns>
        abstract public Type GetNativeType( sbyte type );

        

        abstract public sbyte CheckValue( Object value );
    }
}
