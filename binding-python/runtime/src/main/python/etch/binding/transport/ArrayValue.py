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
from ...python.Exceptions import *
from ..msg.Type import *

class ArrayValue(object):
    """
    An array of values, where each value is of arbitrary type
    chosen from the basic python/etch type: Boolean, Byte, Short,
    Integer, Long, Float, Double, String and array of those, the
    extended type ArrayValue and StructValue, and specific types
    supported by ValueFactory.
    
    ArrayValue is not protected against concurrent access.
    """

    def __init__(self, array, typeCode=0, customStructType=None, dim=0):
        """
        Constructs the ArrayValue
        
        @param array
        @param dim
        @param customStructType
        @param typeCode
        """
        if array == None:
            raise NullPointerException, "array == None"

        if not isinstance(array, [types.ListType, types.TupleType]):
            raise IllegalArgumentException, "array is not a list or tuple"

        self.__array = list(array)
        self.__typeCode = typeCode
        self.__customStructType = customStructType
        self.__dim = dim
        self.__addIndex = 0

    def typeCode(self):
        """
        @return the TypeCode for this array value
        """
        return self.__typeCode
    
    def customStructType(self):
        """
        @return a struct type if a custom type code
        """
        return self.__customStructType
    
    def dim(self):
        """
        @return the dimensionality of the array
        """
        return self.__dim
   
    def __iter__(self):
        return self.__array.__iter__()
    
    def size(self):
        """
        Alias for len(self)
        """
        return self.__len__()        

    def __len__(self):
        """
        @return the number of elements in the array
        """
        return len(self.__array)

    def __getitem__(self, idx):
        """
        @param idx
        @return the element at the specified index
        """
        return self.__array[idx]

    def compact(self):
        """
        Reallocates the array so that it is only as long as needed.
        """
        if self.__addIndex == self.size():
            return
            
        self.__array = self.__array[0:self.__addIndex]    

    def add(self, value):
        """
        Adds the value to the end of the array
        
        @param value
        """
        self.__array[self.__addIndex] = value
        self.__addIndex += 1
        
    def getArray(self):
        """
        @return the array value
        """
        return self.__array
