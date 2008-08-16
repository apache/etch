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
from __future__ import absolute_import
from ...python.Exceptions import *

class MailboxManagerIntf(object):
    """
    Interface to the mailbox manager as needed by Mailbox implementers
    """
    
    def unregister(self, mb):
        """
        Removes the mailbox from the set of mailbox receiving responses to messages
        
        @param mb
        """
        raise UnimplementedInterfaceMethodException
    
    def redeliver(self, sender, msg):
        """
        Redelivers defunct messages from a closed mailbox
        
        @param sender
        @param msg
        @raises Exception
        """
        raise UnimplementedInterfaceMethodException
        