"""
etch.support.Validator_void

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

from etch.msg.TypeValidator import *
from etch.transport.fmt.TypeCode import TypeCode

class Validator_void(ScalarOnlyTypeValidator):
    """
    Validator for void
    """

    def __init__(self):
        self.description = 'void'

    def elementValidator(self):
        return None

    def validate(self, value):
        return value == None

    def checkValue(self, value):
        if self.validate(value):
            return TypeCode.NULL
        return None
