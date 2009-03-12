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
from .StructValue import *
from .ValueFactory import *

class Message(StructValue):
    """
    A message is modeled as a command and some arguments. The command is an
    arbitrary integer value, and the arguments are key/value pairs, where the
    key is an arbitrary integer value and the value is any one of the standard
    python objects, a StructValue, or any type that may be serialized by the
    ValueFactory.
    """

    serialVersionUID = 1
    """A bogus serial version ID"""

    def __init__(self, messageType, valueFactory, length=None):
        """
        Constructs the message

        @param messageType    The type of the message (command)
        @param valueFactory   The value factory
        @param length         Suggested length (ignored)
        """
        super(StructValue, self).__init__(messageType, valueFactory, length)
        if not isinstance(valueFactory, ValueFactory):
            raise IllegalArgumentException, "valueFactory must be of type msg.ValueFactory"
        self.__vf = valueFactory

    def vf(self):
        """
        Return the 'valueFactory'
        """
        return self.__vf

    def reply(self, rType = None):
        """
        Creates a message which is a reply to the current message. The current
        message's value factory is copied to the new message. The message-id
        of the current message (if any) is copied into the in-reply-to field
        of the new message.

        @param rType    The type of the reply.
        @return         A reply message.
        """
        if not rType:
            rType = self.type().getResult()

        rmsg = Message(rType, self.__vf)
        rmsg.setInReplyTo( self.getMessageId() )
        return rmsg

    def getMessageId(self):
        """
        Return the connection specific unique identifier of this message.
        """
        return self.__vf.getMessageId(self)

    def setMessageId(self, msgid):
        """
        Sets the message-id field of this message

        @param msgid    The connection specific unique identifier.
        """
        self.__vf.setMessageId(self, msgid)

    def getInReplyTo(self):
        """
        Return the message-id of the message that this is a response to.
        """
        return self.__vf.getInReplyTo(self)

    def setInReplyTo(self, msgid):
        """
        Sets the in-reply-to field of this message.

        @param msgid    the message-id of the message that this is a response to.
        """
        self.__vf.setInReplyTo(self, msgid)

