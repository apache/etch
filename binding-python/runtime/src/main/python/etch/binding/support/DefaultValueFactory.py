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
from ..msg.Message import *
from ..msg.StructValue import *
from ..msg.Type import *
from ..msg.ValueFactory import *
from ..util.DateSerializer import *
from ..util.ListSerializer import *
from ..util.MapSerializer import *
from ..util.SetSerializer import *
from .Validator_RuntimeException import *
from .Validator_long import *

class DefaultValueFactory(ValueFactory):
    """
    Default implementation of ValueFactory which provides some
    dynamic type and field support, as well as standard value
    conversions and import and rt.
    """

    # Names

    ETCH_RUNTIME_EXCEPTION_TYPE_NAME = "_Etch_RuntimeException"
    ETCH_LIST_TYPE_NAME = "_Etch_List"
    ETCH_MAP_TYPE_NAME = "_Etch_Map"
    ETCH_SET_TYPE_NAME = "_Etch_Set"
    ETCH_DATETIME_TYPE_NAME = "_Etch_Datetime"
    ETCH_AUTH_EXCEPTION_TYPE_NAME = "_Etch_AuthException"
    ETCH_EXCEPTION_MESSAGE_NAME = "_exception"
    MSG_FIELD_NAME = "msg"
    MESSAGE_ID_FIELD_NAME = "_messageId"
    IN_REPLY_TO_FIELD_NAME = "_inReplyTo"
    RESULT_FIELD_NAME = "result"

    # Fields

    _mf_msg = Field(MSG_FIELD_NAME)
    """The msg field of the standard unchecked exception"""

    _mf__messageId = Field(MESSAGE_ID_FIELD_NAME)
    """The well-known _messageId field"""

    _mf__inReplyTo = Field(IN_REPLY_TO_FIELD_NAME)
    """The well-known _inReplyTo field"""

    _mf_result = Field(RESULT_FIELD_NAME)
    """The well-known result field"""


    @staticmethod
    def init(typs, class2type):
        """
        Initializes the standard types and fields needed by all
        etch generated value factories.

        @param types
        @param class2type
        """
        cls = DefaultValueFactory

        RuntimeExceptionSerialzier.init(typs[cls.ETCH_RUNTIME_EXCEPTION_TYPE_NAME], class2type)
        ListSerialzier.init(typs[cls.ETCH_LIST_TYPE_NAME], class2type)
        MapSerialzier.init(typs[cls.ETCH_MAP_TYPE_NAME], class2type)
        SetSerialzier.init(typs[cls.ETCH_SET_TYPE_NAME], class2type)
        DateSerialzier.init(typs[cls.ETCH_DATETIME_TYPE_NAME], class2type)
        AuthExceptionSerialzier.init(typs[cls.ETCH_AUTH_EXCEPTION_TYPE_NAME], class2type)
        
        # _mt__Etch_AuthException

        t = typs.get(cls.ETCH_EXCEPTION_MESSAGE_NAME)
        t.putValidator( cls._mf_result, Validator_RuntimeException.get())
        t.putValidator( cls._mf__messageId, Validator_long.get(0))
        t.putValidator( cls._mf__inReplyTo, Validator_long.get(0))

    def __init__(self, typs, class2type):
        """
        Constructs the DefaultValueFactory.

        @param typs
        @param class2type
        """
        cls = self.__class__
        self.__types = typs
        self.__class2type = class2type
        self._mt__Etch_RuntimeException = typs.get(cls.ETCH_RUNTIME_EXCEPTION_TYPE_NAME)
        self._mt__Etch_List = typs.get(cls.ETCH_LIST_TYPE_NAME)
        self._mt__Etch_Map = typs.get(cls.ETCH_MAP_TYPE_NAME)
        self._mt__Etch_Set = typs.get(cls.ETCH_SET_TYPE_NAME)
        self._mt__Etch_Datetime = typs.get(cls.ETCH_DATETIME_TYPE_NAME)
        self._mt__Etch_AuthException = typs.get(cls.ETCH_AUTH_EXCEPTION_TYPE_NAME)
        self._mt__exception = typs.get(cls.ETCH_EXCEPTION_MESSAGE_NAME)

    def get_mt__Etch_RuntimeException(self):
        return _mt__Etch_RuntimeException
        

