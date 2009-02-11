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

namespace Org.Apache.Etch.Bindings.Csharp.Support
{

    /// <summary>
    /// Constructs the EtchRuntimeException. Don't init anything.
    /// 
    /// Some common runtime exceptions are:
    /// 
    /// ArgumentException
    /// ArgumentNullException
    /// ArgumentOutOfRangeException
    /// ArithmeticException
    /// DivideByZeroException
    /// IndexOutOfRangeException
    /// InvalidCastException
    /// NotImplementedException
    /// NotSupportedException
    /// NullReferenceException
    /// OverflowException
    /// </summary>
    public class _Etch_RuntimeException
        : Exception
    {
        /// <summary>
        /// Constructs the EtchRuntimeException. Don't init anything.
        /// </summary>
        public _Etch_RuntimeException()
        {
            // don't init anything.
        }

       /// <summary>Constructs the EtchRuntimeException</summary>
       /// <param name = "msg">msg description of the exception that was caught by the stub on the remote side.</param>
        public _Etch_RuntimeException(String msg)
        {
            this.msg = msg;
        }

       
        public String GetMessage()
        {
            return msg;
        }

        /// <summary>Description of the exception that was caught by the stub on the remote side. </summary>
        public String msg;

        public override string ToString()
        {
            return "Remote side threw this runtime exception: " + GetMessage();
        }
    }
}