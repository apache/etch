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
from .Pool import *
from ...util.TodoManager import *
from ...util.Todo import *

__all__ = ['QueuedPool']

class _Todo(Todo):
    
    def __init__(self, runnable):
        super(_Todo,self).__init__()
        self.__runnable = runnable
    
    def doit(self, mgr):
        self.__runnable.run()
    
    def exception(self, mgr, e):
        self.__runnable.exception(e)
    

class QueuedPool(Pool):
    """
    A queued implementation of pool
    """
    
    def __init__(self, mgr=None):
        """
        Constructs the queued pool with a specified todo manager.
        
        @param mgr - if mgr is not specified or None, use the default manager
        @raises Exception
        """
        if mgr == None:
            mgr = TodoManager.getTodoManager()
            
        self.__mgr = mgr
    
    def run(self, runnable):
        self.__mgr.add(_Todo(runnable))
        
