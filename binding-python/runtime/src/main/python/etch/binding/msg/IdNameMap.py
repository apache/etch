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
import types

class IdNameMap(object):
    """Map by id and name of IdNames"""
    
    def __init__(self):
        
        self.__byId = {}
        self.__byName = {}
        self.__locked = False 
    
    def get(self, idx):
        if type(idx) in (types.IntType, types.LongType):
            return self.getById(idx)
        else:
            return self.getByName(idx)
    
    def getById(self, identifier):
        """
        Gets ths IdName subclass which corresponds to the specified id.
        
        @param identifier the id of an IdName
        @return the IdName subclass found
        """
        return self.__byId[identifer]
    
    def getByName(self, name):
        """
        Gets the IdName subclass which corresponds to the specified name,
        or creates it if it isn't found and if this map is not locked.
        
        @param name the name of the IdName
        @return the IdName subclass found or created
        """
        t = self.__byName.get(name,None)
        if t == None and not self.__locked:
            t = self.add(self.makeNew(name))
        return t
    
    def add(self, t):
        """
        Adds the IdName subclass to the map.
        
        @param t  the IdName subclass to add
        @return the IdName from the map
        @throws IllegalStateException if locked
        @throws IllegalArgumnetException if there is a collision with and existing entry by id or name
        """
        if self.__locked:
            raise IllegalStateException, "Locked"
        
        if t.getId() in self.__byId.keys():
            raise IllegalArgumentException, "id collision: %s" % t
        
        if t.getName() in self.__byName.keys():
            raise IllegalArgumentException, "name collision: %s" % t
        
        self.__byId[t.getId()]     = t
        self.__byName[t.getName()] = t
        return t
        
    def addAll(self, ts):
        """
        Adds all the IdName to this IdNameMap
        
        @param ts
        @throws IllegalStateException id locked
        @throws IllegalArgumentException if there is a collision with an existing entry by id or name
        """
        if self.__locked:
            raise IllegalStateException, "locked"
        
        for t in ts.values():
            self.add(t)
        
    def values(self):
        """
        @return a new set which is a snapshot of all the values
        """
        return self.__byId.values()
    
    def lock(self):
        """
        Locks the map, preventing further changes
        """
        self.__locked = True
    
    def size(self):
        """
        @return the number of values in the map
        """
        return len(self.__byId)
    
    def makeNew(self, name):
        """
        Makes a new subclass of IdName to put in this map.
        
        @param name the name of the new subclass of IdName
        @return a newly constructed subclass of IdName to put in this map
        """
        raise AbstractMethodException
    
