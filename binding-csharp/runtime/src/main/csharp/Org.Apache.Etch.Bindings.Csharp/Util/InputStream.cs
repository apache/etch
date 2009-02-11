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
    public interface InputStream
    {
        /// <summary>
        /// Returns the number of bytes that can be read (or skipped over) 
        /// from this input stream without blocking by the next caller of a 
        /// method for this input stream.
        /// </summary>
        /// <returns></returns>
         int Available();

        /// <summary>
        /// Closes this input stream and releases any system resources 
        /// associated with the stream.
        /// </summary>
         void Close();

        /// <summary>
        /// Reads the next byte of data from the input stream.
        /// </summary>
        /// <returns>the next byte of data, or -1 if the end of 
        /// the stream is reached.</returns>
         int Read();

        /// <summary>
        ///  Reads some number of bytes from the input stream and stores them 
        /// into the buffer array b.
        /// </summary>
        /// <param name="buf">the buffer into which the data is read.</param>
        /// <returns>the total number of bytes read into the buffer, or -1 is 
        /// there is no more data because the end of the stream has been reached.</returns>
         int Read(byte[] buf);

        /// <summary>
        /// Reads up to len bytes of data from the input stream into an array of bytes.
        /// </summary>
        /// <param name="b">the buffer into which the data is read.</param>
        /// <param name="off">the start offset in array b  at which the data is written.</param>
        /// <param name="len">the maximum number of bytes to read.</param>
        /// <returns>the total number of bytes read into the buffer, or -1 if there is no more
        /// data because the end of the stream has been reached.</returns>
         int Read(byte[] b, int off, int len);
    }
}
