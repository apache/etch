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

import types
from .....python.Exceptions import *
from .....python.Types import *
from ..TypeCode import TypeCode
from ..TaggedData import *
from ...ArrayValue import *
from ....msg.StructValue import *

# TODO - make this singleton
class NoneObject(object):
    def __repr__(self):
        return 'NONE'

class BinaryTaggedData(TaggedData):
    """
    Common elements for binary tagged data input and output
    """
    NONE = NoneObject()
    """Sentinal which marks the end of a struct or array item list in a binary i/o stream."""

    VERSION = 1
    """This is the current version of the protocol"""

    def __init__(self, valueFactory):
        """
        Constructs the BinaryTaggedData
        """
        super(BinaryTaggedData,self).__init__(valueFactory)

    def adjSmallValue(self, typeCode, value):
        if typeCode in (TypeCode.BYTE1, TypeCode.SHORT1, TypeCode.INT1, TypeCode.LONG1):
            # value = value.byteValue
            v = Byte(value)
            if v >= TypeCode.MIN_SMALL_INT and v <= TypeCode.MAX_SMALL_INT:
                if v >= 0:
                    if typeCode == TypeCode.BYTE1:
                        return Byte(v + TypeCode.POSITIVE_BYTE_BASE)
                    elif typeCode == TypeCode.SHORT1:
                        return Byte(v + TypeCode.POSITIVE_SHORT_BASE)
                    elif typeCode == TypeCode.INT1:
                        return Byte(v + TypeCode.POSITIVE_INT_BASE)
                    else:
                        return Byte(v + TypeCode.POSITIVE_LONG_BASE)

                if typeCode == TypeCode.BYTE1:
                    return Byte(-(v+1) + TypeCode.NEGATIVE_BYTE_BASE)
                elif typeCode == TypeCode.SHORT1:
                    return Byte(-(v+1) + TypeCode.NEGATIVE_SHORT_BASE)
                elif typeCode == TypeCode.INT1:
                    return Byte(-(v+1) + TypeCode.NEGATIVE_INT_BASE)
                else:
                    return Byte(-(v+1) + TypeCode.NEGATIVE_LONG_BASE)

        return typeCode

    def checkValue(self, value, validator=None):

        if not isinstance(value,(int,long)) and not value:
            return TypeCode.NULL

        if value == BinaryTaggedData.NONE:
            return TypeCode.NONE

        if validator:
            typeCode = validator.checkValue(value)
            if not typeCode:
                raise UnsupportedOperationException, "typeCode is NULL"

            return self.adjSmallValue(typeCode, value)

        if isinstance(value, (types.IntType, types.LongType, types.FloatType)):
            if isinstance(value, Byte):
                return BinaryTaggedData.checkByte(value, TypeCode.BYTE1, TypeCode.BYTE_DT)
            if isinstance(value, Short):
                return BinaryTaggedData.checkShort(value, TypeCode.SHORT2, TypeCode.SHORT_DT)
            if isinstance(value, types.IntType):
                return BinaryTaggedData.checkInteger(value, TypeCode.INT4, TypeCode.INT_DT)
            if isinstance(value, types.LongType):
                return BinaryTaggedData.checkLong(value, TypeCode.LONG8, TypeCode.LONG_DT)
            if isinstance(value, types.FloatType):
                return TypeCode.FLOAT8
            return TypeCode.CUSTOM

        if isinstance(value, types.StringTypes):
            if len(value) == 0:
                return TypeCode.EMPTY_STRING
            return TypeCode.STRING

        if isinstance(value, types.BooleanType):
            if value:
                return TypeCode.BOOLEAN_TRUE
            else:
                return TypeCode.BOOLEAN_FALSE

        if isinstance(value, StructValue):
            return TypeCode.STRUCT

        if isinstance(value, ArrayValue):
            return TypeCode.ARRAY

        if isinstance(value, ByteArray):
            return TypeCode.BYTES

        if isinstance(value, (types.ListType, types.TupleType)):
            return TypeCode.ARRAY

        return TypeCode.CUSTOM

    def getNativeTypeCode(self, clss):
        classMap = {
            types.BooleanType   : TypeCode.BOOLEAN_TRUE,
            Byte                : TypeCode.BYTE1,
            Short               : TypeCode.SHORT2,
            Integer             : TypeCode.INT4,
            Long                : TypeCode.LONG8,
            Float               : TypeCode.FLOAT4,
            Double              : TypeCode.Double,
            types.IntType       : TypeCode.INT4,
            types.LongType      : TypeCode.LONG8,
            types.FloatType     : TypeCode.FLOAT8,
            types.StringType    : TypeCode.STRING,
            types.UnicodeType   : TypeCode.STRING,
            object.__class__    : TypeCode.ANY
        }

        return classMap.has(clss, TypeCode.CUSTOM)

    def getCustomStructType(self, clss):
        return self.valueFactory.getCustomStructType(clss)

    def getNativeType(self, typeCode):
        typeMap = {
            TypeCode.BOOLEAN_TRUE   : types.BooleanType,
            TypeCode.BOOLEAN_FALSE  : types.BooleanType,
            TypeCode.BYTE1          : Byte,
            TypeCode.SHORT1         : Byte,
            TypeCode.SHORT2         : Short,
            TypeCode.INT1           : Byte,
            TypeCode.INT2           : Short,
            TypeCode.INT4           : types.IntType,
            TypeCode.LONG1          : Byte,
            TypeCode.LONG2          : Short,
            TypeCode.LONG4          : types.IntType,
            TypeCode.LONG8          : types.LongType,
            TypeCode.FLOAT4         : Float,
            TypeCode.FLOAT8         : types.FloatType,
            TypeCode.EMPTY_STRING   : types.UnicodeString,
            TypeCode.STRING         : types.UnicodeString,
            TypeCode.ANY            : object
        }

        try:
            return typeMap[typeCode]
        except:
            raise IllegalArgumentException, "Unsupported native type %s" % typeCode


    @staticmethod
    def checkByte(v, is1, dt):
        v = Byte(v)
        if v >= TypeCode.MIN_SMALL_INT and v <= TypeCode.MAX_SMALL_INT:
            if v >= 0:
                return Byte(TypeCode.PSVALUE | (Byte(dt) << TypeCode.PDTSHIFT) | v)
            return Byte(TypeCode.NSVALUE | (Byte(dt) << TypeCode.NDTSHIFT) | -(v+1))
        return is1

    @staticmethod
    def checkShort(v, is2, dt):
        if v >= Byte.MIN_VALUE and v <= Byte.MAX_VALUE:
            return BinaryTaggedData.checkByte(v, is2+1, dt)
        return is2

    @staticmethod
    def checkInteger(v, is4, dt):
        if v >= Short.MIN_VALUE and v <= Short.MAX_VALUE:
            return BinaryTaggedData.checkShort(v, is4+1, dt)
        return is4

    @staticmethod
    def checkLong(v, is8, dt):
        if v >= Integer.MIN_VALUE and v <= Integer.MAX_VALUE:
            return BinaryTaggedData.checkInteger(v, is8+1, dt)
        return is8
