"""
etch.support.Validator_RuntimeException

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

from etch.support.TypeValidator import *
from etch.transport.fmt.TypeCode import TypeCode
from etch.util.Exceptions import *

class Validator_RuntimeException(ScalarOnlyTypeValidator):
    """
    Validator for RuntimeException
    """

    def __init__(self):
        self.description = "RuntimeException"

    def elementValidator(self):
        raise IllegalArgumentException, "Not an array"

    def validate(self, value):
        return isinstance(value, Exception)

    def checkValue(self, value):
        if self.validate(value):
            return TypeCode.CUSTOM
        return None
