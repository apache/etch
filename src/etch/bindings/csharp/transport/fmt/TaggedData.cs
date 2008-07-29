using System;
using System.Text;
using System.Collections.Generic;
using Etch.Msg;

namespace Etch.Transport.Fmt
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


        protected ArrayValue ToArrayValue( Object value )
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
            if ( typeCode == TypeCode.CUSTOM )
                customStructType = GetCustomStructType( c );
            else
                customStructType = null;

            return new ArrayValue( value, typeCode, customStructType, dim );
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
                throw new ArgumentException();
            
            return Array.CreateInstance( clss, length );
        }

        private Type GetComponentType( sbyte type, XType customStructType, int dim )
        {
            Type c;
            if ( type == TypeCode.CUSTOM )
                c = GetCustomType( customStructType );
            else
                c = GetNativeType( type );
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

        /// <summary>
        /// 
        /// </summary>
        /// <param name="type"></param>
        /// <returns>the class for a specified type.</returns>
        abstract public Type GetCustomType( XType type );

        abstract public sbyte CheckValue( Object value );
    }
}
