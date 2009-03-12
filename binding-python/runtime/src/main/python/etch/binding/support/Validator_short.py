"""
Etch.Support.Validator_short

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
from etch.transport.fmt.TypeCode import *
from etch.util.Exceptions import *
from etch.util.Types import *

class Validator_short(NumberTypeValidator):
    """
    Validator for short
    """

    def __init__(self, nDims):
        """
        Construct the validator.

        @param nDims    the number of dimensions. 0 or scalar.
        """
        typs = [Short]
        super(Validator_short,self).__init__(typs, None, nDims, "short[%d]" % nDims, Short, [TypeCode.SHORTS, TypeCode.SHORT])
    
    def checkValue(self, value):
        tc = super(Validator_short, self).checkValue(value)
        
        if tc == TypeCode.SHORT:
            if value >= Byte.MIN_VALUE and value <= Byte.MAX_VALUE:
                return TypeCode.BYTE
        return tc

