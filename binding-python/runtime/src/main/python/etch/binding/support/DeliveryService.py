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
from ..msg.Field import *
from ..msg.Message import *
from ..msg.Type import *
from ...util.core.io.Transport import *

class DeliveryService(Transport):
    """
    Adapter between remote and message source.
    """
    
    def send(self, msg):
        """
        Sends the message to the recipient, but does not wait for any response.
        
        @param msg - the message to send
        @raises Exception - if there is a problem sending
        """
        raise UnimplementedInterfaceMethodException
        
    def begincall(self, msg):
        """
        Sends the message which begins a call sequence.
        
        @param msg - the message to send.
        @return - a mailbox which can be used to read response,
        using {@link #endcall(Mailbox, Type, Field, int)}
        """
        raise UnimplementedInterfaceMethodException
    
    def endcall(self, mb, responseType):
        """
        @param mb
        @param responseType - the type of the expected response.
        @param responseField - the field of the expected response which would
        contain any result value or raised exception
        @return - the value of the response field if it isn't an exception
        @raises Exception - if there is a problem sending or a timeout waiting
        or if the result value was an exception.
        """
        raise UnimplementedInterfaceMethodException
    
    def getSource(self):
        """
        @return - the message source of this delivery service
        """
        raise UnimplementedInterfaceMethodException
    
    def addStub(self, stub):
        """
        Adds a stub to receive messages from the message source.
        @param stub
        """
        raise UnimplementedInterfaceMethodException
    
    
