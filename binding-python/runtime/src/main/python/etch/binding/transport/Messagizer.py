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
from __future__ import with_statement
from .MessagizerMessageSource import *
from .FormatFactory import *
from ..msg.Message import *
from ..msg.ValueFactory import *
from ...util.FlexBuffer import *
from ...util.Resources import *
from ...util.URL import *
from ...util.core.Who import *
from ...util.core.io.PacketHandler import *
from ...util.core.io.PacketSource import *
from ...python.Exceptions import *
from ...python.SynchronizeOn import *

class Messagizer(PacketHandler,MessagizerMessageSource):
    """
    Messagizer is a PacketHandler which accepts packets from a PacketSource and
    turns them into messages for delivery to a MessageHandler. Messagizer is
    also a MessageSource, accepting messages and turning them into packets for
    delivery to a PacketSource.
    """
    
    FORMAT = "Messagizer.format"
    """
    Name of format name in uri or resources. The value is "Messagizer.format".
    The result of looking up this name should be a String.
    
    @see #Messagizer(String, Resources)
    """
    
    VALUE_FACTORY = "Messagizer.valueFactory"
    """
    Name of value factory in resources. The value is "Messagizer.valueFactory".
    The result of looking up this name should be a {@link ValueFactory} fo the
    service.
    
    @see #Messagizer(String, Resources)
    """
    
    def __init__(self, uri, resources):
        """
        Constructs the Messagizer with NULL handler and tagged data format
        constructed from the format factory.
        
        @param uri - the uri being used to configure the transport stack
        @param resources - the associated set of resources for this service
        """
        # Parse 'uri' string
        uri = URL(uri)
        
        # Find the format
        format = uri.getTerm(Messagizer.FORMAT)
        if format == None:
            format = resources.get(Messagizer.FORMAT)
            if format == None:
                raise IllegalArgumentException, "Format name is not defined as '%s' in uri or resources" % Messagizer.FORMAT
        
        # Find the format factory
        ff = FormatFactory.get(format)
        if ff == None:
            raise IllegalArgumentException, "Format factory is not defined as '%s' in format factories" % format
        
        # Find the value factory
        vf = resources.get(Messagizer.VALUE_FACTORY)
        if vf == None:
            raise IllegalArgumentException, "Value factory is not defined as '%s' in resources" % Messsagizer.VALUE_FACTORY
        
        # Construct the message formatters
        self.__tdi = ff.newTaggedDataInput(vf, repr(uri))
        self.__tdo = ff.newTaggedDataOutput(vf, repr(uri))
        
        # Other inits
        self.__src     = None
        self.__msgBuf  = FlexBuffer()
        self.__handler = None
        
    def __repr__(self):
        return "Messagizer/%s" % self.__src
    
    def packet(self, sender, buf):
        # messagize the packet
        msg = self.__tdi.readMessage(buf)
        # Log.report("Messagizer.packet", "who", self, "recv", msg)
        self.__handler.message(sender, msg)
        
    ## Message Source
    
    def messagex(self, recipient, msg):
        
        # packetize the message
        with SynchronizeOn(self.__msgBuf):
            try:
                # Log.report("Messagizer.packet", "who", self, "send", msg)
                # assert that msgBuf is reset
                # leave space for packet header
                self.__msgBuf.skip(self.__src.headerSize(), True)
                self.__tdo.writeMessage(msg, self.__msgBuf)
                self.__msgBuf.setIndex(0)
                self.__src.packet(recipient, self.__msgBuf)
            finally:
                self.__msgBuf.reset()
        
    ## SourceHandler methods
    
    def getSource(self):
        return self.__src
        
    def setSource(self, src):
        self.__src = src
    
    def sessionQuery(self, query):
        return self.__handler.sessionQuery(query)
    
    def sessionControl(self, control, value):
        self.__handler.sessionControl(control, value)
    
    def sessionNotify(self, event):
        self.__handler.sessionNotify(event)
    
    ## Source methods
    
    def getHandler(self):
        return self.__handler
        
    def setHandler(self, handler):
        self.__handler = handler
        self.__handler.setSource(self)
    
    def transportQuery(self, query):
        return self.__src.transportQuery(query)
    
    def transportControl(self, control, value):
        self.__src.transportControl(control, value)
    
    def transportNotify(self, event):
        self.__src.transportNotify(event)
    
