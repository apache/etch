"""
#  Licensed to the Apache Software Foundation (ASF) under one   *
#  or more contributor license agreements.  See the NOTICE file *
#  distributed with this work for additional information        *
#  regarding copyright ownership.  The ASF licenses this file   *
#  to you under the Apache License, Version 2.0 (the            *
#  "License"); you may not use this file except in compliance   *
#  with the License.  You may obtain a copy of the License at   *
#                                                               *
#    http://www.apache.org/licenses/LICENSE-2.0                 *
#                                                               *
#  Unless required by applicable law or agreed to in writing,   *
#  software distributed under the License is distributed on an  *
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
#  KIND, either express or implied.  See the License for the    *
#  specific language governing permissions and limitations      *
#  under the License. 
"""
from __future__ import absolute_import

from struct import pack,unpack
from ..python.Types import *
from ..python.Exceptions import *

__all__ = ['FlexBuffer']

class FlexBuffer:
    INIT_BUFFER_LEN = 32
    MAX_BUFFER_LEN  = 4*1024*1024
    littleEndian    = False

    def __init__(self, *args):
        """
        Constructs the FlexBuffer out of an existing buffer, ready to
        read, with the specified index and length. The buffer is
        adopted, not copied. If you want a copy, use a put method
        instead.

        Note: if you want to start off writing to the end of the buffer
        and not reading it, specify index as the place to start writing,
        and length as the amount of data that is valid after index
        (which might reasonably be 0).

        @param buffer     (optional) the buffer to adopt
        @param index      (optional but requires buffer and length) the place to start reading and writing
        @param length     (optional but requires buffer) the length of the valid data in the buffer,
        starting at index
        """

        cls = self.__class__

        if len(args) == 0:
            buffer = ByteArray(cls.INIT_BUFFER_LEN)
            index  = 0
            length = 0
        elif len(args) == 1:
            buffer = args[0]
            if not isinstance(buffer, ByteArray):
                raise IllegalArgumentException, "buffer must be a ByteArray"
            index = 0
            length = len(buffer)
        elif len(args) == 2:
            buffer = args[0]
            if not isinstance(buffer, ByteArray):
                raise IllegalArgmentException, "buffer must be a ByteArray"
            index = 0
            length = args[1]
            if not isinstance(length, (int, long)):
                raise IllegalArgumentException, "length must be an integer"
        elif len(args) == 3:
            buffer = args[0]
            index  = args[1]
            length = args[2]
            if not isinstance(buffer, ByteArray):
                raise IllegalArgumentException, "Buffer must be a ByteArray"
            if not isinstance(index, (int,long)) or not isinstance(length, (int, long)):
                raise IllegalArgumentException, "index and length must be integers"
        else:
            raise IllegalArgumentException, "Invalid arguments to constructor"

        if buffer == None:
            raise NullPointerException, "buffer cannot be None"

        if index < 0:
            raise IllegalArgumentException, "index < 0"

        if length < 0:
            raise IllegalArgumentException, "length < 0"

        if index+length > len(buffer):
            raise IllegalArgumentException, "index+length > buffer.length()"

        self._buffer = buffer
        self._index  = index
        self._length = index+length

    def length(self):
        """
        @return the current value of length. This is the last index of
        valid data in the buffer.
        """
        return self._length

    def getLength(self):
        """
        Alias for FlexBuffer.length()
        """
        return self.length()

    def setLength(self, length):
        """
        @param length    the new value of length. Length must be >= 0.
        If length < index, index is also set to length. If length is
        larger than current buffer, the buffer is expanded.
        @return    this flex buffer object
        @throws IllegalArgumentException    if length < 0
        @throws IOException                 if the buffer overflows
        max length.
        """
        if length < 0:
            raise IllegalArgumentException, "length < 0"

        self.ensureLength(length)
        self._length = length
        if self._index > length:
            self._index = length

        return self

    def ensureLength(self, length):
        """
        Ensure buffer is at least 'length' in size, if it is
        not, expand it.

        @param length    the length to test
        """
        #print "ensureLength(%s)" % repr(length)
        cls = self.__class__
        n = len(self._buffer)
        if length <= n:
            #print "return %d <= %d" % (length, n)
            return

        # Not big enough, time to expand
        k = n
        if k < self.INIT_BUFFER_LEN:
            k = self.INIT_BUFFER_LEN

        while length > k and k < cls.MAX_BUFFER_LEN:
            k <<= 1

        if length > k:
            #print "IOException: %d > %d" % (length, k)
            raise IOException, "buffer overflow"

        #print "creating new bytearray of size(%d)" % k
        b = ByteArray(k)
        ByteArray.copy(self._buffer, 0, b, 0, n)
        self._buffer = b
        #print "New length: self._buffer = %d" % len(self._buffer)

    def fixLength(self):
        """
        If the index has moved past length during a put, then adjust
        length to track index.
        """
        if self._index > self._length:
            self._length = self._index

    def index(self):
        """
        @return the current value of index
        """
        return self._index

    def getIndex(self):
        """
        Alias for FlexBuffer.index()
        """
        return self.index()

    def setIndex(self, index):
        """
        @param index    the new value of index. Index must be >= 0.
        @return         this flex buffer object
        @throws IllegalArgumentException if index < 0 or index > length.
        """
        if index < 0 or index > self._length:
            raise IllegalArgumentException, "index < 0 || index > length"

        self._index = index
        return self

    def avail(self):
        """
        @return length-index. Result is always >= 0. This is the ammount
        of data that could be read using the various forms of get. It
        doesn't really mean anything in relation to put.
        """
        return self._length - self._index

    def checkAvail(self, length):
        """
        Checks that there are enough bytes for a read.

        @param length    the length required by the read operation.
        @throws EOFException if length > avail()
        """
        if length > self.avail():
            raise EOFException, "length > avail()"

    def reset(self):
        """
        Sets both the index and length to 0. Shorthand for FlexBuffer.setLength(0)

        @return this flex buffer object
        """
        return self.setLength(0)

    def compact(self):
        """
        Compacts the buffer by moving remaining data (from index to
        length) to the front of the buffer. Sets the index to 0, and
        sets length to avail (before index is changed).

        @return this flex buffer object
        """
        if self._index == 0:
            return self

        n = self.avail()
        if n == 0:
            return self.reset()

        ByteArray.copy(self._buffer, self._index, self._buffer, 0, n)
        self._index  = 0
        self._length = n
        return self

    def skip(self, length, putFlag):
        """
        Adjust index as if by a get or put but without transferring any
        data. This could be used to skip over a data item in an input
        or output buffer.

        @param length    the number of bytes to skip over. length must
        be >= 0. If putFlag is False, it is an error if length > avail().
        If putFlag is True, the buffer may be extended (and the buffer
        length adjusted).
        @param putFlag    if True it is ok to extend the length of the
        buffer.
        @return     this flex buffer object
        @throws    EOFException if putFlag is False and length > avail()
        @throws    IOException if the max buffer size is exceeded
        """
        if length < 0:
            raise IllegalArgumentException, "length < 0"

        if length == 0:
            return self

        if putFlag:
            self.ensureLength(self._index + length)
            self._index += length
            self.fixLength()
            return self

        self.checkAvail(length)
        self._index += length
        return self

    def getBuf(self):
        """
        @return the current ByteArray. Might change if any operation
        needs to extend length past end of array.
        """
        return self._buffer

    def checkBuf(self, buffer, offset, length):
        """
        Checks buffer, offset and length for being reasonable.

        @param buffer
        @param offset
        @param length
        """
        if buffer == None:
            raise NullPointerException, "buffer is None"

        if offset < 0 or offset > len(buffer):
            raise IllegalArgumentException, "offset < 0 or offset > len(buffer)"

        if length < 0:
            raise IllegalArgumentException, "length < 0"

        if offset+length > len(buffer):
            raise IllegalArgumentException, "offset+length > len(buffer)"

    def get(self, *args, **kwargs):
        """
        Copies data from the byte array. If no receive buffer is
        specified a single, unsigned byte value is returned. If a
        receive buffer is specified, the number of bytes are returned.
        If the offset and len are specified, they must be within the
        limits of buf.

        @param buf  (optional) a buffer to receive the data
        @param offset (optional, but requires buf and length) offset
        in buf to receive the data
        @param length (optional, but requires buf and offset) maximum
        amount of data to transfer.
        @returns the unsigned byte value if no buffer specified
        otherwise returns the amount of data transferred.
        @throws IOException if avail() < 1
        """
        if len(args) == 0:
            self.checkAvail(1)
            i = self._index
            self._index += 1
            # TODO - confirm this makes this an unsigned byte value
            return self._buffer[i] & 0xff

        if len(args) == 1:
            buffer = args[0]
            if not isinstance(buffer, ByteArray):
                raise IllegalArgumentException, "buffer must be ByteArray"

            offset = 0
            length = len(buffer)
        elif len(args) == 3:
            buffer = args[0]
            if not isinstance(buffer, ByteArray):
                raise IllegalArgumentException, "buffer must be ByteArray"

            offset = args[1]
            length = args[2]
            if not isinstance(offset, (int,long)) or not isinstance(length, (int,long)):
                raise IllegalArgumentException, "offset and length must be integers"
        else:
            raise IllegalArgumentException, "Wrong number of arguments"

        self.checkBuf(buffer, offset, length)
        if length == 0:
            return 0

        self.checkAvail(1)
        n = min(length, self.avail())
        ByteArray.copy(self._buffer, self._index, buffer, offset, n)
        self._index += n
        return n

    def getAvailBytes(self):
        """
        @return the currently available bytes.
        @throws IOException
        """
        buf = ByteArray(self.avail())
        self.get(buf)
        return buf

    def getFully(self, buffer):
        """
        Fills a buffer fully with the next available bytes

        @param buffer
        @throws IOException if avail() < buffer.length()
        """
        if not isinstance(buffer, ByteArray):
            raise IllegalArgumentException, "buffer must be a ByteArray"

        self.checkAvail(len(buffer))
        n = self.get(buffer, 0, len(buffer))
        if n != len(buffer):
            raise IOException, "avail() < buffer.length()"

    def getByte(self):
        """
        @return the next available byte
        @raise IOException if avail() < 1
        """
        self.checkAvail(1)
        i = self._index
        self._index += 1
        return self._buffer[i]
    
    def _getBytesAsString(self, length):
        self.checkAvail(length)
        start       = self._index
        end         = start + length
        self._index = end
        return ByteArray.toString(self._buffer[start:end])
    
    def getShort(self):
        """
        @return a short composed from the next 2 bytes (littleEndian)
        @raise IOException if avail() < 2.
        """
        s = self._getBytesAsString(2)
        
        if self.littleEndian:
            fmt = "<h"
        else:
            fmt = ">h"
        
        return Short(unpack(fmt, s)[0])
    
    def getInt(self):
        """
        @return an int composed from the next 4 bytes
        @raise IOException if avail() < 4.
        """
        s = self._getBytesAsString(4)
        if self.littleEndian:
            fmt = "<i"
        else:
            fmt = ">i"
        return Integer(unpack(fmt, s)[0])

    def getLong(self):
        """
        @return a long composed from the next 8 bytes.
        @raise IOException if avail() < 8
        """
        s = self._getBytesAsString(8)
        if self.littleEndian:
            fmt = "<q"
        else:
            fmt = ">q"
        
        return Long(unpack(fmt, s)[0])
    
    def getFloat(self):
        """
        @return a float from the next available bytes
        @raise IOException if avail() < 4
        """
        return Float.intBitsToFloat(self.getInt())
    
    def getDouble(self):
        """
        @return a double from the next available bytes
        @raise IOException if avail() < 8
        """
        return Double.longBitsToDouble(self.getLong())
    
    def _put(self, b):
        """
        Puts a byte into the buffer at the current index, then
        adjusts the index by one. Adjusts the length as necessary. The
        buffer is expanded as needed.
        
        @param b
        @return this flex buffer object
        @raise IOException if the buffer overflows its max length.
        """        
        if not isinstance(b, (int, long)):
            raise IllegalArgumentException, "b must be an integer"
        
        i = self._index
        self.ensureLength(i + 1)
        self._buffer[i] = Byte(b)
        self._index = i + 1
        self.fixLength()
        return self
    
    def _putByteArray(self, buffer, offset=0, length=0):
        """
        Puts some bytes into the buffer as if by repeated calls to put.
        
        @param buf    the source of the bytes to put
        @param off    the index to the first byte to put
        @param len    the number of bytes to put
        @return   this flex buffer object
        @raise IOException if the buffer overflows its max length
        """
        self.checkBuf(buffer, offset, length)
        if length == 0:
            return self
        
        self.ensureLength(self._index + length)
        
        ByteArray.copy(buffer, offset, self._buffer, self._index, length)
        self._index += length
        self.fixLength()
        
        return self
    
    def _putFlexBuffer(self, buffer, length=None):
        """
        Copies the available bytes from the buffer into this buffer as if by
        repeated execution of put(buffer.get()).
        
        @param buf   the flex buffer object
        @param length (optional)
        @return this flex buffer object
        @raise IOException if the buffer overflows its max length
        """
        if length != None:
            if length > buffer.avail():
                raise IllegalArgumentException, "length > buffer.avail()"
            n = length
        else:
            n = buffer.avail()
            
        self.put( buffer.getBuf(), buffer.index(), n)
        buffer.skip(n, False)
        return self

    
    def put(self, *args):
        """
        Put bytes in the buffer.
        
        @param b         a single byte -or- a ByteArray -or- a FlexBuffer
        @param offset    (optional - requires b -> ByteArray) offset in ByteArray
        @param length    (optional) length of ByteArray or FlexBuffer to copy
        @return          this flex buffer object
        @raise IOException if the buffer overflows its max length
        @raise IllegalArgumentException on type errors
        """
        
        if len(args) == 0:
            raise IllegalArgumentException, "missing arguments"
        elif len(args) == 1:
            if isinstance(args[0], ByteArray):
                return self._putByteArray(args[0], 0, len(args[0]))
            elif isinstance(args[0], FlexBuffer):
                return self._putFlexBuffer(args[0])
            elif isinstance(args[0], (int, long)):
                return self._put(args[0])
            elif args[0] == None:
                raise NullPointerException, "buffer cannot be None"
            else:
                raise IllegalArgumentException, "buffer must be int, or ByteArray or FlexBuffer"
        elif len(args) == 2:
            if isinstance(args[0], FlexBuffer):
                return self._putFlexBuffer(*args)
            elif args[0] == None:
                raise NullPointerException, "buffer cannot be None"
            else:
                raise IllegalArgumentException, "buffer must be FlexBuffer"
        elif len(args) == 3:
            if isinstance(args[0], ByteArray):
                return self._putByteArray(*args)
            elif args[0] == None:
                raise NullPointerException, "buffer cannot be None"
            else:
                raise IllegalArgumentException, "buffer must be ByteArray"
        else:
            raise IllegalArgumentException, "too many arguments"
        

    def putByte(self, value):
        """
        Puts a byte
        @param value
        @raise IOException
        """
        self.put(value)
    
    def putShort(self, value):
        """
        Put a short as the next 2 bytes.
        
        @param value
        @raise IOException
        """        
        if self.littleEndian:
            fmt = "<h"
        else:
            fmt = ">h"
        
        self.put(ByteArray(pack(fmt, Short(value))))
    
    def putInt(self, value):
        """
        Put a int as the next 4 bytes
        
        @param value
        @raise IOException
        """
        if self.littleEndian:
            fmt = "<i"
        else:
            fmt = ">i"
        
        self.put(ByteArray(pack(fmt, Integer(value))))
    
    def putLong(self, value):
        """
        Put a long as the next 8 bytes
        
        @param value
        @raise IOException
        """
        if self.littleEndian:
            fmt = "<q"
        else:
            fmt = ">q"
        
        self.put(ByteArray(pack(fmt, Long(value))))
    
    def putFloat(self, value):
        """
        Put the float as the next 4 bytes.
        
        @param value
        @raise IOException
        """
        self.putInt(Float.floatToIntBits(value))
        
    def putDouble(self, value):
        """
        Put the double as the next 8 bytes.
        
        @param value
        @raise IOException
        """
        self.putLong(Double.doubleToLongBits(value))
