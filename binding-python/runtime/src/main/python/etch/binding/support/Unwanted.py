"""
$Id$

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
from __future__ import *
from ...python.Exceptions import *

class Unwanted(object):
    """
    Interface which a service implementation may implement which is used by
    the service stub to give notice of an unwanted message. This is a message
    which was not wanted by the service stubs (its 'id' did not match any
    service method)
    """
    
    def _unwanted(self, sender, msg):
        """
        Notifies the service implementation that the message is unwanted, that
        is, its 'id' does not match any defined message.
        
        @param sender   The transport defined sender
        @param msg      The message that was not wanted
        """
        raise UndefinedInterfaceMethodException
