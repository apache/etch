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

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    public class Integer : IComparable, IComparable<Integer>, IEquatable<Integer>, IConvertible
    {
        public Integer(Integer other)
        {
            this.value = other.value;
        }

        public Integer(int value)
        {
            this.value = value;
        }

        private readonly int value;

        public override string ToString()
        {
            return value.ToString();
        }

        public static implicit operator Integer(int value)
        {
            return new Integer(value);
        }

        public static implicit operator int(Integer value)
        {
            return value.value;
        }

        public override bool Equals(object obj)
        {
            if (obj == this)
                return true;

            if (obj == null || obj.GetType() != GetType())
                return false;

            Integer other = (Integer)obj;
            return value == other.value;
        }

        public override int GetHashCode()
        {
            return value;
        }

        #region IComparable Members

        public int CompareTo(object obj)
        {
            if (obj is Integer)
                return CompareTo((Integer)obj);
            throw new ArgumentException("obj is not Integer");
        }

        #endregion

        #region IComparable<Integer> Members

        public int CompareTo(Integer other)
        {
            if (value < other.value) return -1;
            if (value > other.value) return 1;
            return 0;
        }

        #endregion

        #region IEquatable<Integer> Members

        public bool Equals(Integer other)
        {
            return other != null && value == other.value;
        }

        #endregion

        #region IConvertible Members

        TypeCode IConvertible.GetTypeCode()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        bool IConvertible.ToBoolean(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        byte IConvertible.ToByte(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        char IConvertible.ToChar(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        DateTime IConvertible.ToDateTime(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        decimal IConvertible.ToDecimal(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        double IConvertible.ToDouble(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        short IConvertible.ToInt16(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        int IConvertible.ToInt32(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        long IConvertible.ToInt64(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        sbyte IConvertible.ToSByte(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        float IConvertible.ToSingle(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        string IConvertible.ToString(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        object IConvertible.ToType(Type conversionType, IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        ushort IConvertible.ToUInt16(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        uint IConvertible.ToUInt32(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        ulong IConvertible.ToUInt64(IFormatProvider provider)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion
    }
}