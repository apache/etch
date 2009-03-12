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

__all__ = ['PoolRunnable', 'Pool']

class PoolRunnable(object):
    """
    A runnable which can throw an exception
    """
    
    def run(self):
        """
        @raises Exception
        """
        raise UnimplementedInterfaceMethodException
    
    def exception(self, e):
        """
        Reports an exception raised by the run method
        
        @param e
        """
        raise UnimplementedInterfaceMethodException

class Pool(object):
    """
    Interface to a queued or free thread pool.
    """
    
    def run(self, runnable):
        """
        @param runnable - the thing to run
        @raises Exception - if there is a problem scheduling the runnable to run
        """
        raise UnimplementedInterfaceMethodException
        
