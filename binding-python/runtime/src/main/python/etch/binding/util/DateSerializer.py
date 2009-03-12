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
from ..support.Validator_long import *
import time

class DateSerializer(ImportExportHelper):
    """
    An etch serializer for Date.
    """
    
    FIELD_NAME = "dateTime"

    @classmethod
    def init(cls, typ, class2type):
        field = typ.getField(cls.FIELD_NAME)
        class2type.put(time.struct_time, typ)
        typ.setComponentType(time.struct_time)
        typ.setImportExportHelper( DateSerializer(typ, field) )
        typ.putValidator( field, Validator_long.get(0))
        typ.lock()
    
    def __init__(self, typ, field):
        self.__type = typ
        self.__field = field
    
    def importValue( self, struct ):
        # Convert long-int ot time_struct
        # TODO - confirm/normalize epoch
        return time.gmtime(struct.get(self.__field))
    
    def exportValue( self, vf, value):
        struct = StructValue(self.__type, vf)
        # Convert time.time_struct to long-int
        # TODO - confirm/normalize epoch
        struct.put(self.__field, long(time.mktime(value)))
        return struct
        
