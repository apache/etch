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
from ..msg.Message import *
from ...util.core.Who import *
from ...util.core.io.Session import *

class SessionMessage(Session):
    """
    Interface used to deliver messages to the session from the transport.
    """
    
    def sessionMessage(self, sender, msg):
        """
        Delivers data to the session from the transport.
        
        @param sender the sender of the message, for transport which allow multiple sends. This is who
        to return the response to.
        @param msg a Message
        @return true if the message was consumed, false otherwise
        """
        raise UndefinedInterfaceMethodException
