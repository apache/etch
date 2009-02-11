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

namespace Org.Apache.Etch.Bindings.Csharp.Msg
{
    /// <summary>
    /// class used to implement ValueFactory based import / export code
    /// for service defined types.
    /// </summary>
    public class ImportExportHelper
    {
        protected ImportExportHelper()
        {
            ev = delegate(ValueFactory vf, Object value)
            {
                throw new NotImplementedException("must override ExportValue method");
            };
            iv = delegate(StructValue sv)
            {
                throw new NotImplementedException("must override ImportValue method");
            };
        }

        public ImportExportHelper( ExportValueDelegate ev, ImportValueDelegate iv )
        {
            if (ev == null)
                throw new ArgumentNullException("ev == null");

            if (iv == null)
                throw new ArgumentNullException("iv == null");

            this.ev = ev;
            this.iv = iv;
        }

        private readonly ExportValueDelegate ev;

        private readonly ImportValueDelegate iv;

        /// <summary>
        /// Exports a value by creating an equivalent StructValue.
        /// </summary>
        /// <param name="vf">the value factory used to export.</param>
        /// <param name="value">a service defined type to export.</param>
        /// <returns>a StructValue representing the exported value.</returns>
        public virtual StructValue ExportValue( ValueFactory vf, Object value )
        {
            return ev( vf, value );
        }

        /// <summary>
        /// Imports a value from an equivalent StructValue.
        /// </summary>
        /// <param name="sv">a StructValue representing the exported value.</param>
        /// <returns>a service defined type.</returns>
        public virtual Object ImportValue( StructValue sv )
        {
            return iv( sv );
        }

        /// <summary>
        /// Exports a value by creating an equivalent StructValue.
        /// </summary>
        /// <param name="vf">the value factory to use for export.</param>
        /// <param name="value">a service defined type to export.</param>
        /// <returns>a StructValue representing the exported value.</returns>
        public delegate StructValue ExportValueDelegate(ValueFactory vf, Object value);

        /// <summary>
        /// Imports a value from an equivalent StructValue.
        /// </summary>
        /// <param name="sv">a StructValue representing the exported value.</param>
        /// <returns>a service defined type.</returns>
        public delegate Object ImportValueDelegate( StructValue sv );
    }
}
