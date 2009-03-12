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
from __future__ import with_statement
from ...python.SynchronizedOn import *
from ...python.SynchronizedMap import *
from ...python.Exceptions import *
from .MailboxManager import *
from ..msg.Message import *
from ..support.Mailbox import *
from ...util.IdGenerator import *
from ...util.Resources import *
from ...util.URL import *
from ...util.core.Who import *
from ...util.core.io.Session import *

class PlainMailboxManager(MailboxManager):
    """
    MailboxManager is a MessageHandler which accepts packets for possible delivery to a mailbox, or
    to another message handler if an appropriate mailbox cannot be found. MailboxManager is also a
    MessageSource, tagging messages with msgIds and then forwarding them to another MessageSource.
    If requested, a mailbox is created with a message's msgid and added to the set of mailboxes waiting
    for messages.
    """
    
    def __init__(self, transport, uri, resources):
        """
        @param transport used to deliver messages to our peer
        @param uri
        @param resources
        """
        self.__transport = transport
        self.__session   = None
        self.__up        = False
        self.__mailboxes = SynchronizedMap()
        
        # TODO - change 'seed' 
        self.__idGen     = IdGenerator('seed', 37)
        self.__transport.setSession(self)
    
    def getTransport(self):
        """
        @return the transport
        """
        return self.__transport
    
    def __repr__(self):
        return "PlainMailboxManager/%s" % repr(self.__transport)
    
    ####
    # MessageHandler Methods
    ####
    
    def sessionMessage(self, sender, msg):
        msgid = msg.getInReplyTo()
        if msgid != None:
            mb = self.getMailbox(msgid)
            if mb != None:
                try:
                    return mb.message(sender, msg)
                except InterruptedException:
                    # timeout or mailbox closed .. fall through
                    pass
            # no such mailbox .. reject
            return False
        # no msgid .. pass of to session
        return self.__session.sessionMessage(sender, msg)
    
    ####
    # MessageSource Methods
    ####
    
    def transportMessage(self, recipient, msg):
        if msg.getMessageId() != None:
            raise IllegalStateException, "message has already been sent"
        
        msg.setMessageId(self.__idGen.next())
        
        self.__transport.transportMessage(recipient, msg)
    
    def transportCall(self, recipient, msg):
        if msg.getMessageId() != None:
            raise IllegalStateException, "message has already been sent"
        
        if msg.getInReplyTo() != None:
            raise IllegalStateException, "message is marked as a reply"
        
        msgid = self.__idGen.next()
        msg.setMessageId(msgid)
        
        mb = PlainMailbox(self, msgid)
        self.register(mb)
        
        try:
            self.__transport.transportMessage(recipient, msg)
        except:
            self.unregister(mb)
            raise Exception
        
        return mb
    
    ####
    # Mailbox methods
    ####
    
    def register(self, mb):
        """
        Adds a mailbox to the set of mailbox receiving responses to messages.
        @param mb
        """
        msgid = mb.getMessageId()
        with SynchronizedOn(self.__mailboxes):
            if not self.__up:
                raise IllegalStateException, "connection down"
            
            if msgid in self.__mailboxes.keys():
                raise IllegalArgumentException, "duplicate msgid in mailboxes"
            
            self.__mailboxes[msgid] = mb
        
        
    def unregister(self, mb):
        del self.__mailboxes[mb.getMessageId()]
    
    def unregisterAll(self):
        for mb in self.__mailboxes.values():
            mb.closeDelivery()
    
    def getMailbox(self, msgid):
        """
        Returns the mailbox for the specified msgid. *This is a testing api*
        @param msgid
        @return the mailbox for the specified msgid
        """
        return self.__mailboxes[msgid]
    
    def redeliver(self, sender, msg):
        self.__session.sessionMessage(sender, msg)
    
    def size(self):
        """
        @return the number of mailboxes
        """
        return len(self.__mailboxes)
    
    ####
    # SourceHandler methods
    ####
    
    def sessionQuery(self, query):
        return self.__session.sessionQuery(query)
    
    def sessionControl(self, control, value):
        self.__session.sessionControl(control, value)
    
    def sessionNotify(self, event):
        if event == Session.UP:
            self.__up = True
        elif event == Session.DOWN:
            self.__up = False
            self.unregisterAll()
        self.__session.sessionNotify(event)
    
    ####
    # Source methods
    ####
    
    def transportQuery(self, query):
        return self.__transport.transportQuery(query)
    
    def transportControl(self, control, value):
        self.__transport.transportControl(control, value)
    
    def transportNotify(self, event):
        self.__transport.transportNotify(event)
    
    def getSession(self):
        return self.__session
    
    def setSession(self, session):
        self.__session = session


        
    
    
    
    
