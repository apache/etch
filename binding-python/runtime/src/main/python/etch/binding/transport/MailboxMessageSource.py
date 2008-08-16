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
from ..msg.Message import *
from ..support.Mailbox import *
from ..support.MessageSource import *
from ...util.core.Who import *
from ...util.core.io.Source import *

class MailboxMessageSource(MessageSource,Source):
    """
    Interface used to send messages back to a message source
    """
    
    def sendMessage(self, recipient, msg, maxMessages, lifetime):
        """
        Sends a message to a message source. The message is encoded in some
        way (e.g., compressed binary format), and then delivered to a transport
        (e.g., packetized data stream).
        
        @param recipient - a transport specific opaque value which identifies
        the sender to a message handler and mya be passed here to send a message
        back. Passing None means "use the default recipient".
        @param msg - the message to send.
        @param maxMessages - maximum number of messages which can be queued to
        the mailbox before it is full.
        @param lifetime - time in milliseconds until mailbox expires and is
        closed. 0 means don't expire the mailbox.
        @return - a mailbox created to receive replies to this message.
        @raises Exception
        """
        raise UnimplementedInterfaceMethodException
