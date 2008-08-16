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
from .TcpTransportFactory import *
from ..support.DeliveryService import *
from ...util.Resouces import *
from ...util.URL import *
from ...util.core.io.Transport import *
from ...python.SynchronizedMap import *

__all__ = ['TransportFactory']

default_transportFactories = { 'tcp' : TcpTransportFactory() }

class TransportFactory(object):
    """
    Interface to transport factory
    """
    
    transportFactories = SynchronizedMap(default_transportFactories)
    
    def newTransport(self, uri, resources):
        """
        Constructs a new Transport stack topped by a DeliveryService which is
        used by the remote service implementation to send messages and make 
        calls.
        
        @param uri - transport configuration parameters
        @param resources - additional resources needed by the stack
        @return - the DeliveryService topping the transport stack
        @raises Exception
        """
        raise AbstractMethodException
    
    @classmethod
    def getTransport(cls, uri, resources):
        """
        Constructs a new Transport stack topped by a DeliveryService which is
        used by the remote service implementation to send messages and make
        calls.
        
        @param uri - transport configuration parameters
        @param resources - additional resources needed by the stack
        @return - the DeliveryService topping the transport stack
        @raises Exception
        """
        u = URL(uri)
        f = cls.get(u.getScheme)
        return f.newTransport(uri, resources)

    def newListener(self, uri, resources):
        """
        Constructs a new Transport Listener which is used to construct server
        sessions.
        
        @param uri - transport configuration parameters
        @param resources - additional resources needed by the listener
        @param factory - constructs the actual service class instances
        @return - an out-of-band source which may be used to control the listener
        @raises Exception
        """
        raise AbstractMethodException

    @classmethod
    def getListener(cls, uri, resources, factory):
        """
        Constructs a new Transport Listener which is used to construct server
        sessions.
        
        @param uri - transport configuration parameters
        @param resources - additional resources needed by the listener
        @param factory - constructs the actual service class instances
        @return - an out-of-band source which may be used to control the listener
        @raises Exception
        """
        u = URL(uri)
        f = cls.get(u.getScheme)
        return f.newListener(uri, resources, factory)

    @classmethod
    def get(cls, name):
        """
        Gets the named transport factory.
        
        @param name - the name of the configured transport factory
        @return - the named transport factory
        @raises IOException - if the transport factory is not defined
        """
        tf = cls.transportFactories.get(name, None)
        if tf == None:
            raise IOException, "Transport factory scheme %s is unknown" % name
        return tf
    
    @classmethod
    def put(cls, name, tf):
        """
        Puts a named transport factory.
        
        @param name - the uri scheme of this transport factory
        @param tf - the transport factory to associate with the name
        """
        cls.transportFactories[name] = tf

