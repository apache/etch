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
from ....python.Exceptions import *
from .Typecode import TypeCode
from ..ArrayValue import *

class TaggedData(object):
    """
    Common interface for various types of tagged data input and output
    """

    def __init__(self, valueFactory):
        """
        Constructs the TaggedData
        """
        self.vf = valueFactory

    def getValueFactory(self):
        """
        @return the value factory to use for tagged input and output
        """
        return self.vf

    def toArrayValue(self, value):
        """
        Convert value to ArrayValue

        @param value
        @return         An ArrayValue which models the given array
        """

        c = value

        dim = 0
        while isinstance(c, (types.TupleType, types.SetType, types.ListType)):
            dim += 1
            if len(c) > 0:
                c = [0]
            else:
                c = None
                break

        c = c.__class__
        # now we want the type code for 'c', and if the type code is custom,
        # we will also want the struct type code

        typeCode = self.getNativeTypeCode(c)

        # TODO - customStructType
        customStructType = None
        if typeCode == TypeCode.CUSTOM:
            customStructType = self.getCustomStructType(c)
        else:
            customStructType = None

        return ArrayValue(value, typeCode, customStructType, dim)

    def fromArrayValue(self, arrayValue):
        """
        @param arrayValue
        @return             a native array converted from an array value
        """
        return arrayValue.getArray()

    def allocArrayValue(self, typeCode, customStructType, dim, length):
        """
        Construct an array prepopulated with 'length' instance's of a class

        @param typeCode
        @param customStructType
        @param dim
        @param length
        """
        clss = self.getComponentType(typeCode, customStructType, dim-1)
        if clss == None:
            raise IllegalArgumentException

        return [clss() for i in range(0,length)]

    def getComponentType(self, typeCode, customStructType, dim):
        """
        getComponentType
        """
        if typeCode == TypeCode.CUSTOM:
            c = self.customStructType.getClss()
        else:
            c = self.getNativeType(typeCode)


        if c == None:
            return None

        if dim == 0:
            return c

        dims = [int() for i in range(0,dim)]
        # TODO - what are the dimensions of this array?
        # TODO - what is the returned class supposed to be?
        # TODO - lookup java.lang.reflect.Array
        # 		int[] dims = new int[dim];
        #		Object o = Array.newInstance( c, dims );
        #//		System.out.printf( "object = %s\n", o );
        #		c = o.getClass();
        #//		System.out.printf( "class = %s\n", c );
        #		return c;
        o = [c(), dims]
        c = o.getClass()
        return c

    def checkValue(self, value):
        """
        @param value
        @return         return a typecode for the specified value
        """
        raise AbstractMethodException

    def getNativeTypeCode(self, c):
        """
        Returns the type code for the specified class. This is needed when we
        have an array and we have to determine the base type and now we're
        fixing to serialize it.

        @param c
        @return     a type code
        """
        raise AbstractMethodException

    def getCustomStructType(self, c):
        """
        @param c
        @return     a struct type for the specified custom class
        """
        raise AbstractMethodException

    def getNativeType(self, typeCode):
        """
        @param typeCode
        @return         the class for a specified (native) type code
        """
        raise AbstractMethodException

