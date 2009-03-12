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

class ThreadGroup:
    pass

class Thread:
    pass
    

class _Runnable(object):
    def __init__(self, poolRunnable):
        self.__poolRunnable = poolRunnable
    
    def run(self):
        try:
            self.__poolRunnable.run()
        except Exception, e:
            self.__poolRunnable.exception(e)

class FreePool(Pool):
    """
    A free implementation of Pool.
    """
    
    def __init__(self, maxSize=50):
        """
        Constructs a FreePool with specified maxSize.
        
        @param maxSize - maximum number of free threads at one time, after
        maximum is reached, reject the requests. Default size is 50.
        """
        self.__maxSize = maxSize
        self.__tg      = ThreadGroup("FreePoolThreadGroup")
        self.__open    = True
    
    def close(self):
        """
        Closes the pool. This just marks the pool as being closed, it doesn't
        actually do anything to the currently running thread. But no more 
        threads are allowed to start.
        """
        self.__open = False
    
    def join(self):
        """
        Joins each of the threads in this pool until there are none left. The
        pool will be closed first.
        
        @raises Exception
        """
        self.close()
        lst = [None for x in range(0,10)]
        n = self.__tg.enumerate(lst)
        while n > 0:
            for i in range(0,n):
                lst[i].join()
            n = self.__tg.enumerate(lst)
    
    def run(self, poolRunnable):
        # TODO: Whole method Sychronized?
        
        if not self.__open or self.__tg.activeCount() >= self.__maxSize:
            raise IllegalStateException, "free pool thread count exceeded or pool closed"
        
        Thread(self.__tg, _Runnable(poolRunnable)).start()
        
