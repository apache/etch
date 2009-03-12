"""
etch.support.Validator_object

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

class Validator_object(ScalarOnlyTypeValidator):
    """
    Validator for object
    """
    validator = None

    @classmethod
    def get(cls, nDims):

        if cls.validator == None:
            cls.validator = cls()
        return cls.validator

    def elementValidator():
        return self

    def validate(self, value):
        return True

    def checkValue(self, value):
        return TypeCode.ANY