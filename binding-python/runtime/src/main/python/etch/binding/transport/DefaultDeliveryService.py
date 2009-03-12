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
from ..msg.Message import *
from ..msg.Type import *
from ..support.DeliveryService import *
from ..support.Mailbox import *
from ...util.Monitor import *
from ...util.Resources import *
from ...util.TimeoutException import *
from ...util.URL import *
from ...util.core.Who import *

 
class DefaultDeliveryService(DeliveryService):
    """
    Default implementation of DeliveryService
    """
    
    DISABLE_TIMEOUT = "DefaultDeliveryService.disableTimeout"
    """Parameter to Globally Disable Timeout."""
    
    def __init__(self, transport, uri, resources):
        """
        @param transport
        @param uri
        @param resources
        """
        self.__transport = transport
        self.__session = None
        self.__status  = Monitor("session status")
        u = URL(uri)
        self.__transport.setSession(self)
        self.__disableTimeout = u.getBooleanTerm(self.DISABLE_TIMEOUT, False)

    def getTransport(self):
        """
        @return the transport
        """
        return self.__transport
    
    def getSession(self):
        return self.__session
        
    def sessionQuery(self, query):
        return self.__session.sessionQuery(query)
    
    def sessionControl(self, control, value):
        self.__sessionControl(control, value)
    
    def sessionNotify(self, event):
        if event == self.UP:
            self.__status.set(self.UP)
        elif event == self.DOWN:
            self.__status.set(self.DOWN)
        
        self.__session.sessionNotify(event)
    
    def sessionMessage(self, sender, msg):
        return self.__session.sessionMessage(sender, msg)
    
    def __repr__(self):
        return self.__transport.__repr__()
    
    def waitUp(self, maxDelay):
        self.__status.waitUntilEq(self.UP, maxDelay)
    
    def waitDown(self, maxDelay):
        self.__status.waitUntilEq(self.DOWN, maxDelay)
    
    def transportMessage(self, recipient, msg):
        self.__transport.transportMessage(recipient, msg)
    
    def transportQuery(self, query):
        #TODO - where are WaitUp and WaitDown defined?
        if query.__class__ == WaitUp:
            self.waitUp(query.maxDelay)
            return None
        elif query.__class__ == WaitDown:
            self.waitDown(query.maxDelay)
            return None
        else:
            return self.__transport.transportQuery(query)
    
    def transportControl(self, control, value):
        if control == self.START_AND_WAIT_UP:
            self.__transport.transportControl(self.START, None)
            self.waitUp(value)
        elif control == self.STOP_AND_WAIT_DOWN:
            self.__transport.transportControl(self.STOP, None)
            self.waitDown(value)
        else:
            self.__transport.transportControl(control, value)
    
    def transportNotify(self, event):
        self.__transport.transportNotify(event)

    def beginCall(self, msg):
        try:
            return self.__transport.transportCall(None, msg)
        except:
            # TODO - more detail in raise?
            raise Exception, "unexpected exception sending message"
    
    def endCall(self, mb, responseType):
        try:
            timeout = 0
            if not self.__disableTimeout:
                timeout = responseType.getTimeout()
            
            mbe = mb.read(timeout)
            if mbe == None:
                raise TimeoutException, "timeout waiting for %s" % repr(responseType)
            
            rmsg = mbe.msg
            rmsg.checkType(responseType)
            r = rmsg.get(responseType.getResponseField())
            if isinstance(r, Exception):
                raise r
            return r
        finally:
            mb.closeDelivery()
    
    
    
