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
from .Validator import *

class ComboValidator(Validator):
    """
    Validates values using one of two validators.
    """

    def __init__(self, a, b):
        """
        Constructs the validator
        """
        self.__a = a
        self.__b = b
    
    def __repr__(self):
        return "(%s OR %s)" % (repr(self.__a), repr(self.__b))

    # elementValidator
    def elementValidator(self):
        
        na = None
        try:
            na = self.__a.elementValidator()
        except IllegalArgumentException:
            na = None
        
        
        nb = None
        try:
            nb = self.__b.elementValidator()
        except IllegalArgumentException:
            nb = None
        
        if na == None and nb == None:
            raise IllegalArgumentException, "na == None and nb == None"
        
        if na == None:
            return nb
        
        if nb == None:
            return na
        
        return ComboValidator(na, nb)
    
    def validate(self, value):
        return self.__a.validate(value) or self.__b.validate(value)
    
    def validateValue(self, value):
        try:
            return self.__a.validateValue(value)
        except IllegalArgumentException:
            return self.__b.validateValue(value)

    def a(self):
        """
        @return the 'a' child of this validator.
        """
        return self.__a
        
    def b(self):
        """
        @return - the 'b' child of this validator.
        """
        return self.__b

