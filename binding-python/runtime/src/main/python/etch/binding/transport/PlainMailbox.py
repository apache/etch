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
from ..msg.Message import *
from ..support.Mailbox import *      # Mailbox and MailboxElement
from ...util.AlarmListener import *
from ...util.AlarmManager import *
from ...util.CircularQueue import *
from ...python.SynchronizedOn import *
from ...python.Exceptions import *

class PlainMailbox(Mailbox, AlarmListener):
    """
    A plain implementation of a mailbox using a fixed size circular queue
    """
    
    def __init__(self, mailboxManager, messageId, maxMessageDelay, lifetime, maxMessages):
        """
        Constructs the Mailbox
        
        @param mailboxManager - the mailbox manager to use to unregister this mailbox
        and to deliver undelivered messages.
        @param messageId
        @param maxMessageDelay - the maximum delay in milliseconds to wait when trying
        to put a message in the mailbox.
        @param lifetime - the time in milliseconds to keep the mailbox open. Lifetime
        of 0 means to keep it open until explicitly closed.
        @param maxMessages
        """
        if mailboxManager == None:
            raise NullPointerException, "mailboxManager cannot be None"
        
        if messageId == None:
            raise NullPointerException, "messageId cannot be None"
        
        if lifetime < 0:
            raise IllegalArgumentException, "lifetime < 0"
        
        if maxMessages < 1:
            raise IllegalArgumentException, "maxMessages < 1"
        
        self.__mailboxManager = mailboxManager
        self.__messageId = messageId
        self.__maxMessageDelay = maxMessageDelay
        self.__lifetime = lifetime
        self.__maxMessages = maxMessages
        
        self.__queue = CircularQueue(maxMessages)
    
        self.__alarmSet = False
        self.__notify = None
        self.__state  = None
        if lifetime > 0:
            self.__alarmSet = True
            AlarmManager.staticAdd(self, None, self.__lifetime)
        
    def getMailboxManager(self):
        """
        @return - the mailboxManager of this mailbox
        """
        return self.__mailboxManager
    
    def getMessageId(self):
        return self.__messageId
    
    def getMaxMessageDelay(self):
        """
        @return - the maxMessageDelay of this mailbox
        """
        return self.__maxMessageDelay
    
    def getLifetime(self):
        """
        @return - the lifetime of this mailbox
        """
        return self.__lifetime
    
    def getMaxMessages(self):
        """
        @return - the maxMessages of this mailbox
        """
        return self.__maxMessages
    
    # TODO: original signature 'boolean message(Who sender, Message msg)
    def queueMessage(self, sender, msg):
        ok = self.__queue.put(MailboxElement(sender, msg), maxMessageDelay)
        if ok:
            self.fireNotify()
        return ok
    
    def fireNotify(self):
        with SynchronizedOn(self.__queue):
            n = self.__notify
            s = self.__state
            c = self.__queue.isClosed()
        
        if n != None:
            n.mailboxStatus(self, s, c)
    
    def read(self, maxDelay=None):
        return self.__queue.get(maxDelay)
    
    def closeDelivery(self):
        with SynchronizedOn(self.__queue):
            if self.__queue.isClosed():
                return False
            
            if self.__alarmSet:
                self.__alarmSet = False
                AlarmManager.staticRemove(self)
            
            self.__mailboxManager.unregister(self)
            self.__queue.close()
        
        self.fireNotify()
        return True
    
    def closeRead(self):
        
        if closeDelivery():
            mbe = self.read()
            while mbe != None:
                self.__mailboxManager.redeliver(mbe.sender, mbe.msg)
                mbe = self.read()
                return True
        return False
    
    def wakeup(self, manager, state, due):
        self.closeDelivery()
        return 0
    
    def registerNotify(self, notify, state, maxDelay):
        if notify == None:
            raise NullPointerException, "Notify == None"
        
        if maxDelay < 0:
            raise IllegalArgumentException, "maxDelay < 0"
        
        if self.__notify != None:
            raise IllegalStateException, "self.__notify != None"
        
        isNotEmptyOrIsClosed = False
        with SynchronizedOn(self.__queue):
            self.__notify = notify
            self.__state  = state
            
            if maxDelay > 0:
                self.__alarmSet = True
                AlarmManager.staticAdd(self, None, maxDelay)
            
            isNotEmptyOrIsClosed = not self.__queue.isEmpty() or self.__queue.isClosed()
        
        if isNotEmptyOrIsClosed:
            self.fireNotify()
    
    def unregisterNotify(self, notify):
        if notify == None:
            raise NullPointerException, "notify == None"
        
        if notify != self.__notify:
            raise IllegalStateException, "notify != self.__notify"
        
        with SynchronizedOn(self.__queue):
            if self.__alarmSet:
                self.__alarmSet = False
                AlarmManager.staticRemove(self)
            
            self.__notify = None
            self.__state  = None
    
    def isEmpty(self):
        return self.__queue.isEmpty()
    
    def isClosed(self):
        return self.__queue.isClosed()
    
    def isFull(self):
        return self.__isFull()

