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
namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    public interface OutputStream
    {
        /// <summary>
        /// Closes this output stream and releases any system resources associated with this stream.
        /// </summary>
        void Close();

        /// <summary>
        /// Writes b.length bytes from the specified byte array to this output stream.
        /// </summary>
        /// <param name="b">the data</param>
        void Write(byte[] b);

        /// <summary>
        /// Writes len bytes from the specified byte array starting at offset off to this output stream.
        /// </summary>
        /// <param name="b">the data</param>
        /// <param name="off">the start offset in the data.</param>
        /// <param name="len">the number of bytes to write</param>
        void Write(byte[] b, int off, int len);

        /// <summary>
        /// Writes the specified byte to this output stream.
        /// </summary>
        /// <param name="b">the byte</param>
        void Write(int b);

        /// <summary>
        /// Get bytes from this output stream.
        /// </summary>
        /// <returns></returns>
        byte[] GetBytes();
    
    }
}
