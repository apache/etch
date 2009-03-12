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
from .Validator import *

class ValueFactory(object):
    """
    Interface which defines the value factory which helps the NSDL compiler
    serialize and deserialize messages, convert values, etc.
    """

    ## Type Methods

    def getType(self, key):
        """
        Translates a type id into the appropriate Etch.Msg.Type
        
        @param key      A type 'id' or a type 'name'
        @return         KEY translated into the appropriate type
        """
        raise UndefinedInterfaceMethodException

    def addType(self, typ):
        """
        Adds the type if it doesn't already exist. Use this to dynamically add types to
        a ValueFactory. The type is per instance of the ValueFactory, not global. Not available
        if dynamic typing is locked.
        
        @param typ
        """
        raise UndefinedInterfaceMethodException
        
    def lockDynamicTypes(self):
        """
        Locks the dynamic typing so that no new types may be created by addType or getType
        """
        raise UndefinedInterfaceMethodException
    
    def unlockDynamicTypes(self):
        """
        Unlocks the dynamic typing so that new types may be created by addType or getType.
        """
        raise UndefinedInterfaceMethodException

    def getTypes(self):
        """
        @return   a collection of all the types
        """
        raise UndefinedInterfaceMethodException

    ## String Encoding

    def getStringEncoding(self):
        """
        @return   the encoding to use for strings
        """
        raise UndefinedInterfaceMethodException

    ## Message ID

    def getMessageId(self, msg):
        """
        @param msg      The message whose well-known message-id field is to be
                        returned
        @return         The value of the well-known message-id field. This is
                        a unique identifier for this message on a particular
                        transport during a particular session. If there is no
                        well-known message-isd field defined, or if the
                        message-id field has not been set, then return None
        """
        raise UndefinedInterfaceMethodException

    def setMessageId(self, msg, msgid):
        """
        Sets the value of the well-known message-id
        
        @param msg        the message whose well-known message-id is to be set
        @param msgid      the value of the well-known message-id field
        """
        raise UndefinedInterfaceMethodException

    def get_mf__messageId(self):
        """
        @return well-known message field for message id
        """
        raise UndefinedInterfaceMethodException
        
    ## IN REPLY TO

    def getInReplyTo(self, msg):
        """
        @param msg        The message whose well-known in-reply-to field is to be returned
        @return           hte value o the in-reply-to field
        """
        raise UndefinedInterfaceMethodException

    def setInReplyTo(self, msg, msgid):
        """
        @param msg        The message wholes well-known in-reply-to field is to be set
        @param msgid      The valud ot the well-known in-reply-to field        
        """
        raise UndefinedInterfaceMethodException


    def get_mf__inReplyTo(self):
        """
        @return well-known message field for in reply to
        """
        raise UndefinedInterfaceMethodException

    ## Value Conversion

    def exportCustomValue(self, value):
        """
        Converts a value to a struct value representation to be exported to a tagged data output
        
        @param value      A custom type defined by a service, or a well-known standard type
        @return           A struct value representing the value
        @throws           UnsupportedOperationException if the type cannot be exported
        """
        raise UnsupportedOperationException

    def importCustomValue(self, structValue):
        """
        Converts a struct value imported from a tagged data input to a normal type
        
        @param structValue A Struct value representation
        @return            A custom type
        @throws            UnsupportedOpertationException if the type cannot be imported
        """
        raise UnsupportedOperationException

    def getCustomStructType(self, cls):
        """
        @param c          The class of a custom value
        @return           the Struct type of a custom value class
        @throws           UnsupportedOperationException
        """
        raise UnsupportedOperationException

    def get_mt__exception(self):
        """
        @return a well-known message type for exception thrown by one-way message.
        """
        raise UndefinedInterfaceMethodException

    def getLevel(self):
        """
        @return the validation level of field StructValue.put and TaggedDataOutput
        """
        raise UndefinedInterfaceMethodException
    
    def setLevel(self, level):
        """
        Sets the validation level of field StructValue.put and TaggedDataOutput
        
        @param level
        @return the old value
        """
        raise UndefinedInterfaceMethodException
