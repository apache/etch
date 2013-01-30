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
using System.Collections.Generic;
using System.Text;
using Org.Apache.Etch.Bindings.Csharp.Msg;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    public class DummyValueFactory : ValueFactory
    {
        #region ValueFactory Members

        public XType GetType(int id)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public XType GetType(string name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void AddType(XType type)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void LockDynamicTypes()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void UnlockDynamicTypes()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public ICollection<XType> GetTypes()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public Encoding GetStringEncoding()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public long? GetMessageId(Message msg)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetMessageId(Message msg, long? msgid)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public Field Get_mf__messageId()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public long? GetInReplyTo(Message msg)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetInReplyTo(Message msg, long? msgid)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public Field Get_mf__inReplyTo()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public StructValue ExportCustomValue(object value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public object ImportCustomValue(StructValue sv)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public XType GetCustomStructType(Type c)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public XType get_mt__exception()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public Validator.Level GetLevel()
        {
            return Validator.Level.FULL;
        }

        public Validator.Level SetLevel(Validator.Level level)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion
    }
}
