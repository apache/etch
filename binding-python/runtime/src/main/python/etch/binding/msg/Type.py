"""
$Id$

# Copyright 2007-2008 Cisco Systems Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy
# of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#
"""
from __future__ import absolute_import

import types
from .IdName                  import *
from ...python.Exceptions     import *
from ..support.FieldMap       import *
from ..support.ComboValidator import *
from ..support.AsyncMode      import *

class Type(IdName):
    """
    Type denotes the type of a struct or message. When used with a message it
    typically denotes an action or event.
    """

    runValidators   = True
    """Validators flag"""

    def __init__(self, *args, **kwargs):
        """
        Constructs the type

        @param type_id    The id of the type
        @param type_name  The name of the type
        """
        super(Type,self).__init__(*args, **kwargs)

        self.__rType           = None
        self.__helper          = None
        self.__clss            = None
        self.__locked          = False
        self.__timeout         = 0
        self.__responseField   = None
        self.__stubHelper      = None
        self.__asyncMode       = AsyncMode.NONE
        self.__validators      = {}    # TODO: self.__validators should be synchronized?
        self.__fields          = FieldMap()

    def getValidator(self, key):
        """
        @param key
        @return       The validator for this key of this type
        """

        if not self.__class__.runValidators:
            return None

        return self.__validators.get(key, None)

    @classmethod
    def getRunValidators(cls):
        """
        @return       The current setting of runValidators
        """
        return cls.runValidators

    @classmethod
    def setRunValidators(cls, value):
        """
        Enables or disables type validation for messages

        @param value
        """
        cls.runValidators = value

    def putValidator(self, key, vldtr):
        """
        Adds the validator to the chain for this key

        @param    key
        @param    vldtr
        """
        self.checkNotLocked()

        if vldtr == None:
            return;

        if self.__fields.get(key.getId()) == None:
            self.addField(key)

        v = self.__validators.get(key, None)
        if v != None:
            self.__validators[key] = ComboValidator(v, vldtr)
        else:
            self.__validators[key] = vldtr

    def clearValidator(self, key):
        """
        Removes the validator chain for this key

        @param key
        """
        self.checkNotLocked()
        self.__validators.remove(key)

    def getResult(self):
        """
        @return   The result message type of this message type
        """
        return self.__rType

    def setResult(self, rType):
        """
        Sets the result message type of this message type

        @param rType
        """
        self.checkNotLocked()
        self.__rType = rType

    def getImportExportHelper(self):
        """
        @return       The associated import/export helper
        """
        return self.__helper

    def setImportExportHelper(self, helper):
        """
        Sets the associated import/export helper

        @param helper
        """
        self.checkNotLocked()
        self.__helper = helper

    def getClss(self):
        """
        @return       The associated class
        """
        return self.__clss

    def setClss(self, clss):
        """
        Sets the associated class

        @param clss
        """
        self.checkNotLocked()
        self.__clss = clss

    def setComponentType(self, clss):
        """
        Sets the associated component type fro an array of this type.

        This type is not used when de-serializing the array component, just
        when allocating the array itself.

        @param clss
        """
        self.checkNotLocked()
        self.__clss = clss


    def getStubHelper(self):
        """
        @return       An object to help the stub dispatch the received message
        """
        return self.__stubHelper

    def setStubHelper(self, stubHelper):
        """
        Sets an object to help the stub dispatch the recieved message

        @param stubHelper
        """
        if self.__stubHelper != None:
            raise IllegalStateException, "stubHelper is already set"
        self.__stubHelper = stubHelper

    def lock(self):
        """
        Locks the fields for this type
        """
        self.__locked = True
        self.__fields.lock()

    def checkNotLocked(self):
        if self.__locked:
            raise IllegalStateException, "locked"

    def addField(self, field):
        """
        Adds a field to the set of fields

        @param field      A Field to add
        @return           The argument
        @throws           IllegalArguementException
        """
        return self.__fields.add(field)

    def getField(self, key):
        """
        Translates a field id or name into the appropriate Field object

        @param key    A Field id or name
        @return       Id translated into the appropriate Field
        """
        return self.__fields.get(key)

    def getFields(self):
        """
        @return       A set of all the fields
        """
        return set(self.__fields.values())

    def getTimeout(self):
        """
        @return     The time in milliseconds to wait for this response message
        """
        return self.__timeout
    
    def setTimeout(self, timeout):
        """
        @param timeout  the time in milliseconds to wait for this response message
        """
        if not isinstance(timeout, (types.LongType, types.IntType)):
            raise IllegalArgumentException, "timeout must be an integer"
        if timeout < 0:
            raise IllegalArgumentException, "timeout cannot be negative"
            
        self.__timeout = timeout
        
    def getResponseField(self):
        """
        @return the field with the value of this response.
        """
        return self.__responseField

    def setResponseField(self, responseField):
        """
        @param responseField    the field with the value of this response
        """
        self.__responseField = responseField
    
    def isAssignableFrom(self, otherType):
        """
        Checks whether this type is assignment compatible with otherType. This 
        means that the otherType is a subclass of this one

        @param otherType
        @return     True if this type is assignable from other
        """
        return other != None and (self == other or self.isAssignableFrom(otherType.superType()))

    def checkIsAssignableFrom(self, otherType):
        """
        Checks to see if this type is assignable from the other type.

        @param other
        @raises IllegalArgumentException if self is not assignable from otherType

        """
        if not self.isAssignableFrom(otherType):
            raise IllegalArgumentException "This is not a super type of other"
            
    def superType(self):
        """
        @return the super type of this type. If struct A extends B, then B is the super type of A
        """
        return self.__supertype

    def setSuperType(self, superType):
        """
        Sets the super type of this type. If struct A extends B, then B is the super type of A.
        @param superType
        """
        self.checkNotLocked()
        self.__superType = superType
    
    def getAsyncMode(self):
        """
        @return the AsyncMode for this type.
        """
        return self.__asyncMode
  
    def setAsyncMode(self, mode):
        """
        Sets the AsyncMode for this type.
        @param mode
        """
        self.__asyncMode = mode

    
