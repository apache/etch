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
from ..msg.Field import *
from ..msg.ImportExportHelper import *
from ..msg.StructValue import *
from ..msg.Type import *
from .Class2TypeMap import *
from .Validator_string import *
from ._Etch_AuthException import *

class AuthExceptionSerializer(ImportExportHelper):
    
    FIELD_NAME = "msg"
    
    @staticmethod
    def init(typ, class2type):
        field = typ.getField(AuthExceptionSerializer.FIELD_NAME)
        class2type[_Etch_AuthException] = typ
        typ.setComponentType(_Etch_AuthException)
        typ.setImportExportHelper(AuthExceptionSerializer(typ, field))
        typ.putValidator(field, Validator_string.get(0))
        typ.lock()
    
    def __init__(self, typ, field):
        self.__type  = typ
        self.__field = field
    
    def exportValue(self, value):
        sv = StructValue(self.__type)
        sv[field] = repr(value)
        return sv
    
    def importValue(self, structValue):
        structValue.checkType(self.__type)
        return _Etch_AuthException(structValue[self.__field])

        
