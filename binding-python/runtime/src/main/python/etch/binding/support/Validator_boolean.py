"""
Etch.Support.Validator_boolean

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
import types
from etch.support.TypeValidator import *
from etch.transport.fmt.TypeCode import TypeCode
from etch.util.Exceptions import *
from etch.util.Types import *
from etch.transport.fmt.TypeCode import *

class Validator_boolean(ScalarAndArrayTypeValidator):
    """
    Validator for boolean
    """

    def __init__(self, nDims=0):
        """
        Construct the validator.

        @param nDims    the number of dimensions. 0 or scalar.
        """
        typs = [types.BooleanType]
        super(Validator_boolean,self).__init__(typs, None, nDims, "boolean[%d]" % nDims, False, [TypeCode.BOOLS, TypeCode.BOOLEAN_TRUE])


    def checkValue(self, value):
        tc = super(Validator_boolean,self).checkValue(value)
            
        if tc == TypeCode.BOOLEAN_TRUE:
            if value == False:
                tc = TypeCode.BOOLEAN_FALSE
        
        return tc
