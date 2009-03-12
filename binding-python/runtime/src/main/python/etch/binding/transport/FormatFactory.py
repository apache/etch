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
from ..msg.ValueFactory import *
from ...python.Exceptions import *
from ...python.SynchronizedMap import *

__all__ = ['FormatFactory']

default_FormatFactories = {
    'binary' : 'etch.transport.fmt.binary.BinaryTaggedDataFormatFactory'
}

class FormatFactory(object):
    """
    A factory used to construct tagged data processors for messagizer
    """
    
    formatFactories = SynchronizedMap(default_FormatFactories)  # TODO: needs to be a synchronized Map

    def newTaggedDataInput(self, vf, uri):
        """
        @param vf
        @param uri
        @return the tagged data input with the specified value factory and 
        initialized by terms from the uri
        """
        raise AbstractMethodException
        
    def newTaggedDataOutput(self, vf, uri):
        """
        @param vf
        @param uri
        @return the tagged data output with the specified value factory and
        initialized by the terms of the uri
        """
        raise AbstractMethodException

    @classmethod
    def get(cls,name):
        """
        Gets the named format factory.
        
        @param name     the name of a configured format factory
        @return         the named format factory, or None if it isn't defined
        """
        factory = cls.formatFactories.get(name, None)
        if factory == None:
            return None
        
        factoryInstance = None
        if isinstance(factory, types.StringTypes):
            try:
                # TODO: module lookup ?
                exec("factoryInstance = %s()" % factory)
            except:
                raise IllegalArgumentException, "Could not create instance of format name %s: %s" % (name, sys.exc_info()[1])
        else:
            factoryInstance = factory
        
        if isinstance(factoryInstance, FormatFactory):
            return factoryInstance
        else:
            raise IllegalArgumentException, "instance for format name %s(%s) does not implement FormatFactory" % (name, repr(factory))    


    @classmethod
    def put(cls, name, factory):
        """
        Puts a named format factory into the dictionary.
        
        @param name     the name of the format factory (e.g. 'binary').
        @param factory  the name of the fully qualified class name or instance
        of the FormatFactory to associate with the name.
        """
        cls.formatFactories[name] = factory
        
