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
from ..msg.ValueFactory import *
from ..support.Class2TypeMap import *
from ..support.Validator_object import *

class MapSerializer(ImportExportHelper):
    """
    Serializer for generic dict
    """
    
    FIELD_NAME = "keysAndValues"
    
    @classmethod
    def init(cls, typ, class2type):
        """
        Define custom fields in the value factory so that the importer can find them.
        
        @param typ
        @param class2type
        """
        field = typ.getField(cls.FIELD_NAME)
        class2type.put(dict, typ)
        typ.setComponentType(dict)
        typ.setImportExportHelper( MapSerializer(typ, field))
        typ.putValidator(field, Validator_object.get(1))
        typ.lock()
    
    def __init__(self, typ, field):
        self.__type  = typ
        self.__field = field
        
    
    def importValue(self, struct):
        struct.checkType(self.__type)
        keysAndValues = struct.get(self.__field)
        h = {}
        for i in range(0,len(keysAndValues),2):
            h[keysAndValues[i]] = keysAndValues[i+1]
        return h
    
    def exportValue(self, vf, value):
        keysAndValues = []
        for x in value.keys():
            keysAndValues.append(x)
            keysAndValues.append(value[x])
        
        struct = StructValue(self.__type, vf)
        struct.put(self.__field, keysAndValues)
        return struct
        
    
