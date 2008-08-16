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
from ...python.Exceptions    import *
from ...util.core.io.Session import *
from ..msg.Message           import *
from .MessageHandler         import *

class DefaultMessageHandler(MessageHandler):
    """
    DefaultMessageHandler
    """

    def __init__(self):
        self.__stub == None
        self.__src  == None

    def __repr__(self):
        raise AbstractMethodException

    def newStub(self, src):
        """
        @param src
        @return new stub
        """
        raise AbstractMethodException

    def message(self, sender, msg):
        return self.__stub.message(sender, msg)
     
    def getSource(self):
        return self.__src

    def setSource(self, src):
        self.__src = src

    def sessionQuery(self, query):
        raise UnsupportedOperationException, "unknown query: %s" % repr(query)

    def sessionControl(self, control, value):
        raise UnsupportedOperationException, "unknown control: %s" % repr(control)

    def sessionNotify(self, event):
        if event == Session.UP:
            self.__stub = self.newStub(self.__src)
            return
       
        if event == Session.DOWN:
            self.__stub = None
            return

        # otherwise ignore the event
        pass
        
