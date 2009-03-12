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

from etch.msg.Validator import Validator
from etch.transport.fmt.TypeCode import TypeCode
from etch.transport.fmt.binary.BinaryTaggedData import BinaryTaggedData

class Validator_none(Validator):

    validator = None

    @staticmethod
    def get():
        if Validator_none.validator == None:
            Validator_none.validator = Validator_none()
        return Validator_none.validator

    def elementValidator(self):
        return None

    def validate(self, value):
        return value == BinaryTaggedData.NONE

    def checkValue(self, value):
        if self.validate(value):
            return TypeCode.NONE
        else:
            return None

