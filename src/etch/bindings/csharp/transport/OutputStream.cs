using System;
using System.Text;
using System.Collections.Generic;

namespace Etch.Transport
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
