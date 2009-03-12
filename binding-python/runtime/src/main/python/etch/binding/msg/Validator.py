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

class Validator(object):
    """
    Validate values put into messages.
    """

    MAX_NDIMS  = 9
    """The maximum number of dimensions for arrays (9)"""

    MAX_CACHED = 4
    """The maximum number of validators to cache per type."""
    
    NONE = "Validator.NONE"
    """No validation on put or output"""
    
    MISSING_OK = "Validator.MISSING_OK"
    """Validation on put or output if validator defined"""
    
    FULL = "Validator.FULL"
    """Like MISSING_OK, but fail if no validator defined"""

    def elementValidator(self):
        """
        Return for an array type, return a validator for an element of the array
        """
        raise AbstractMethodException

    def validate(self, value):
        """
        Checks the value for being valid. If valid, return true

        @param value      The value to be validated
        @return           True if the value is valid by any validator in the chain
        """
        raise AbstractMethodException

    def validateValue(self, value):
        """
        @param value
        @return the appropriate value given the input value and this validator's
        sensibility about what it should really be (e.g., input Byte, but output
        Long.
        """
        raise AbstractMethodException
    
