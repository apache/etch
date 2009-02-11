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
namespace Org.Apache.Etch.Bindings.Csharp.Msg
{

   /// <summary>Field is an IdName which denotes a field of a struct or message (i.e., a key for a value).</summary>
    public class Field : IdName
    {
        ///<summary>Constructs the Field.</summary>
        ///<param name="id">id the id of the field.</param>
        ///<param name="name">name the name of the field.</param>
        public Field(int id, string name)
            : base(id, name)
        { }

        ///<param name="name">name the name of the field.</param>
        public Field(string name)
            : base(name)
        { }
    }
}