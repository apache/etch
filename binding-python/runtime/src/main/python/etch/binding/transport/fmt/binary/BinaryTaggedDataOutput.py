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
from ...TaggedDataOutput import *
from .....util.Flexbuffer import *
from ....support.Validator_int import *
from .Validator_none import *

class BinaryTaggedDataOutput(BinaryTaggedData,TaggedDataOutput):

    def __init__(self, buffer=None, valueFactory=None, uri=None):
        super(BinaryTaggedDataOutput,self).__init__(valueFactory)
        if buffer:
            assert isinstance(buffer,FlexBuffer), "buffer is not type 'FlexBuffer'"
        self.__buffer = buffer
        # nothing for uri

        self.intValidator = Validator_int.get(0)
        self.noneValidator = Validator_none.get()

    # TaggedDataOutput Methods

    def setBuffer(self, buffer):
        assert isinstance(buffer, FlexBuffer), "buffer is not type 'FlexBuffer'"

        self.__buffer = buffer

    def writeMessage(self, message):
        self.startMessage(message)
        self.writeKeysAndValues(message)
        self.endMessage(message)

    def writeStruct(self, sv):
        self.startStruct(sv)
        self.writeKeysAndValues(sv)
        self.endStruct(sv)

    def writeArray(self, av, validator):
        self.startArray(av)
        self.writeValues(av, validator)
        self.endArray(av)

    def writeKeysAndValues(self, sv):
        t = sv.type()
        for me in sv:
            f = me.getKey()
            self.writeValue(self.intValidator, f.getId())
            self.writeValue(t.getValidator(f), me.getValue())

    def writeValues(self, av, validator):
        ev = validator.elementValidator()
        for value in av:
            self.writevalue(ev, value)

    # Main output methods

    def startMessage(self, msg):
        self.__buffer.putByte(BinaryTaggedData.VERSION)
        self.writeType(msg.type())

    def endMessage(self, msg):
        self.writeValue(self.noneValidator, BinaryTaggedData.NONE)

    def startStruct(self, struc):
        self.writeType(struc.type())

    def endStruct(self, struc):
        self.writeValue(self.noneValidator, BinaryTaggedData.NONE)

    def startArray(self, ar):
        typ = ar.typeCode()
        self.__buffer.putByte(typ)
        if typ == TypeCode.CUSTOM:
            self.writeType(ar.customStructType())
        self.writeValue(self.intValidator, ar.dim())

    def endArray(self, ar):
        self.writeValue(self.noneValidator, BinaryTaggedData.NONE)

    def writeType(self, typ):
        self.writeValue(self.intValidator, typ.getId())


    # Local Utility Methods

    def writeValue(self, v, value):
        typ = self.checkValue(value, v)

        if typ == TypeCode.ANY:
            typ = self.checkValue(value)

        # disabled -- not needed except during development
        if True:
            if not v:
                raise IOException, "value %s (class %s) has no validator" % (repr(value), repr(value.__class__))
            if v and not v.validate(value):
                raise IllegalArgumentException, "value %s (class %s) not valid for validator %s" % (repr(value),repr(value.__class__),repr(v))
            oldTyp = self.checkValue(value)
            if typ != oldTyp:
                raise IOException, "value %s (class %s) typecode %d, old typecode %d" % (repr(value), repr(value.__class__), typ, oldTyp)

        self.__buffer.putByte(typ)

        if   typ in (TypeCode.NULL, TypeCode.BOOLEAN_FALSE, TypeCode.BOOLEAN_TRUE, TypeCode.EMPTY_STRING, TypeCode.NONE):
            return
        elif typ in (TypeCode.BYTE1, TypeCode.SHORT1, TypeCode.INT1, TypeCode.LONG1):
            self.__buffer.putByte(Byte(value))
            return
        elif typ in (TypeCode.SHORT2, TypeCode.INT2, TypeCode.LONG2):
            self.__buffer.putShort(Short(value))
            return
        elif typ in (TypeCode.INT4, TypeCode.LONG4):
            self.__buffer.putInt(Integer(value))
            return
        elif typ in (TypeCode.LONG8,):
            self.__buffer.putLong(Long(value))
            return
        elif typ in (TypeCode.FLOAT4,):
            self.__buffer.putFloat(Float(value))
            return
        elif typ in (TypeCode.FLOAT8,):
            self.__buffer.putDouble(Double(value))
            return
        elif typ in (TypeCode.BYTES,):
            self.writeBytes(ByteArray(value))
            return
        elif typ in (TypeCode.STRING,):
            self.writeBytes(String(value).getBytes(self.__vf.getStringEncoding()))
            return
        elif typ in (TypeCode.STRUCT,):
            self.writeStruct(value)
            return
        elif typ in (TypeCode.ARRAY,):
            self.writeArray(self.toArrayValue(value), v)
            return
        elif typ in (TypeCode.CUSTOM,):
            struc = self.__vf.exportCustomValue(value)
            if not struc:
                raise UnsupportedOperationException, "unsupported type %s" % repr(value.__class__)

            self.writeStruct(struc)
            return
        else:
            return

    def writeBytes(self, value):
        assert isinstance(value, ByteArray), "value is not ByteArray"
        n = value.length
        self.writeValue(self.intValidator, n)
        self.__buffer.put(value, 0, n)

    @staticmethod
    def getBytes(self, msg, vf):
        buf = FlexBuffer()
        tdo = BinaryTaggedDataOutput(buf, vf)
        tdo.writeMessage(msg)
        return buf.getAvailBytes()

