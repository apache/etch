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
using System.Diagnostics;
using System.IO;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// A FlexBuffer wraps a byte array and manages the active region of
    /// it (0..length). It also supports dynamically extending the buffer
    /// as needed.
    /// 
    /// A FlexBuffer also has an index (read or write cursor). The various
    /// Get and put operations always occur at the current index, with the
    /// index adjusted appropriately afterward. Get will not move index past
    /// length. If put needs to move index past length, length is also
    /// adjusted. This may cause the byte array to be re-allocated to a
    /// larger size.
    /// </summary>
    public sealed class FlexBuffer
    {
        /// <summary>
        /// Constructs a FlexBuffer with initial length and index of 0.
        /// </summary>
        public FlexBuffer()
        	: this(new byte[INIT_BUFFER_LEN], 0, 0)
        {
            
        }

        /// <summary>
        /// Constructs the FlexBuffer out of an existing buffer, ready to
        /// read, with the index set to 0 and length set to buffer.length. 
        /// The buffer is adopted, not copied. If you want to copy, use one 
        /// of the put methods instead.
        /// 
        /// Note: this is the same as FlexBuffer( buffer, 0, buffer.length ).
        /// </summary>
        /// <param name="buffer">the buffer to adopt.</param>
        /// <see cref="put(byte[])"/>

        public FlexBuffer(byte[] buffer)
        	: this(buffer, 0, buffer.Length)
        {
            
        }

        /// <summary>
        /// Constructs the FlexBuffer out of an existing buffer, ready to
        /// read, with the index set to 0 and length set to buffer.length. 
        /// The buffer is adopted, not copied. If you want to copy, use one 
        /// of the put methods instead.
        /// 
        /// Note: this is the same as FlexBuffer( buffer, 0, length ).
        /// </summary>
        /// <param name="buffer">the buffer to adopt.</param>
        /// <param name="length">the length of the data in the buffer (data presumed
        /// to start at 0).</param>
        /// <see cref="put(byte[], int, int)"/>

        public FlexBuffer(byte[] buffer, int length)
        	: this(buffer, 0, length)
        {
            
        }

        /// <summary>
        /// Constructs the FlexBuffer out of an existing buffer, ready to
        /// read, with the index set to 0 and length set to buffer.length. 
        /// The buffer is adopted, not copied. If you want to copy, use one 
        /// of the put methods instead.
        /// 
        /// Note: if you want to start off writing to the end of the buffer
        /// and not reading it, specify index as the place to start writing, 
        /// and length as the amount of data that is valid after index ( which 
        /// might reasonably be 0 )
        /// </summary>
        /// <param name="buffer">the buffer to adopt.</param>
        /// /// <param name="index">the index to start reading or writing.</param>
        /// <param name="length">the length of the valid data in the buffer, 
        /// starting at the index.</param>
        /// 
        
        public FlexBuffer(byte[] buffer, int index, int length )
        {
            if ( buffer == null )
                throw new NullReferenceException( "buffer == null" );
            if ( index < 0 )
                throw new ArgumentException( " index < 0 " );
            if ( length < 0 )
                throw new ArgumentException( "length < 0 " );
            if ( index + length > buffer.Length )
                throw new ArgumentException( "index + length > buffer.Length" );

            this.buffer = buffer;
            this.index = index;
            this.length = index + length;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the current byte array. Might change if any operation
        /// needs to extend length past the end of the array.</returns>
        public byte[] GetBuf()
        {
            return buffer;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="len"></param>
        /// Exception:
        ///     throws IOException

        private void EnsureLength( int len ) 
	    {
		    int n = buffer.Length;
		    if (len <= n)
			    return;
    		
            // the buffer is not big enough, expand it

		    int k = n;
            if ( k < INIT_BUFFER_LEN )
                k = INIT_BUFFER_LEN;

            while ( len > k && k < MAX_BUFFER_LEN )
                k = k << 1;
    		
		    if (len > k)
			    throw new IOException( "buffer overflow" );
    		
		    byte[] b = new byte[k];
            Array.Copy( buffer, 0, b, 0, n );
		    buffer = b;
	    }

        private byte[] buffer;
	
	    private const int INIT_BUFFER_LEN = 32;
	    
	    private const int TRIM_BUFFER_LEN = 16*1024;
	
	    private const int MAX_BUFFER_LEN = 4*1024*1024;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the current value of length. This is the last
        /// index of valid data in the buffer.</returns>
        public int Length()
        {
            return length;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="length">length the new value of length. Length must be >= 0.
        /// If length is less than index, index is also set to length. If length
        /// is larger than the current buffer, the buffer is expanded.</param>
        /// <returns>this flex buffer object.</returns>
        /// Exception:
        ///     throws ArgumentOutOfRangeException, IOException
        public FlexBuffer SetLength( int length ) 
	    {
		    if (length < 0)
			    throw new ArgumentOutOfRangeException( "length < 0" );
    		
		    EnsureLength( length );
    		
		    this.length = length;
    		
		    if (index > length)
			    index = length;
    		
		    return this;
	    }

        private int length;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the current value of index.</returns>
        public int Index()
        {
            return index;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="index">index the new value of index. Index must be >= 0.</param>
        /// <returns>this flex buffer object.</returns>
        /// Exception:
        ///     throws ArgumentOutOfRangeException

        public FlexBuffer SetIndex( int index ) 
	    {
		    if (index < 0 || index > length)
			    throw new ArgumentOutOfRangeException( "index < 0 || index > length" );
    		
		    this.index = index;
    		
		    return this;
	    }

        private int index;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>length() - index(). Result is always >= 0. This is the amount
        /// of data that could be read using the various forms of Get. It doesn't
        /// really mean anything in relation to put.</returns>

        public int Avail()
        {
            return length - index;
        }

        /// <summary>
        /// Sets both length and index to 0.
        /// 
        /// Shorthand for SetLength( 0 ).
        /// </summary>
        /// <returns>this flex buffer object.</returns>

        public FlexBuffer Reset()
        {
            index = 0;
            length = 0;
		
			if (buffer.Length > TRIM_BUFFER_LEN)
				buffer = new byte[TRIM_BUFFER_LEN];

            return this;
        }

        /// <summary>
        /// Compacts the buffer by moving remaining data (from index to length)
        /// to the front of the buffer. Sets index to 0, and sets length to
        /// Avail (before index was changed).
        /// </summary>
        /// <returns>this flex buffer object.</returns>

        public FlexBuffer Compact()
        {
            if(index == 0)
                return this;

            int n = Avail();
            if(n == 0)
            {
                Reset();
                return this;
            }

            Array.Copy(buffer, index, buffer, 0, n);

            index = 0;
            length = n;

            return this;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the byte value at index, and adjusts index
        /// by adding one.</returns>
        /// Exception:
        ///     End of File exception / NullReferenceException

        public int Get() 
	    {
		    CheckAvail( 1 );

		    return buffer[index++] & 255;
	    }

        /// <summary>
        /// Copies data from the byte array to buf as if by repeated
        /// calls to Get().
        /// </summary>
        /// <param name="buf">a buffer to receive the data. At most
        /// min( buf.length, Avail() ) bytes are transferred.</param>
        /// <returns>the amount of data transferred.</returns>
        /// Exception:
        ///     End of File exception / NullReferenceException

        public int Get( byte[] buf ) 
	    {
            return Get( buf, 0, buf.Length );
	    }


        /// <summary>
        /// Copies data from the byte array to buf as if by repeated
        /// calls to Get().
        /// </summary>
        /// <param name="buf">a buffer to receive the data. At most
        /// min( len, Avail() ) bytes are transferred, starting
        /// at off.</param>
        /// <param name="off">the index in buf to receive the data.
        /// off must be >= 0 && less than or equal to buf.length.</param>
        /// <param name="len">the max amount of data to transfer. Len
        /// must be >= 0 and less than or equal to buf.length - off.</param>
        /// <returns>the amount of data transferred.</returns>
        /// Exception:
        ///     End of File exception / NullReferenceException

        public int Get( byte[] buf, int off, int len ) 
	    {
		    CheckBuf( buf, off, len );
    		
		    if (len == 0)
			    return 0;

            CheckAvail( 1 );
    		
		    int n = Math.Min( len, Avail() );
    		
	//	    Array.Copy( buffer, index, buf, off, n );
            Buffer.BlockCopy(buffer, index, buf, off, n);
		    index += n;
    		
		    return n;
	    }

        public sbyte GetByte()
        {
            CheckAvail( 1 );
            return (sbyte)buffer[ index++ ];
        }
        public readonly static bool littleEndian = false;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>a short composed from the next 2 bytes. Little-endian.</returns>
        /// Exception:
        ///     throws Exception if avail() < 2
        public short GetShort()
        {
            CheckAvail( 2 );

            if ( littleEndian )
            {
                // little-endian
                int value = buffer[ index++ ] & 255;
                return ( short ) ( value + ( ( buffer[ index++ ] & 255 ) << 8 ) );
            }
            else
            {
                // big-endian
                int value = buffer[ index++ ];
                return ( short ) ( ( value << 8 ) + ( buffer[ index++ ] & 255 ) );
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>an integer composed of 4 bytes from
        /// the buffer, with the first byte being the least
        /// significant and the last being the most significant.</returns>
        /// Exception:
        ///     End of File exception / NullReferenceException

        public int GetInt() 
	    {
            CheckAvail( 4 );

            if ( littleEndian )
            {
                // little-endian
                int value = buffer[ index++ ] & 255;
                value += ( ( buffer[ index++ ] & 255 ) << 8 );
                value += ( ( buffer[ index++ ] & 255 ) << 16 );
                return value + ( ( buffer[ index++ ] & 255 ) << 24 );
            }
            else
            {
                // big-endian
                int value = buffer[ index++ ];
                value = ( value << 8 ) + ( buffer[ index++ ] & 255 );
                value = ( value << 8 ) + ( buffer[ index++ ] & 255 );
                return ( value << 8 ) + ( buffer[ index++ ] & 255 );
            }
	    }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>a long comprised of the next 8 bytes. Little-endian</returns>
        public long GetLong()
        {
            CheckAvail( 8 );
            if ( littleEndian )
            {
                // little-endian
                long value = buffer[ index++ ] & 255;
                value += ( ( ( long ) ( buffer[ index++ ] & 255 ) ) << 8 );
                value += ( ( ( long ) ( buffer[ index++ ] & 255 ) ) << 16 );
                value += ( ( ( long ) ( buffer[ index++ ] & 255 ) ) << 24 );
                value += ( ( ( long ) ( buffer[ index++ ] & 255 ) ) << 32 );
                value += ( ( ( long ) ( buffer[ index++ ] & 255 ) ) << 40 );
                value += ( ( ( long ) ( buffer[ index++ ] & 255 ) ) << 48 );
                return value + ( ( ( long ) ( buffer[ index++ ] & 255 ) ) << 56 );
            }
            else
            {
                // big-endian
                long value = buffer[ index++ ];
                value = ( value << 8 ) + ( buffer[ index++ ] & 255 );
                value = ( value << 8 ) + ( buffer[ index++ ] & 255 );
                value = ( value << 8 ) + ( buffer[ index++ ] & 255 );
                value = ( value << 8 ) + ( buffer[ index++ ] & 255 );
                value = ( value << 8 ) + ( buffer[ index++ ] & 255 );
                value = ( value << 8 ) + ( buffer[ index++ ] & 255 );
                return ( value << 8 ) + ( buffer[ index++ ] & 255 );
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>a float from the next available bytes</returns>
        public float GetFloat()
        {
            return BitConverter.ToSingle( BitConverter.GetBytes( GetInt() ), 0 );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>a double from the next available bytes</returns>
        public double GetDouble()
        {
            return BitConverter.Int64BitsToDouble( GetLong() );
        }


        public void GetFully( byte[] b )
        {
            CheckAvail( b.Length );
            int n = Get( b, 0, b.Length );
            Debug.Assert( n == b.Length );
        }

        
        /// <summary>
        /// Puts a byte into the buffer at the current index,
        /// then adjusts the index by one. Adjusts the length
        /// as necessary. The buffer is expanded if needed.
        /// </summary>
        /// <param name="b">byte to be put</param>
        /// <returns>this flex buffer object.</returns>
        /// Exception: 
        ///     IOException if the buffer overflows its max length.

        public FlexBuffer Put( int b ) 
	    {
		    EnsureLength( index+1 );
		    buffer[index++] = (byte) b;
		    FixLength();
    		
		    return this;
	    }

        /// <summary>
        /// Puts some bytes into the buffer as if by repeated
        /// calls to put().
        /// </summary>
        /// <param name="buf">the source of the bytes to put. The entire
        /// array of bytes is put.</param>
        /// <returns>flex buffer object.</returns>
        /// Exception: 
        ///     IOException if the buffer overflows its max length.
        public FlexBuffer Put( byte[] buf ) 
	    {
            return Put( buf, 0, buf.Length );
	    }

        /// <summary>
        /// Puts some bytes into the buffer as if by repeated
        /// calls to Put().
        /// </summary>
        /// <param name="buf">the source of the bytes to put.</param>
        /// <param name="off">the index to the first byte to put.</param>
        /// <param name="len">the number of bytes to put.</param>
        /// <returns>flex buffer object.</returns>
        /// Exception:
        ///     IOException if the buffer overflows its max length.

        public FlexBuffer Put( byte[] buf, int off, int len ) 
	    {
		    CheckBuf( buf, off, len );
    		
		    if (len == 0)
			    return this;
    		
		    EnsureLength( index+len );
	//	    Array.Copy( buf, off, buffer, index, len );
            Buffer.BlockCopy(buf, off, buffer, index, len);
		    index += len;
		    FixLength();    
    		
		    return this;
	    }

        /// <summary>
        /// Copies the Available bytes from buf into buffer as if by
        /// repeated execution of put( buf.Get() ).
        /// </summary>
        /// <param name="buf">the source of the bytes to put. All Available
        /// bytes are copied.</param>
        /// <returns>flex buffer object.</returns>
        /// Exception: 
        ///     IOException if the buffer overflows its max length.

        public FlexBuffer Put( FlexBuffer buf ) 
	    {
            int n = buf.Avail();
            Put( buf.buffer, buf.index, n );
            buf.Skip( n, false );
		    return this;
	    }

        /// <summary>
        /// Copies the specified number of bytes from buf into buffer
        /// as if by repeated execution of Put( buf.Get() ).
        /// </summary>
        /// <param name="buf">the source of the bytes to put.</param>
        /// <param name="len"></param>len the number of bytes to put. Len must be 
        /// less than or equal to buf.Avail().
        /// <returns>flex buffer object.</returns>
        /// Exception:
        ///     IOException if the buffer overflows its max length.

        public FlexBuffer Put( FlexBuffer buf, int len ) 
	    {
		    if (len > buf.Avail())
			    throw new ArgumentOutOfRangeException( "len > buf.Avail()" );

            Put( buf.buffer, buf.index, len );
            buf.Skip( len, false );
		    return this;
	    }

        public void PutByte( byte value )
        {
            EnsureLength( index + 1 );
            buffer[ index++ ] = ( byte ) value;
            FixLength();
        }

        public void PutByte( sbyte value )
        {
            EnsureLength( index + 1 );
            buffer[ index++ ] = ( byte ) value;
            FixLength();
        }

        /// <summary>
        /// Put short as the next 2 bytes. Little-endian
        /// </summary>
        /// <param name="value"></param>
        public void PutShort( short value )
        {
            EnsureLength( index + 2 );
            if ( littleEndian )
            {
                buffer[ index++ ] = ( byte ) value;
                buffer[ index++ ] = ( byte ) ( value >> 8 );
            }
            else
            {
                /// In C#, since we're using the byte (which is unsigned),
                /// it doesn't matter whether you do logical or arithmetic 
                /// shift. Hence, an equivalent of the Java >>> operator is 
                /// not required here. 
                buffer[ index++ ] = ( byte ) ( value >> 8 );
                buffer[ index++ ] = ( byte ) value;
            }
            FixLength();
        }

        public void PutInt( int value )
        {
            EnsureLength( length + 4 );

            if ( littleEndian )
            {
                buffer[ index++ ] = ( byte ) value;
                buffer[ index++ ] = ( byte ) ( value >> 8 );
                buffer[ index++ ] = ( byte ) ( value >> 16 );
                buffer[ index++ ] = ( byte ) ( value >> 24 );
            }
            else
            {

                buffer[ index++ ] = ( byte ) ( value >> 24 );
                buffer[ index++ ] = ( byte ) ( value >> 16 );
                buffer[ index++ ] = ( byte ) ( value >> 8 );
                buffer[ index++ ] = ( byte ) value;
                
            }
            FixLength();
        }

        public void PutLong( long value )
        {
            EnsureLength( index+8 );

            if ( littleEndian )
            {
                buffer[ index++ ] = ( byte ) value;
                buffer[ index++ ] = ( byte ) ( value >> 8 );
                buffer[ index++ ] = ( byte ) ( value >> 16 );
                buffer[ index++ ] = ( byte ) ( value >> 24 );
                buffer[ index++ ] = ( byte ) ( value >> 32 );
                buffer[ index++ ] = ( byte ) ( value >> 40 );
                buffer[ index++ ] = ( byte ) ( value >> 48 );
                buffer[ index++ ] = ( byte ) ( value >> 56 );
            }
            else
            {

                buffer[ index++ ] = ( byte ) ( value >> 56 );
                buffer[ index++ ] = ( byte ) ( value >> 48 );
                buffer[ index++ ] = ( byte ) ( value >> 40 );
                buffer[ index++ ] = ( byte ) ( value >> 32 );
                buffer[ index++ ] = ( byte ) ( value >> 24 );
                buffer[ index++ ] = ( byte ) ( value >> 16 );
                buffer[ index++ ] = ( byte ) ( value >> 8 );
                buffer[ index++ ] = ( byte ) value;
                
            }
            FixLength();
        }

        public void PutFloat( float value )
        {
            PutInt( BitConverter.ToInt32( BitConverter.GetBytes( value ), 0 ) );
        }

        public void PutDouble( double value )
        {
            PutLong( BitConverter.DoubleToInt64Bits( value ) );
        }


        /// <summary>
        /// Adjusts index as if by a Get or put but without transferring
        /// any data. This could be used to skip over a data item in an
        /// input or output buffer.
        /// </summary>
        /// <param name="len">len the number of bytes to skip over. Len must be
        /// greater than or equal to 0. If put is false, it is an error if len >Avail(). 
        /// If put is true, the buffer 
        /// may be extended (and the buffer length adjusted).</param>
        /// <param name="put">put if true it is ok to extend the length of the
        /// buffer.</param>
        /// <returns>this Flexbuffer object.</returns>
        /// Exception:
        ///     IOException 
        public FlexBuffer Skip( int len, bool put ) 
	    {
		    if (len < 0)
			    throw new ArgumentException( "count < 0" );
    		
		    if (len == 0)
			    return this;
    		
		    if (put)
		    {
			    EnsureLength( index+len );
    			
			    index += len;
			    FixLength();
    			
			    return this;
		    }

            CheckAvail( len );
    		
		    index += len;
    		
		    return this;
	    }

        
        /// <summary>
        /// If index has moved past length during a put, then adjust length
        /// to track index.
        /// </summary>
        private void FixLength()
        {
            if(index > length)
                length = index;
        }


        private void CheckBuf(byte[] buf, int off, int len)
        {
            if(buf == null)
                throw new NullReferenceException("buf == null");

            if(off < 0 || off > buf.Length)
                throw new ArgumentOutOfRangeException("off < 0 || off > buf.length");

            if(len < 0)
                throw new ArgumentOutOfRangeException("len < 0");

            if(off+len > buf.Length)
                throw new ArgumentOutOfRangeException("off+len > buf.length");
        }

        /// <summary>
        /// Return the currently Available bytes.
        /// </summary>
        /// <returns>the currently Available bytes.</returns>
        /// Exception: 
        ///     throws an IO Exception
        public byte[] GetAvailBytes() 
	    {
		    byte[] buf = new byte[Avail()];
		    Get( buf );
		    return buf;
	    }

        /// <summary>
        /// Checks that there are enough bytes to for a read.
        /// </summary>
        /// <param name="len">the length required by a read operation.</param>
        private void CheckAvail( int len )
        {
            if ( len > Avail() )
                throw new EndOfStreamException( " len > Avail() " );
        }
    }
}
