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
import unittest
from etch.python.Types import *
from etch.python.Exceptions import *
from etch.util.FlexBuffer import *

class Test_EtchUtilFlexBuffer(unittest.TestCase):
    
    def setUp(self):
        self.buf = FlexBuffer()
    
    def _checkBuf(self, length, index, avail):
        self.assertEqual(length, self.buf.length(), "Length")
        self.assertEqual(index,  self.buf.index(),  "Index")
        self.assertEqual(avail,  self.buf.avail(),  "Avail")
    
    def test_initial1(self):
        self._checkBuf(0, 0, 0)
    
    def test_initial2(self):
        self.buf = FlexBuffer(ByteArray(5))
        self._checkBuf(5, 0, 5)

    def test_initial3(self):
        self.buf = FlexBuffer(ByteArray(5), 2)
        self._checkBuf(2, 0, 2)
        
    def test_initial4(self):
        self.buf = FlexBuffer(ByteArray(5), 1, 3)
        self._checkBuf(4, 1, 3)
    
    def test_setLength0(self):
        self.buf.setLength(0)
        self._checkBuf(0, 0, 0)
        
    def test_setLength1(self):
        self.buf.setLength(1)
        self._checkBuf(1, 0, 1)
        
    def test_setLength2(self):
        self.buf.setLength(2)
        self._checkBuf(2, 0, 2)
        
    def test_setLength3(self):
        self.buf.setLength(2)
        self.buf.setLength(0)
        self._checkBuf(0, 0, 0)
        
    def test_setLength4(self):
        self.assertRaises(IllegalArgumentException, self.buf.setLength, -1)
    
    def test_setIndex0(self):
        self.buf.setIndex(0)
        self._checkBuf(0, 0, 0)
    
    def test_setIndex1(self):
        self.assertRaises(IllegalArgumentException, self.buf.setIndex, 1)
        
    def test_setIndex2(self):
        self.assertRaises(IllegalArgumentException, self.buf.setIndex, -1)
        
    def test_setIndex3(self):
        self.buf.setLength(5)
        self.buf.setIndex(1)
        self._checkBuf(5, 1, 4)
        
    def test_setIndex4(self):
        self.buf.setLength(5)
        self.buf.setIndex(4)
        self._checkBuf(5, 4, 1)
        
    def test_setIndex5(self):
        self.buf.setLength(5)
        self.buf.setIndex(5)
        self._checkBuf(5, 5, 0)
        
    def test_setIndex5(self):
        self.buf.setLength(5)
        self.buf.setIndex(5)
        self.buf.setLength(0)
        self._checkBuf(0, 0, 0)
        
    def test_setIndex7(self):
        self.buf.setLength(5)
        self.buf.setIndex(5)
        self.buf.setLength(2)
        self._checkBuf(2, 2, 0)
    
    def test_reset(self):
        self.buf.setLength(2)
        self.buf.setIndex(1)
        self.buf.reset()
        self._checkBuf(0, 0, 0)
    
    def test_put1a(self):
        self.buf.put(1)
        self._checkBuf(1, 1, 0)
        
    def test_put1b(self):
        self.buf.put(1)
        self.buf.put(2)
        self._checkBuf(2, 2, 0)
        
    def _testPutBytes(self, bufLength):
        self.buf.put(1)
        if bufLength >= 0:
            self.buf.put(ByteArray(bufLength))
        else:
            self.buf.put(None)
        self._checkBuf(bufLength+1, bufLength+1, 0)
    
    def test_put2a(self):
        self.assertRaises(NullPointerException, self._testPutBytes, -1)
    
    def test_put2b(self):
        self._testPutBytes(0)
    
    def test_put2c(self):
        self._testPutBytes(1)
        
    def test_put2d(self):
        self._testPutBytes(2)
        
    #// ------------------------------------------------------
    #// cases for put( byte[] buf, int off, int len ):
    #// buf.length, off, len // failure reason
    #// --- group 3:
    #// null, 0, 0 // fail (buf is null)
    #// null, 0, 1 // fail (buf is null)
    #// null, 1, 0 // fail (buf is null)
    #// --- group 4:
    #// 0, 0, 0
    #// 0, 0, 1 // fail (off+len > buf.length)
    #// 0, 1, 0 // fail (off > buf.length)
    #// --- group 5:
    #// 1, 0, 0
    #// 1, 0, 1
    #// 1, 0, 2 // fail (off+len > buf.length)
    #// 1, 1, 0
    #// 1, 1, 1 // fail (off+len > buf.length)
    #// 1, 1, 2 // fail (off+len > buf.length)
    #// 1, 2, 0 // fail (off > buf.length)
    #// 1, 2, 1 // fail (off > buf.length)
    #// 1, 2, 2 // fail (off > buf.length)
    #// --- group 6:
    #// 2, 0, 0
    #// 2, 0, 1
    #// 2, 0, 2
    #// 2, 1, 0
    #// 2, 1, 1
    #// 2, 1, 2 // fail (off+len > buf.length)
    #// 2, 2, 0    
    #// 2, 2, 1 // fail (off+len > buf.length)
    #// 2, 2, 2 // fail (off+len > buf.length)
    
    def _testPutBytesOffLen(self, bufLength, offset, length):
        self.buf.put(1)
        if bufLength >= 0:
            self.buf.put(ByteArray(bufLength), offset, length)
        else:
            self.buf.put(None, offset, length)
        self._checkBuf(length+1, length+1, 0)
    
    def test_put3a(self):
        self.assertRaises(NullPointerException, self._testPutBytesOffLen, -1, 0, 0)
    
    def test_put3b(self):
        self.assertRaises(NullPointerException, self._testPutBytesOffLen, -1, 0, 0)
        
    def test_put3c(self):
        self.assertRaises(NullPointerException, self._testPutBytesOffLen, -1, 1, 0)
        
    def test_put4a(self):
        self._testPutBytesOffLen(0, 0, 0)
        
    def test_put4b(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 0, 0, 1)
    
    def test_put4c(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 0, 1, 0)
    
    def test_put5a(self):
        self._testPutBytesOffLen(1, 0, 0)
    
    def test_put5b(self):
        self._testPutBytesOffLen(1, 0, 1)
        
    def test_put5c(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 1, 0, 2)
    
    def test_put5d(self):
        self._testPutBytesOffLen(1, 1, 0)
    
    def test_put5e(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 1, 1, 1)
        
    def test_put5f(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 1, 1, 2)
    
    def test_put5g(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 1, 2, 0)
        
    def test_put5h(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 1, 2, 1)
        
    def test_put5i(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 1, 2, 2)
    
    def test_put6a(self):
        self._testPutBytesOffLen(2, 0, 0)
        
    def test_put6b(self):
        self._testPutBytesOffLen(2, 0, 1)
        
    def test_put6c(self):
        self._testPutBytesOffLen(2, 0, 2)
        
    def test_put6d(self):
        self._testPutBytesOffLen(2, 1, 0)
        
    def test_put6e(self):
        self._testPutBytesOffLen(2, 1, 1)
        
    def test_put6f(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 2, 1, 2)
        
    def test_put6g(self):
        self._testPutBytesOffLen(2, 2, 0)
    
    def test_put6h(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 2, 2, 1)
        
    def test_put6i(self):
        self.assertRaises(IllegalArgumentException, self._testPutBytesOffLen, 2, 2, 2)

    #// ------------------------------------------------------
    #// cases for put(int), get():
    #// put 0, get 0. // already tested above
    #// put 0, get 1. // fails with EOFException
    #// put 1, get 0.
    #// put 1, get 1.
    #// put 1, get 2. // fails with EOFException
    #// put 2, get 0.
    #// put 2, get 1.
    #// put 2, get 2.
    #// put 2, get 3. // fails with EOFException
    #// put 10000, get 10000.
    
    #// just like above, putting byte array instead
    #// just like above, getting byte array instead.
    #// just like above, putting and getting byte array.

    def _testPutGet(self, nPuts, nGets):
        for i in range(0, nPuts):
            self.buf.put(i)
        
        self.buf.setIndex(0)
        for i in range(0, nGets):
            self.assertEqual(Byte(i), Byte(self.buf.get()))
        
        self._checkBuf(nPuts, nGets, nPuts-nGets)
    
    def _testPutBytesGet(self, nPuts, nGets):
        buffer = ByteArray(nPuts)
        for i in range(0, nPuts):
            buffer[i] = Byte(i)
        
        self.buf.put(buffer)
        self.buf.setIndex(0)
        
        for i in range(0, nGets):
            self.assertEquals(Byte(i), Byte(self.buf.get()))
        
        self._checkBuf(nPuts, nGets, nPuts - nGets)
    
    def _testPutGetBytes(self, nPuts, nGets):
        for i in range(0, nPuts):
            #print "%s" % repr(i)
            self.buf.put(i)
            
        self.buf.setIndex(0)
        
        buffer = ByteArray(nGets)
        n = self.buf.get(buffer)
        self.assertEquals(min(nPuts, nGets), n)
        
        for i in range(0, n):
            self.assertEquals(Byte(i), buffer[i])
        
        self._checkBuf(nPuts, n, nPuts-n)
        
    def _testPutBytesGetBytes(self, nPuts, nGets):
        buffer = ByteArray(nPuts)
        for i in range(0, nPuts):
            buffer[i] = Byte(i)
        
        self.buf.put(buffer)
        self.buf.setIndex(0)
        
        buffer = ByteArray(nGets)
        n = self.buf.get(buffer)
        self.assertEquals(min(nPuts, nGets), n)
        
        for i in range(0, n):
            self.assertEquals(Byte(i), buffer[i])
        
        self._checkBuf(nPuts, n, nPuts-n)
        
    def test_get1a(self):
        self.assertRaises(EOFException, self._testPutGet, 0, 1)
        
    def test_get1b(self):
        self._testPutGet(1, 0)
        
    def test_get1c(self):
        self._testPutGet(1, 1)
        
    def test_get1d(self):
        self.assertRaises(EOFException, self._testPutGet, 1, 2)
        
    def test_get1e(self):
        self._testPutGet(2, 0)
    
    def test_get1f(self):
        self._testPutGet(2, 1)
    
    def test_get1g(self):
        self._testPutGet(2, 2)
        
    def test_get1h(self):
        self.assertRaises(EOFException, self._testPutGet, 2, 3)
    
    def test_get1i(self):
        self._testPutGet(10000, 9995)
    
    def test_get1j(self):
        self._testPutGet(10000, 10000)
        
    def test_get1k(self):
        self._testPutGet(100000, 100000)
    
    def test_get2a(self):
        self.assertRaises(EOFException, self._testPutBytesGet, 0, 1)
    
    def test_get2b(self):
        self._testPutBytesGet(1, 0)
        
    def test_get2d(self):
        self.assertRaises(EOFException, self._testPutBytesGet, 1, 2)
        
    def test_get2e(self):
        self._testPutBytesGet(2, 0)
    
    def test_get2f(self):
        self._testPutBytesGet(2, 1)
        
    def test_get2g(self):
        self._testPutBytesGet(2, 2)
        
    def test_get2h(self):
        self.assertRaises(EOFException, self._testPutBytesGet, 2, 3)
        
    def test_get2i(self):
        self._testPutBytesGet(10000, 9995)
        
    def test_get2j(self):
        self._testPutBytesGet(10000, 10000)
        
    def test_get2k(self):
        self._testPutBytesGet(100000, 100000)
        
    def test_get3a(self):
        self.assertRaises(EOFException, self._testPutBytesGet, 0, 1)
    
    def test_get3b(self):
        self._testPutGetBytes(1, 0)
        
    def test_get3c(self):
        self._testPutGetBytes(1, 1)
    
    def test_get3d(self):
        self._testPutGetBytes(1, 2)
        
    def test_get3e(self):
        self._testPutGetBytes(2, 0)
        
    def test_get3f(self):
        self._testPutGetBytes(2, 1)
    
    def test_get3g(self):
        self._testPutGetBytes(2, 2)
    
    def test_get3h(self):
        self._testPutGetBytes(2, 3)
    
    def test_get3i(self):
        self._testPutGetBytes(10000, 9995)
        
    def test_get3j(self):
        self._testPutGetBytes(10000, 10000)
        
    def test_get3k(self):
        self._testPutGetBytes(100000, 100000)
        
    def test_get4a(self):
        self.assertRaises(EOFException, self._testPutBytesGetBytes, 0, 1)
    
    def test_get4b(self):
        self._testPutBytesGetBytes(1, 0)
        
    def test_get4c(self):
        self._testPutBytesGetBytes(1, 1)
    
    def test_get4d(self):
        self._testPutBytesGetBytes(1, 2)
        
    def test_get4e(self):
        self._testPutBytesGetBytes(2, 0)
    
    def test_get4f(self):
        self._testPutBytesGetBytes(2, 1)
        
    def test_get4g(self):
        self._testPutBytesGetBytes(2, 2)
    
    def test_get4h(self):
        self._testPutBytesGetBytes(2, 3)
    
    def test_get4i(self):
        self._testPutBytesGetBytes(10000, 9995)
    
    def test_get4j(self):
        self._testPutBytesGetBytes(10000, 10000)
        
    def test_get4k(self):
        self._testPutBytesGetBytes(100000, 100000)
    
    def test_get5a(self):
        self.buf = FlexBuffer(ByteArray([1,2,3]))
        self.assertEquals(1, self.buf.get())
        self.assertEquals(2, self.buf.get())
        self.assertEquals(3, self.buf.get())
        self._checkBuf(3, 3, 0)
    
    def test_get5b(self):
        self.buf = FlexBuffer(ByteArray([1,2,3]), 2)
        self.assertEquals(1, self.buf.get())
        self.assertEquals(2, self.buf.get())
        self._checkBuf(2, 2, 0)
        
    def test_get5c(self):
        self.buf = FlexBuffer(ByteArray([1,2,3,4,5]),1,2)
        self.assertEquals(2, self.buf.get())
        self.assertEquals(3, self.buf.get())
        self._checkBuf(3, 3, 0)

    def test_get6(self):
        self.buf = FlexBuffer(ByteArray([5,6,7,8]))
        buffer   = ByteArray([11,12,13,14,15])
        self.buf.get(buffer, 1, 3)
        
        self.assertEquals(11, buffer[0])
        self.assertEquals(5,  buffer[1])
        self.assertEquals(6,  buffer[2])
        self.assertEquals(7,  buffer[3])
        self.assertEquals(15, buffer[4])
    
    def test_compact(self):
        self.buf = FlexBuffer(ByteArray([1,2,3,4,5]), 1, 3)
        self.buf.compact()
        self._checkBuf(3, 0, 3)
        self.assertEquals(2, self.buf.get())
        self.assertEquals(3, self.buf.get())
        self.assertEquals(4, self.buf.get())
        
    def test_getByte1(self):
        self.buf = FlexBuffer(ByteArray([127, 1, 0, -1, -128]))
        self.assertEquals(127, self.buf.getByte())
        self.assertEquals(1, self.buf.getByte())
        self.assertEquals(0, self.buf.getByte())
        self.assertEquals(-1, self.buf.getByte())
        self.assertEquals(-128, self.buf.getByte())
        self._checkBuf(5, 5, 0)
    
    def test_getByte2(self):
        self.assertRaises(EOFException, self.buf.getByte)
    
    def test_getShort1(self):
        self.buf = FlexBuffer(ByteArray([1,2,-1,-2]))
        self.assertEquals(Short(0x0102), self.buf.getShort())
        self.assertEquals(Short(0xfffe), self.buf.getShort())
        self._checkBuf(4, 4, 0)
    
    def test_getShort2(self):
        self.assertRaises(EOFException, self.buf.getShort)
    
    def test_getInt1(self):
        self.buf = FlexBuffer(ByteArray([1,2,3,4,-1,-2,-3,-4]))
        self.assertEquals(Integer(0x01020304), self.buf.getInt())
        self.assertEquals(Integer(0xfffefdfc), self.buf.getInt())
        self._checkBuf(8, 8, 0)
        
    def test_getInt2(self):
        self.assertRaises(EOFException, self.buf.getInt)
        
    def test_getLong1(self):
        self.buf = FlexBuffer(ByteArray([1,2,3,4,5,6,7,8,-1,-2,-3,-4,-5,-6,-7,-8]))
        self.assertEquals(Long(0x0102030405060708), self.buf.getLong())
        self.assertEquals(Long(0xfffefdfcfbfaf9f8), self.buf.getLong())
        self._checkBuf(16, 16, 0)
        
    def test_getLong2(self):
        self.assertRaises(EOFException, self.buf.getLong)
        
    def test_getFloat1(self):
        
        self.buf = FlexBuffer(ByteArray([
               0, 0, 0, 0, 63, -128, 0, 0, 83, -113, 48, -37, 100, -98, -112, -120,
             117, -120, 20, 25, -65, -128, 0, 0, -45, -113, 48, -37, -28, -98, -112, -120,
             -11, -120, 20, 25, 50, 83, 79, -7, 34, 44, -87, 90, 17, -38, -85, 115,
             -78, 83, 79, -7, -94, 44, -87, 90, -111, -38, -85, 115]))
        
        self.assertEquals( 0.0,      self.buf.getFloat() )
        self.assertEquals( 1.0,      self.buf.getFloat() )
        self.assertEquals( 123e10,   self.buf.getFloat() )
        self.assertEquals( 234e20,   self.buf.getFloat() )
        self.assertEquals( 345e30,   self.buf.getFloat() )
        self.assertEquals( -1.0,     self.buf.getFloat() )
        self.assertEquals( -123e10,  self.buf.getFloat() )
        self.assertEquals( -234e20,  self.buf.getFloat() )
        self.assertEquals( -345e30,  self.buf.getFloat() )
        self.assertEquals( 123e-10,  self.buf.getFloat() )
        self.assertEquals( 234e-20,  self.buf.getFloat() )
        self.assertEquals( 345e-30,  self.buf.getFloat() )
        self.assertEquals( -123e-10, self.buf.getFloat() )
        self.assertEquals( -234e-20, self.buf.getFloat() )
        self.assertEquals( -345e-30, self.buf.getFloat() )
        
        self._checkBuf(15*4, 15*4, 0)
        
    def test_getFloat2(self):
        self.assertRaises(EOFException, self.buf.getFloat)
    
    def test_getDouble1(self):
        # TODO - double conversions appear to be off compared to Java
        self.buf = FlexBuffer(ByteArray([
            0, 0, 0, 0, 0, 0, 0, 0
            , 63, -16, 0, 0, 0, 0, 0, 0
            , 66, 113, -26, 27, 104, -64, 0, 0
            , 85, 48, -73, 88, 68, 89, -6, -80
            , 105, -4, 43, 112, 122, -31, 67, -116
            , -65, -16, 0, 0, 0, 0, 0, 0
            , -62, 113, -26, 27, 104, -64, 0, 0
            , -43, 48, -73, 88, 68, 89, -6, -80
            , -23, -4, 43, 112, 122, -31, 67, -116
            , 62, 74, 105, -1, 27, 85, 80, 81
            , 43, -87, -105, 64, -36, -23, -48, -66
            , 22, -16, -127, 73, 111, 91, -75, -29
            , -66, 74, 105, -1, 27, 85, 80, 81
            , -85, -87, -105, 64, -36, -23, -48, -66
            , -106, -16, -127, 73, 111, 91, -75, -29
        ]))
                
        self.assertEquals( 0.0,              self.buf.getDouble())
        self.assertEquals( 1.0,              self.buf.getDouble())
        self.assertEquals( 123e10,           self.buf.getDouble())
        self.assertEquals( 234e100,          self.buf.getDouble())
        self.assertEquals( 345e200,          self.buf.getDouble())
        self.assertEquals( -1.0,             self.buf.getDouble())
        self.assertEquals( -123e10,          self.buf.getDouble())
        self.assertEquals( -234e100,         self.buf.getDouble())
        self.assertEquals( -345e200,         self.buf.getDouble())
        self.assertEquals( 123e-10,          self.buf.getDouble())
        self.assertEquals( 234e-100,         self.buf.getDouble())
        self.assertEquals( 345e-200,         self.buf.getDouble())
        self.assertEquals( -123e-10,          self.buf.getDouble())
        self.assertEquals( -234e-100,         self.buf.getDouble())
        self.assertEquals( -345e-200,         self.buf.getDouble())

        self._checkBuf(15*8, 15*8, 0)
        
    def test_getDouble2(self):
        self.assertRaises(EOFException, self.buf.getDouble)
        
    def test_getFully1(self):
        self.buf = FlexBuffer(ByteArray([1,2,3,4,5,6]))
        b = ByteArray(3)
        self.buf.getFully(b)
        self.assertEquals(1, b[0])
        self.assertEquals(2, b[1])
        self.assertEquals(3, b[2])
        self.buf.getFully(b)
        self.assertEquals(4, b[0])
        self.assertEquals(5, b[1])
        self.assertEquals(6, b[2])
        self._checkBuf(6, 6, 0)
    
    def test_getFully2(self):
        self.buf = FlexBuffer(ByteArray([1,2]))
        b = ByteArray(3)
        #self.buf = self.buf.getFully(b)
        self.assertRaises(EOFException, self.buf.getFully, b)
    
    # // ------------------------------------------------------
    # // cases for FlexBuffer put( FlexBuffer ):
    # // put, put FlexBuffer with valid buffer
    # // put, put Flexbuffer to a Flexbuffer approaching max length, fails with IOException.
    
    def test_putFlexBuffer1(self):
        buf0     = FlexBuffer(ByteArray([1,2]))
        self.buf = FlexBuffer(ByteArray([]))
        self.buf.put(buf0)
        self._checkBuf(buf0.length(), buf0.length(), 0)
    
    def test_putFlexBuffer2(self):
        buf0 = FlexBuffer(ByteArray([1,2]))
        max  = 4*1024*1024
        self.buf = FlexBuffer(ByteArray(max))
        self.buf.setIndex(max)
        # TODO - Confirm this is the correct test
        self.assertRaises(IOException, self.buf.put, buf0)
        #self._checkBuf(max + buf0.length(), max + buf0.length(), 0)
    
    #// ------------------------------------------------------
    #// cases for FlexBuffer put( FlexBuffer, int ):
    #// put, put FlexBuffer with length larger than available bytes, fails with IllegalArgumentException
    #// put, put FlexBuffer with valid length value
    #// put, put Flexbuffer to a Flexbuffer approaching max length, fails with IOException.

    def test_putFlexBuffer3(self):
        buf0 = FlexBuffer(ByteArray([1,2]))
        self.buf = FlexBuffer(ByteArray([]))
        self.buf.setIndex(0)
        # TODO - confirm this is the correct test
        self.assertRaises(IllegalArgumentException, self.buf.put, buf0, 3)
        #self._checkBuf(3,3,0)
    
    def test_putFlexBuffer4(self):
        buf0 = FlexBuffer(ByteArray([1,2]))
        self.buf = FlexBuffer(ByteArray([]))
        self.buf.put(buf0, 1)
        self._checkBuf(1,1,0)
        
    def test_putFlexBuffer5(self):
        buf0 = FlexBuffer(ByteArray([1,2]))
        max  = 4*1024*1024
        self.buf = FlexBuffer(ByteArray(max))
        self.buf.setIndex(max)
        self.assertRaises(IOException, self.buf.put, buf0, 1)
        #self._checkBuf(max+1, max+1, 0)
    
    #// ------------------------------------------------------
    #// cases for void putByte( byte ):
    #// putByte, put byte value and expect getByte to return matched value.
    #// putByte to a Flexbuffer approaching max length, fails with IOException.
    
    def test_putByte1(self):
        self.buf = FlexBuffer(ByteArray())
        self.buf.putByte(0x01)
        self._checkBuf(1, 1, 0)
        self.buf.setIndex(0)
        self.assertEquals(0x01, self.buf.getByte())
        self._checkBuf(1, 1, 0)
        
    def test_putByte2(self):
        max = 4*1024*1024
        self.buf = FlexBuffer(ByteArray(max))
        self.buf.setIndex(max)
        self.assertRaises(IOException, self.buf.putByte, 0x01)
        #self._checkBuf(max+1, max+1, 0)
    
    #// ------------------------------------------------------
    #// cases for void putDouble( double ):
    #// putDouble, put Double and expect getDouble to return matched value.
    #// putDouble to a Flexbuffer approaching max length, fails with IOException.    
    def test_putDouble1(self):
        self.buf = FlexBuffer(ByteArray([]))
        self.buf.putDouble(Double.MAX_VALUE)
        self._checkBuf(8,8,0)
        self.buf.setIndex(0)
        self.assertEquals(Double.MAX_VALUE, self.buf.getDouble())
        self._checkBuf(8,8,0)
    
    def test_putDouble2(self):
        max = 4*1024*1024
        self.buf = FlexBuffer(ByteArray(max))
        self.buf.setIndex(max)
        self.assertRaises(IOException, self.buf.putDouble, Double.MIN_VALUE)
        #self._checkBuf(max+8, max+8, 0)

    #// ------------------------------------------------------
    #// cases for void putFloat( float ):
    #// putFloat, put Float and expect getFloat to return matched value.
    #// putFloat to a Flexbuffer approaching max length, fails with IOException.    
        
    def test_putFloat1(self):
        self.buf = FlexBuffer(ByteArray([]))
        self.buf.putFloat(Float.MAX_VALUE)
        self._checkBuf(4, 4, 0)
        self.buf.setIndex(0)
        self.assertEquals(Float.MAX_VALUE, self.buf.getFloat())
        self._checkBuf(4, 4, 0)
    
    def test_putFloat2(self):
        max = 4*1024*1024
        self.buf = FlexBuffer(ByteArray(max))
        self.buf.setIndex(max)
        # TODO - confirm this is the correct test
        self.assertRaises(IOException, self.buf.putFloat, Float.MIN_VALUE)
        #self._checkBuf(max+4, max+4, 0)
    
    #// ------------------------------------------------------
    #// cases for void putInt( int ):
    #// putInt, put Int and expect getInt to return matched value.
    #// putInt to a Flexbuffer approaching max length, fails with IOException.
    
    def test_putInt1(self):
        self.buf = FlexBuffer(ByteArray([]))
        self.buf.putInt(Integer.MAX_VALUE)
        self._checkBuf(4, 4, 0)
        self.buf.setIndex(0)
        self.assertEquals(Integer.MAX_VALUE, self.buf.getInt())
        self._checkBuf(4 ,4, 0)
    
    def test_putInt2(self):
        max = 4*1024*1024
        self.buf = FlexBuffer(ByteArray(max))
        self.buf.setIndex(max)
        # TODO - validate test
        self.assertRaises(IOException, self.buf.putInt, Integer.MIN_VALUE)
        #self._checkBuf(max+4, max+4, 0)
    
    #// ------------------------------------------------------
    #// cases for void putLong( long ):
    #// putLong, put Long and expect getLong to return matched value.
    #// putShort to a Flexbuffer approaching max length, fails with IOException.
    def test_putLong1(self):
        self.buf = FlexBuffer(ByteArray([]))
        self.buf.putLong(Long.MAX_VALUE)
        self._checkBuf(8, 8, 0)
        self.buf.setIndex(0)
        self.assertEquals(Long.MAX_VALUE, self.buf.getLong())
        self._checkBuf(8, 8, 0)
    
    def test_putLong2(self):
        max = 4*1024*1024
        self.buf = FlexBuffer(ByteArray(max))
        self.buf.setIndex(max)
        # TODO - confirm this is the correct test
        self.assertRaises(IOException, self.buf.putLong, Long.MIN_VALUE)
        #self._checkBuf(max+8, max+8, 0)
    
    #// ------------------------------------------------------
    #// cases for void putShort( short ):
    #// putShort, put short and expect getShort to return matched value.
    #// putShort to a Flexbuffer approaching max length, fails with IOException.
    
    def test_putShort1(self):
        self.buf = FlexBuffer(ByteArray([]))
        self.buf.putShort(Short.MAX_VALUE)
        self._checkBuf(2, 2, 0)
        self.buf.setIndex(0)
        self.assertEquals(Short.MAX_VALUE, self.buf.getShort())
        self._checkBuf(2, 2, 0)
    
    def test_putShort2(self):
        max = 4*1024*1024
        self.buf = FlexBuffer(ByteArray(max))
        self.buf.setIndex(max)
        # TODO - confirm this correct test
        self.assertRaises(IOException, self.buf.putShort, 1)
        #self._checkBuf(max+2, max+2, 0)
    
    #// ------------------------------------------------------
    #// cases for FlexBuffer skip( int, boolean ):
    #// skip length < 0, put = false.                  // fails with illegal argument
    #// skip length > buffer length, put = false.      // fails with EOFException
    #// skip length = 0, put = false.
    #// skip length = buffer length, put = false.
    #// skip length < buffer length, put = false.
    #// skip length > buffer length, put = true. 
    #// skip length < buffer length, put = true.    
    #// skip length = max buffer length, put = true.    // fails with IOException
    
    def test_skip1(self):
        self.buf = FlexBuffer(ByteArray([]))
        self.assertRaises(IllegalArgumentException, self.buf.skip, -1, False)
    
    def test_skip2(self):
        self.buf = FlexBuffer(ByteArray([]))
        self.assertRaises(EOFException, self.buf.skip, 2, False)
        
    def test_skip3(self):
        self.buf = FlexBuffer(ByteArray([1, 2]))
        self.buf.skip(2, False)
        self._checkBuf(2, 2, 0)
    
    def test_skip4(self):
        self.buf = FlexBuffer(ByteArray([1,2]))
        self.buf.skip(2, False)
        self._checkBuf(2, 2, 0)
    
    def test_skip5(self):
        self.buf = FlexBuffer(ByteArray([1,2]))
        self.buf.skip(1, False)
        self._checkBuf(2, 1, 1)
        
    def test_skip6(self):
        self.buf = FlexBuffer(ByteArray([1,2]))
        self.buf.skip(1, True)
        self._checkBuf(2, 1, 1)
    
    def test_skip7(self):
        self.buf = FlexBuffer(ByteArray([1,2]))
        self.buf.skip(5, True)
        self._checkBuf(5, 5, 0)
    
    def test_skip8(self):
        self.buf = FlexBuffer(ByteArray([1,2]))
        max = 4*1024*1024
        # TODO - confirm this is the correct test
        self.assertRaises(IOException, self.buf.skip, max+1, True)
        #self._checkBuf(max+1, max+1, 0)

if __name__ == '__main__':
    unittest.main()
