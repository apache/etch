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

from .....python.Exceptions import *
from .....python.Types import *
from ..TypeCode import *
from .BinaryTaggedData import *
from ...TaggedDataInput import *
from ...ArrayValue import *
from ....msg.StructValue import *
from ....msg.Message import *
from .....util.Flexbuffer import *
from ....support.Validator_int import *

class BinaryTaggedDataInput(BinaryTaggedData,TaggedDataInput):
    
    def __init__(self, valueFactory, buffer=None, uri=None):
        super(BinaryTaggedDataInput,self).__init__(valueFactory)
        if buffer:
            self.__buffer = buffer
        ## Nothing to do for 'uri' yet
        
        self.intValidator = Validator_int.get(0)
    
    def setBuffer(self, buffer):
        self.__buffer = buffer
    
    def readMessage(self):
        msg = self.startMessage()
        self.readKeysAndValues(msg)
        self.endMessage(msg)
        return msg
    
    def readStruct(self):
        sv = self.startStruct()
        self.readKeysAndValues(sv)
        self.endStruct(sv)
        return sv
    
    def readArray(self, validator):
        av = self.startArray()
        self.readValues(av, validator)
        self.endArray(av)
        return av
    
    def readKeysAndValues(self, sv):
        t = sv.type()
        while True:
            obj = self.readValue(self.intValidator, True)
            if obj == BinaryTaggedData.NONE:
                break

            key   = t.getField(Integer(obj))
            v     = t.getValidator(key)
            value = self.readValue(v)
            sv.put(key, value)
    
    def readValues(self, av, v):
        ev = v.elementValidator()
        while True:
            value = self.readValue(ev, True)
            if value == BinaryTaggedData.NONE:
                break
            
            av.add(value)
    
    ## MAIN INPUT METHODS
    
    def startMessage(self):
        version = self.__buffer.getByte()
        
        if version != BinaryTaggedData.VERSION:
            raise IOException, "binary tagged data version mismatch: got %d expected %d" % (version, BinaryTaggedData.VERSION)
        
        return Message(self.readType(), self.valueFactory )
    
    def endMessage(self):
        # Nothing to do
        pass
    
    def startStruct(self):
        return StructValue(self.readType())
    
    def endStruct(self):
        # Nothing to do
        pass
    
    def startArray(self):
        t = self.__buffer.getByte()
        
        if t == TypeCode.CUSTOM:
            customStructType = self.readType()
        else:
            customStructType = None
        
        dim = self.readValue(self.intValidator)
        length = 8
        
        ar = self.allocArrayValue(t, customStructType, dim, length)
        return ArrayValue(ar, t, customStructType, dim)

    def endArray(self, ar):
        ar.compact()
    
    def readType(self):
        i = Integer(self.readValue(self.intValidator))
        return self.valueFactory.getType(i)
    
    # Local Utility Methods
    
    def validateValue(self, v, noneOk, value):
        if not v:
            raise IOException, "no validator"
        
        if not value:
            return None
        
        if value == BinaryTaggedData.NONE and noneOk:
            return value
        
        if not v.validate(value):
            raise IOException, "validator '%s' failed for value '%s'" % (repr(v),repr(value))
        
        return value
    
    def readValue(self, v, noneOk=False):
        typ = self.__buffer.getByte()
        
        if   typ == TypeCode.NULL:
            return self.validateValue(v, noneOk, None)
        
        elif typ == TypeCode.BOOLEAN_FALSE:
            return self.validateValue(v, noneOk, False)
        
        elif typ == TypeCode.BOOLEAN_TRUE:
            return self.validateValue(v, noneOk, True)
        
        elif typ in (TypeCode.BYTE1, TypeCode.SHORT1, TypeCode.INT1, TypeCode.LONG1):
            return self.validateValue(v, noneOk, self.__buffer.getByte())
        
        elif typ in (TypeCode.SHORT2, TypeCode.INT2, TypeCode.LONG2):
            return self.validateValue(v, noneOk, self.__buffer.getShort())
        
        elif typ in (TypeCode.INT4, TypeCode.LONG4):
            return self.validateValue(v, noneOk, self.__buffer.getInt())
        
        elif typ == TypeCode.LONG8:
            return self.validateValue(v, noneOk, self.__buffer.getLong())
        
        elif typ == TypeCode.FLOAT4:
            return self.validateValue(v, noneOk, self.__buffer.getFloat())
        
        elif typ == TypeCode.FLOAT8:
            return self.validateValue(v, noneOk, self.__buffer.getDouble())
        
        elif typ == TypeCode.BYTES:
            return self.validateValue(v, noneOk, self.readBytes())
        
        elif typ == TypeCode.EMPTY_STRING:
            return self.validateValue(v, noneOk, "")
        
        elif typ == TypeCode.STRING:
            return self.validateValue(v, noneOk, ByteArray.toString(self.readBytes(), self.valueFactory.getStringEncoding()))
        
        elif typ == TypeCode.ARRAY:
            return self.validateValue(v, noneOk, self.fromArrayValue(self.readArray(v)))
        
        elif typ == TypeCode.STRUCT:
            return self.validateValue(v, noneOk, self.readStruct())
        
        elif typ == TypeCode.CUSTOM:
            return self.validateValue(v, noneOk, self.valueFactory.importCustomValue(self.readStruct()))
        
        elif typ == TypeCode.NONE:
            return self.validateValue(v, noneOk, self.NONE)
        
        else:
            if typ & TypeCode.PSMASK == TypeCode.PSVALUE:
                # Positive small integers
                value = typ & TypeCode.PVMASK
                dt    = (typ >> TypeCode.PDTSHIFT) & TypeCode.DTMASK
                if dt == TypeCode.BYTE_DT:
                    return self.validateValue(v, noneOk, Byte(value))
                if dt == TypeCode.SHORT_DT:
                    return self.validateValue(v, noneOk, Short(value))
                if dt == TypeCode.INT_DT:
                    return self.validateValue(v, noneOk, value)
                return self.validateValue(v, noneOk, value)    
            if typ & TypeCode.NSMASK == TypeCode.NSVALUE:
                # Negative small integers
                value = -( typ & TypeCode.NVMASK) - 1
                dt    = (typ >> TypeCode.NDTSHIFT) & TypeCode.DTMASK
                if dt == TypeCode.BYTE_DT:
                    return self.validateValue(v, noneOk, Byte(value))
                if dt == TypeCode.SHORT_DT:
                    return self.validateValue(v, noneOk, Short(value))
                if dt == TypeCode.INT_DT:
                    return self.validateValue(v, noneOk, value)
                return self.validateValue(v, noneOk, value)    
            
            raise UnsupportedOperationException, "unsuppported type code %s" % typ

    def readBytes(self):
        n = self.readValue(self.intValidator)
        b = ByteArray(length=n)
        self.__buffer.getFully(b)
        return b
    
    @staticmethod
    def readMessageStatic(vf, buf, offset=0, length=None):
        if isinstance(buf, ByteArray):
            buf = FlexBuffer(buf, offset, length)
        
        tdi = BinaryTaggedDataInput(vf, buf)
        return tdi.readMessage()

