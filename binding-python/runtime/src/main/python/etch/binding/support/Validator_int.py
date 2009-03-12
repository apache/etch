"""
Etch.Support.Validator_int

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

class Validator_int(NumberTypeValidator):
    """
    Validator for int
    """

    def __init__(self, nDims):
        """
        Construct the validator.

        @param nDims    the number of dimensions. 0 or scalar.
        """
        typs = [Integer]
        super(Validator_int,self).__init__(typs, None, nDims, "int[%d]" % nDims, Integer, [TypeCode.INTS, TypeCode.INT])

    def checkValue(self, value):
        tc = super(Validator_int,self).checkValue(value)

        if tc == TypeCode.INT:
            if value >= Byte.MIN_VALUE and value <= Byte.MAX_VALUE:
                return TypeCode.BYTE
            if value >= Short.MIN_VALUE and value <= Short.MAX_VALUE:
                return TypeCode.SHORT
        return tc
