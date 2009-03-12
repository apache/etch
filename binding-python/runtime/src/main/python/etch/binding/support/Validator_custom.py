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
import types

from ...python.Exceptions import *
from ...python.Types import *
from .TypeValidator import *
from ..transport.fmt.TypeCode import *


class Validator_custom(ScalarAndArrayTypeValidator):
    """
    Validator for Custom Types
    """

    def __init__(self, className, nDims=0):
        """
        Constructor

        @param className   The class name to validate
        @param nDims       The number of dimensions, use 0 for scalar
        """
        super(Validator_custom,self).__init__(className, className, nDims, "%s[%d]" % (repr(className), nDims), True, TypeCode.CUSTOM)
        self.__class__.setClassName(className)
