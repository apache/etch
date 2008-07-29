using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    public class Validator_object : TypeValidator
    {

        private static Validator_object[] validators = new Validator_object[MAX_CACHED];

        public override Validator ElementValidator()
        {
            return nDims > 0 ? Get(nDims - 1) : this;
        }

        private Validator_object(int nDims)
            : base(typeof (object), typeof (object), nDims, string.Format("object[{0}]", nDims), true)
        {
            
        }

        public static Validator_object Get( int nDims )
        {
            CheckDims(nDims);

            if (nDims >= validators.Length)
                return new Validator_object(nDims);

            Validator_object v = validators[nDims];

            if ( v == null )
                v=validators[nDims]= new Validator_object(nDims);

            return v;
        }      
    }
}
