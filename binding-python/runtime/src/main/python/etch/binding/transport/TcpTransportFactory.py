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
from .TransportFactory import *
from .MailboxManager import *
from .Messagizer import *
from .DefaultDeliveryService import *
from ..support.DeliveryService import *
from ...util.Resources import *
from ...util.core.io.Listener import *
from ...util.core.io.ListenerHandler import *
from ...util.core.io.Packetizer import *
from ...util.core.io.Session import *
from ...util.core.io.TcpConnection import *
from ...util.core.io.Transport import *
from ...python.Exceptions import *

__all__ = ['TcpTransportFactory']

class _ListenerHandler(ListenerHandler):
    
    def __init__(self, transportFactory, uri, resources, serverFactory):
        super(_ListenerHandler,self).__init__()
        self.__transportFactory = transportFactory
        self.__uri = uri
        self.__resources = resources
        self.__serverFactory = serverFactory
    
        self.__src = None
        
    def accepted(self, s):
        r = Resources(self.__resources)
        r.put('SOCKET', s)
        
        d = self.__transportFactory.newTransport(self.__uri, r)
        self.__serverFactory.newServer(d)
        d.transportControl(Transport.START, None)
    
    def getSource(self):
        return self.__src
    
    def setSource(self, src):
        self.__src = src
        
    def sessionQuery(self, query):
        if isinstance(self.__serverFactory, Session):
            return self.__serverFactory.sessionQuery(query)
        else:
            raise UnsupportedOperationException, "unknown query %s" % query
    
    def sessionControl(self, control, value):
        if isinstance(self.__serverFactory, Session):
            self.__serverFactory.sessionControl(control, value)
        else:
            raise UnsupportedOperationException, "unknown control %s" % query
    
    def sessionNotify(self, event):
        if isinstance(self.__ServerFactory, Session):
            self.__serverFactory.sessionQuery(event)


class TcpTransportFactory(TransportFactory):
    """
    Transport factory for tcp
    """

    def newTransport(self, uri, resources):
        
        socket = resources.get("SOCKET")
        
        c = TcpConnection(uri, resources, socket)
        p = Packetizer(uri, resources)
        c.setHandler(p)
        m = Messagizer(uri, resources)
        p.setHandler(m)
        r = MailboxManager(uri, resources)
        m.setHandler(r)
        d = DefaultDeliveryService(uri, resources)
        r.setHandler(d)
        
        return d
    
    def newListener(self, uri, resources, factory):
        lh = _ListenerHandler()
        l  = Listener(uri)
        l.setHandler(lh)
        return l


        
