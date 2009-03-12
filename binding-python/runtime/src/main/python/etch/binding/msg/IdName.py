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
from ...util.Hash import *
from ...python.Exceptions import *

__all__ = ['IdName']

class IdName(Hash):
    """
    An IdName is a base class of Field or Type. It is used to bind together a
    type or field name with the associated id. The id is used for certain
    operations, such as the key in a Map, comparisons, and binary encoding on
    the wire, while the name is used for display.

    @see Field
    @see Type
    """

    def __init__(self, *args):
        """
        Constructs the IdName.

        @param id       The 'id' for the 'name' (normally computed using
                        _hash('name')).
        @param name     The 'name' of the item.
        """
        if  len(args) < 1 or len(args) > 2:
            raise IllegalArgumentException, "Incorrect number of arguments: must specify either ('name') or ('id','name')"

        if len(args) == 1:
            # Compute the appropriate msg_id given the msg_name
            msg_id   = self.hash(args[0])
            msg_name = args[0]

        if len(args) == 2:
            # Take the msg_id and msg_name from the arg list
            msg_id   = args[0]
            msg_name = args[1]

        self.__id   = msg_id
        self.__name = msg_name

    def getId(self):
        """
        @return the 'id' of the item.
        """
        return self.__id

    def getName(self):
        """
        @return the 'name' of the item.
        """
        return self.__name

    def __repr__(self):
        """
        Return the string representation of the item.'
        """
        return "%s(%d)" % (self.__name, self.__id)

    def __ne__(self, obj):
        return not self.__eq__(obj)

    def __eq__(self, obj):
        """
        Return boolean equals
        """
        if id(self) == id(obj):
            return True

        if obj == None:
            return False

        if obj.__class__ != self.__class__:
            return False

        return ((self.__id == obj.getId()) and (self.__name == obj.getName()))

    def __hash__(self):
        """
        Return the computed 'id' for the IdName.

        TODO: is this really necessary or equivalent to Java's .hashCode method?
        """
        return id(self.__id) ^ id(self.__name)

