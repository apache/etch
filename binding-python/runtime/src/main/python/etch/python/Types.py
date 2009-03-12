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
import array
import types
import struct
from .Exceptions import *

Boolean = types.BooleanType

class SignedFixedInteger(long):
    BITSIZE   = 8
    MASK      = (2**BITSIZE) - 1
    MAX_VALUE = (MASK/2)
    MIN_VALUE = -MAX_VALUE - 1
    SIGNMASK  = MASK - MAX_VALUE + 1

    def __new__(cls,i):
        cls.MASK      = (2 ** cls.BITSIZE) - 1
        cls.MAX_VALUE = (cls.MASK / 2)
        cls.MIN_VALUE = -cls.MAX_VALUE - 1
        cls.SIGNMASK  = cls.MASK - cls.MAX_VALUE + 1

        return long.__new__(cls, cls.box(i))

    @classmethod
    def box(cls, i):
        s = i
        if abs(i) > cls.MAX_VALUE:
            s = (i & cls.MASK) - cls.MASK - 1
            if s < cls.MIN_VALUE:
                s = s + cls.MASK + 1
        return s

    def __add__(self,b):
        #return self.__class__(int(self) + int(b))
        return self.__class__(super(SignedFixedInteger,self).__add__(b))

    def __sub__(self,b):
        #return self.__add__(-b)
        return self.__class__(super(SignedFixedInteger,self).__sub__(b))

    def __mul__(self,b):
        #return self.__class__(int(self) * int(b))
        return self.__class__(super(SignedFixedInteger,self).__mul__(b))

    def __lshift__(self,b):
        #return self.__class__(int(self) << int(b))
        return self.__class__(super(SignedFixedInteger,self).__lshift__(b))

    def __rshift__(self,b):
        #return self.__class__(int(self) >> int(b))
        return self.__class__(super(SignedFixedInteger, self).__rshift__(b))

    def __rmul__(self,b):
        return self.__mul__(b)

    def __radd__(self,b):
        return self.__add__(b)

    def __rsub__(self,b):
        # TODO - rsub
        #super(SignedFixedInteger, self).__rsub__(x)
        return self.__class__(super(SignedFixedInteger,self).__rsub__(b))

class ByteArray(array.array):
    
    def __new__(cls, i=None):
        if i == None:
            initializer = None
        elif isinstance(i, (int, long)) and i >= 0:
            initializer = [0 for x in range(0,i)]
        elif isinstance(i, (list, tuple)):
            initializer = [Byte(x) for x in i]
        elif isinstance(i, str):
            initializer = [Byte(ord(x)) for x in i]
        else:
            raise IllegalArgumentException, "Must initialize with a size or a list of integers"
        
        if initializer:
            return array.array.__new__(cls,'b',initializer)
        else:
            return array.array.__new__(cls,'b')
        
    @staticmethod
    def copy(srcBuffer, srcIndex, destBuffer, destIndex, length):
        # TODO - implement ByteArray.copy
        srcEndIndex  = srcIndex + length
        destEndIndex = destIndex + length
        destBuffer[destIndex:destEndIndex] = srcBuffer[srcIndex:srcEndIndex]

    @staticmethod
    def toString(byteArray, encoding='UTF-8'):
        # TODO - implement ByteArray.toString, account for encoding
        return "".join([chr(x&0xff) for x in byteArray])

class Byte(SignedFixedInteger):
    BITSIZE   = 8
    MASK      = (2**BITSIZE) - 1
    MAX_VALUE = (MASK/2)
    MIN_VALUE = -MAX_VALUE - 1
    SIGNMASK  = MASK - MAX_VALUE + 1

class Short(SignedFixedInteger):
    BITSIZE   = 16
    MASK      = (2**BITSIZE) - 1
    MAX_VALUE = (MASK/2)
    MIN_VALUE = -MAX_VALUE - 1
    SIGNMASK  = MASK - MAX_VALUE + 1

class Integer(SignedFixedInteger):
    BITSIZE   = 32
    MASK      = (2**BITSIZE) - 1
    MAX_VALUE = (MASK/2)
    MIN_VALUE = -MAX_VALUE - 1
    SIGNMASK  = MASK - MAX_VALUE + 1

class Long(SignedFixedInteger):
    BITSIZE   = 64
    MASK      = (2**BITSIZE) - 1
    MAX_VALUE = (MASK/2)
    MIN_VALUE = -MAX_VALUE - 1
    SIGNMASK  = MASK - MAX_VALUE + 1

class Float(float):
    MIN_VALUE = float(1.4012984643248170e-45)
    MAX_VALUE = float(3.4028234663852886e+38)    
    
    @staticmethod
    def floatToIntBits(value):
        a = 0
        for i in map(ord, struct.pack('>f', Float(value))):
            a = (a<<8) + i
        return a           
            
    @staticmethod
    def intBitsToFloat(value):
        v = value & 0xffffffff
        a = []
        for i in range(0,4):
            a.append(chr(v&0xff))
            v >>= 8
        a.reverse()    
        s = "".join(a)
        return Float(struct.unpack('>f',s)[0])
    
class Double(float):
    MIN_VALUE = float(4.9000000000000000e-324)
    MAX_VALUE = float(1.7976931348623157e+308)

    @staticmethod
    def longBitsToDouble(value):
        v = value & 0xffffffffffffffff
        a = []
        for i in range(0,8):
            a.append(chr(v&0xff))
            v >>= 8
        a.reverse()    
        s = "".join(a)
        return Double(struct.unpack('>d',s)[0])
        
    @staticmethod
    def doubleToLongBits(value):
        a = 0
        for i in map(ord, struct.pack('>d', Double(value))):
            a = (a<<8)+i
        return a

class String(str):
    def getBytes(self, encoding):
        # TODO - account for encoding
        return self
