using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Base class of validators which validate based on exact type.
    /// </summary>
    abstract public class TypeValidator : Validator
    {
        
        /// <summary>
        /// Constructs the TypeValidator.
        /// </summary>
        /// <param name="scalarClass">class to use if nDims == 0</param>
        /// <param name="arrayClass">class to use if nDims > 0</param>
        /// <param name="nDims">the number of dimensions. 0 for a scalar.</param>
        /// <param name="descr"></param>
        public TypeValidator( Type scalarClass, Type arrayClass, int nDims,
            String descr )
        {
            CheckDims( nDims );

            this.expectedClass = MkExpectedClass( scalarClass, arrayClass, nDims );
            this.nDims = nDims;
            this.descr = descr;
        }

        /// <summary>
        /// Checks the number of dimensions for standard validators.
        /// </summary>
        /// <param name="nDims"></param>
        public static void CheckDims( int nDims )
        {
            if ( nDims < 0 || nDims > MAX_NDIMS )
                throw new ArgumentOutOfRangeException( "nDims < 0 || nDims > MAX_NDIMS" );
        }

        private Type expectedClass;

        /// <summary>
        /// number of dimensions if this is an array
        /// </summary>
        protected int nDims;

        public int GetNDims()
        {
            return nDims;
        }

        private String descr;

        public Type GetExpectedClass()
        {
            return expectedClass;
        }

        public override string ToString()
        {
            return descr;
        }

        public override bool Validate( object value )
        {
            return value.GetType() == expectedClass;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="scalarClass">class to use if nDims == 0</param>
        /// <param name="arrayClass">class to use if nDims > 0</param>
        /// <param name="nDims">number of dimensions, 0 for scalar</param>
        /// <returns>an appropriate class given nDims.</returns>
        /// 
        private static Type MkExpectedClass( Type scalarClass, Type arrayClass, int nDims )
        {
            if ( nDims == 0 )
                return scalarClass;

            Array tempArray = Array.CreateInstance( arrayClass, 1 );
            // Note: About C# jagged arrays
            // 
            for ( int i = 0; i < ( nDims - 1 ); i++ )
                tempArray =  Array.CreateInstance( tempArray.GetType(), 1 );

            return tempArray.GetType();
        }
    }
}
