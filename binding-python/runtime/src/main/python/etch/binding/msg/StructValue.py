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
from .Type import *
from .Validator import *

class StructValue(dict):
    """
    A typed Dict, where the type is an etch.util.Type, each key is an etch.msg.Field,
    and each value of arbitrary type chosen from the basic python types:
    boolean, byte, short, int, long, float, double, string, and array, the
    extended types and specific types supported by etch.msg.ValueFactory.
    """

    serialVersionUID = 1
    """Bogus serial version uid"""

    DEFAULT_SIZE = 8
    """magic number 6 results in super(8) in dict constructor"""

    def __init__(self, typ, vf, length=0):
        """
        Constructs the StructValue

        @param structType   The type of the struct (or the action or event
                            if this is a message)
        @param vf           the value factory
        @param length       The expected size of the dict (ignored)
        """
        if not typ:
            raise IllegalArgumentException, "typ == None"

        if not isinstance(typ, Type):
            raise IllegalArgumentException, "typ must be of type %s" % repr(Type)

        self.__type  = typ
        self.__level = vf.getLevel()

    def isEmpty(self):
        return self.size() == 0

    def size(self):
        """
        Alias for len(self)
        """
        return super(StructValue,self).__len__()
    
    def level(self):
        """
        @return validation level
        """
        return self.__level
        
    def type(self):
        """
        Return the struct type.
        """
        return self.__type

    def isType(self, otherType):
        """
        Compares the type of this struct to another type.

        @param otherType    The type to compare this type to.
        @return             True if this struct is of the specified type.
        """
        if not isinstance(otherType, Type):
            return False

        return self.__type == otherType

    def checkType(self, expectedType):
        """
        Checks a struct for having the expected type.

        @param expectedType     The expected type of this struct.
        @throws IllegalArgumentException    If the type is not as expected.
        """
        if not self.isType(expectedType):
            raise IllegalArgumentException, "Type == %s, but expected %s" % (repr(self.__type), repr(expectedType))

    def put(self, key, value):
        """
        Alias for X[key] = value
        """
        super(StructValue,self).__setitem__(key, value)

    def remove(self, key):
        """
        Alias for del X[key], ignores exception if 'key' does not exist
        """
        try:
            super(StructValue,self).__delitem__(key)
        except:
            pass
    
        
    def __setitem__(self, key, value):
        """
        Set item, if 'value' is None then delete the item from the Map
        """
        if value is None:
            super(StructValue,self).__delitem__(key)

        # TODO - disable validate on put?
        if self.__level != Validator.NONE:
            v = self.__type.getValidator(key)
            
            if self.__level == Validator.FULL and v is None:
                raise IllegalArgumentException, "validator missing for type %s field %s" % (repr(self.__type), key)
                
            if v is not None or not v.validate(value):
                raise IllegalArgumentException, "Validator %s failed for type %s field %s: value %s" % (repr(v), repr(self.__type), repr(key), repr(value))

        super(StructValue,self).__setitem__(key, value)

    def __repr__(self):
        return "%s: %s" % (repr(self.__type), super(StructValue,self).__repr__())

