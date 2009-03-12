"""
$Id: URLSerializer.py 712749 2008-08-18 03:26:52Z dixson3 $

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
from __future__ import absolute_import
from ..msg.Field import *
from ..msg.ImportExportHelper import *
from ..msg.StructValue import *
from ..msg.Type import *
from ..msg.ValueFactory import *
from ..support.Class2TypeMap import *
from ..support.Validator_string import *
from ...util.URL import *

class URLSerializer(ImportExportHelper):
    """
    An etch serializer for URL
    """
    
    FIELD_NAME = "urlStr"
    
    @classmethod
    def init(cls, typ, class2type):
        """
        Defines custom fields in the value factory so that the importer can find them.
        
        @param typ
        @param class2type
        """
        field = typ.getField(cls.FIELD_NAME)
        class2type.put(URL, typ)
        typ.setComponentType(URL)
        typ.setImportExportHelper( URLSerializer(typ, field))
        typ.putValidator( field, Validator_string.get(0))
        typ.lock()
    
    def __init__(self, typ, field):
        self.__type = typ
        self.__field = field
        
    def importValue(self, struct):
        return URL(struct.get(field))
    
    def exportValue(self, vf, value):
        struct = StructValue(self.__type, vf)
        struct.put(self.__field, repr(value))
        return struct
