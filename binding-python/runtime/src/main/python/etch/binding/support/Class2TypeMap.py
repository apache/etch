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
from ..msg.Type import *
from ...python.Exceptions import *

class Class2TypeMap(object):
    """
    Mapping between Class and Type
    """
    
    def __init__(self):
        self.__c2t = {}
        self.__locked = False

    def __getitem__(self, c):
        return self.__c2t[c]
    
    def get(self, c):
        try:
            return self.__getitem__(c)
        except:
            return None

    def __setitem__(self, c, t):
        if self.__locked:
            raise IllegalStateException, "locked"
        
        x = self.get(c)
        if x != None:
            if x != t:
                raise IllegalArgumentException, "type %s: class %s is already mapped to type %s" % (t,c,x)
            # x == t, ergo our work is finished
            return
        
        self.__c2t[c] = t
    
    def __iter__(self):
        return self.__c2t.__iter__()
        
    def put(self, c, t):
        self.__setitem__(c,t)
        
    def lock(self):
        self.__locked = True
    
    def addAll(self, other):
        for c in other:
            self.put(c,other[c])
