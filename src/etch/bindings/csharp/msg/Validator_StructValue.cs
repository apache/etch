using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for structvalue
    /// </summary>
    public class Validator_StructValue : TypeValidator
    {
        public static Validator_StructValue Get(XType type, int nDims)
        {
            CheckDims(nDims);          
            return new Validator_StructValue(type,nDims);
        }

       

        private Validator_StructValue(XType type, int nDims)
            : base( typeof( StructValue ), typeof( StructValue ), nDims, string.Format("StructValue[{0},{1}]",type,nDims) )
        {
            this.type = type;
        }

        private XType type;

        public XType GetXType()
        {
            return type;
        }

        public override Validator ElementValidator()
        {
            return Get(type, nDims - 1);
        }

        public override bool Validate( object value )
        {
            return
                base.Validate(value) &&
                (value.GetType() != typeof (StructValue) || type.IsAssignableFrom(((StructValue) value).GetXType));
 
        }     
    }
}
