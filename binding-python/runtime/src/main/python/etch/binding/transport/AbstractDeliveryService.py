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
from ...python.Exceptions import *
from ...util.core.Who import *
from ..msg.Message import *
from ..msg.Type import *
from ..support.DeliveryService import *
from ..support.Mailbox import *
from ..support.StubIntf import *

class AbstractDeliveryService(DeliveryService,MessageHandler):
    """
    Abstraction of DeliveryService functionality
    """

    def __init__(self):
        super(DeliveryService,self).__init__()
        self.__src = None
        self.__stub = None

    def getSource(self):
        return self.__src
    
    def setSource(self, src):
        self.__src = src

    def addStub(self, stb):
        """
        Adds a stub to the set of stubs.
        @param stb
        """
        if self.__stub != None:
            raise UnsupportedOperationException, "Only one stub allowed for now"

        self.__stub = stb

    def removeStub(self):
        """
        Removes all the stubs in the set of stubs.
        """
        self.__stub = None
   
    def sessionQuery(self, query):
        if self.__stub != None:
            return self.__stub.sessionQuery(query)
        raise UnsupportedOperationsException, "Unknown query %s" % query
    
    def sessionControl(self, control, value):
        if self.__stub != None:
            self.__stub.sessionControl(control, value)
        raise UnsupportedOperationException, "Unknown control %s" % query
    
    def sessionNotify(self, event):
        if self.__stub != None:
            self.__stub.sessionNotify(event)
        elif isinstance(event, Exception) or issubclass(event, Exception):
            # TODO: raise the event or just dump the stacktrace?
            # Java is: ((Throwable) event).printStackTrace()
            pass

    # TODO: is this 'queueMessage' or ??
    def message(self, sender, msg):
        return self.__stub.message(sender, msg)
    
    def transportQuery(self, query):
        return self.__src.transportQuery(query)
    
    def transportControl(self, control, value):
        self.__src.transportControl(control, value)
    
    def transportNotify(self, event):
        self.__src.transportNotify(event)
    
    def send(self, msg):
        self.__src.messagex(None, msg)
    
    def begincall(self, msg):
        try:
            # TODO: change to correct 'message' method name
            return self.__src.message(None, msg, 1, 0)
        except Exception, e:
            raise RuntimeException, "Unexpected exception sending message: %s" % e
    
    def endcall(self, mb, responseType):
        # TODO: rewrite with 'with'?
        try:
            mbe = mb.read(responseType.getTimeout())
            if mbe == None:
                raise TimeoutException, "timeout waiting for %s" % repr(responseType)
            rmsg = mbe.msg
            rmsg.checkType(responseType)
            r = rmsg.get(responseType.getResponseField(), None)
            if isinstance(r, Exception):
                raise Exception, repr(r.args) 
            
            # TODO: lookup if finally is called in java in this case
            return r
        finally:
            mb.closeDelivery()


