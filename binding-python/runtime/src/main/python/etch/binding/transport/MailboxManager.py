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
from __future__ import with_statement
from .MailboxManagerIntf import *
from .MessageHandler import *
from .MailboxMessageSource import *
from .PlainMailbox import *
from ..msg.Message import *
from ..support.Mailbox import *
from ...util.IdGenerator import *
from ...util.Log import *
from ...util.Resources import *
from ...util.core.Who import *
from ...python.Exceptions import *
from ...python.SynchronizeOn import *

class MailboxManager(MailboxManagerIntf,MessageHandler,MailboxMessageSource):
    """
    MailboxManager is a MessageHandler which accepts packets for possible delivery
    to a mailbox, or to another message handler if an appropriate mailbox cannot
    be found. MailboxManager is also a MessageSource, tagging messages with
    msgids and then forwarding them to another MessageSource. If requested, a
    mailbox is created with a message's msgid and added to the set of mailboxes
    waiting for messages.
    """
    
    def __init__(self, defaultHandler=None, maxDelay=0, uri=None, resources=None):
        """
        @param defaultHandler - a message handler to receive messages which
        cannot be delivered to a mailbox
        @param maxDelay - maximum amount of time to wait for a full mailbox
        to allow a new message before giving up and delivering to the message
        handler. 0 means wait forever, -1 means don't wait at all.
        @param uri - not used
        @param resources - not used
        """
        self.__handler  = defaultHandler
        self.__maxDelay = maxDelay
        
        self.__src = None
        self.__idGen = IdGenerator(time.time(), 37)
    
    def __repr__(self):
        return "MailboxManager/%s" % repr(self.__src)

    def message(self, *args, **kwargs):
        # sigh .. overloading
        if len(args) == 2:
            return self.isMessageDelivered(*args)
        else:
            return self.sendMessage(*args)
    
    ## MessageHandler Methods
    
    ## TODO: replace calls to (boolean) message(...) with 'queueMessage'??
    ## boolean message(Who sender, Message msg)
    def isMessageDelivered(self, sender, message):
        #Log.report("MailboxManager.recv", "msg", msg)
        msgid = msg.getInReplyTo()
        if msgid != None:
            mb = self.getMailbox(msgid)
            #Log.report("MailboxManager.recv", "msg", msg, "mb", mb)
            if mb == None:
                try:
                    return mb.message(sender, msg)
                except InterrupedException, e:
                    # timeout or mailbox closed -- fall through
                    pass
        delivered = handler.message(sender, msg)
        if not delivered:
            Log.report("MailboxManager.undelivered", "msg", msg)
        return delivered
        
        
    ## MessageSource Methods
    
    def messagex(self, recipient, msg):
        msg.setMessageId(self.__idGen.next())
        # Log.report("MailboxManager.send", "msg", msg)
        self.__src.messagex(recipient, msg)
    
    ## TODO: replace calls to (Mailbox) message(...) with 'getMessageMailbox'
    ## Mailbox message(Who recipient, Message msg, int maxMessages, int lifetime)
    def sendMessage(self, recipient, msg, maxMessages, lifetime):
        if lifetime < 0:
            raise IllegalArgumentException, "lifetime < 0"
        
        msgid = self.__idGen.next()
        msg.setMessageId(msgid)
        
        mb = PlainMailbox(self, msgid, maxDelay, lifetime, maxMessages)
        self.register(mb)
        
        #Log.report("MailboxManager.send", "msg", msg)
        try:
            self.__src.messagex(recipient, msg)
        except Exception, e:
            self.unregister(mb)
            raise Exception, e
        
        return mb
    
    ## Mailbox Methods
    
    def register(self, mb):
        """
        Adds a mailbox to the set of mailbox receiving responses to messages
        @param mb
        """
        msgid = mb.getMessageId()
        with SynchronizedOn(self.__mailboxes):
            if msgid in self.__mailboxes:
                raise IllegalArgumentException, "Duplicate msgid in mailboxes"
            self.__mailboxes[msgid] = mb

    def unregister(self, mb):
        with SynchronizedOn(self.__mailboxes):
            del self.__mailboxes[mb.getMessageId()]
    
    def getMailbox(self, msgid):
        with SynchronizedOn(self.__mailboxes):
            return self.__mailboxes.get(msgid,None)
    
    def redeliver(self, sender, msg):
        self.__handler.message(sender, msg)
    
    ## SourceHandler methods
    
    def getSource(self):
        return self.__src
    
    def setSource(self, src):
        self.__src = src
    
    def sessionQuery(self, query):
        return self.__handler.sessionQuery(query)
    
    def sessionControl(self, control, value):
        self.__handler.sessionControl(control, value)
    
    def sessionNotify(self, event):
        self.__handler.sessionNotify(event)
    
    ## Source Methods
    
    def getHandler(self):
        return self.__handler
        
    def setHandler(self, handler):
        self.__handler = handler
        self.__handler.setSource(self)
    
    def transportQuery(self, query):
        return self.__src.transportQuery(control, value)
    
    def transportNotify(self, event):
        self.__src.transportNotify(event)
    

    
     
     