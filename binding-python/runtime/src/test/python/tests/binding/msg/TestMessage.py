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

import unittest
from etch.python.Types import *
from etch.binding.msg.Message import Message
from etch.binding.msg.Type import Type
from etch.binding.msg.Field import Field
from etch.binding.msg.ValueFactory import ValueFactory
from etch.binding.support.Validator_int import Validator_int
from etch.binding.support.Validator_long import Validator_long


class Test_EtchMsgMessage(unittest.TestCase):

    def setUp(self):
        self.__vf  = MyValueFactory()
        self.__mt1 = Type(1,"mt")
        self.__rmt = Type(2,"rmt")
        self.__mf1 = Field(3,"x")
        self.__mf2 = Field(4,"y")

        self.__mt1.putValidator(self.__mf1, Validator_int.get(0))
        self.__mt1.putValidator(self.__mf2, Validator_int.get(0))
        self.__mt1.putValidator(self.__vf.mf_messageId, Validator_long.get(0))

        self.__rmt.putValidator(self.__vf.mf_messageId, Validator_long.get(0))
        self.__rmt.putValidator(self.__vf.mf_inReplyTo, Validator_long.get(0))

    def test_getVf(self):
        msg = Message(self.__mt1, self.__vf)
        self.assertEquals(self.__vf, msg.vf())
        
    def test_reply(self):
        msg = Message( self.__mt1, self.__vf)
        msg.setMessageId(12345)
        
        rmsg = msg.reply(self.__rmt)
        self.assertEquals(self.__rmt, rmsg.type() )
        self.assertEquals(self.__vf, rmsg.vf() )
        self.assertEquals(12345, rmsg.getInReplyTo())
        
    def test_getMessageId(self):
        msg = Message(self.__mt1, self.__vf)
        self.assertEquals(None, msg.getMessageId())
        msg.setMessageId(234)
        self.assertEquals(234, msg.getMessageId())
        msg.setMessageId(345)
        self.assertEquals(345, msg.getMessageId())
            
    def getInReplyTo(self):
        msg = Message(self.__rmt, self.__vf)
        self.assertEquals(None, msg.getInReplyTo())
        msg.setInReplyTo(234)
        self.assertEquals(234, msg.getInReplyTo())
    
    def setInReplyTo(self):
        msg = Message(self.__rmt, self.__vf)
        self.assertEquals(None, msg.getInReplyTo())
        msg.setInReplyTo(234)
        self.assertEquals(234, msg.getInReplyTo())
        msg.setInReplyTo(345)
        self.assertEquals(345, msg.getInReplyTo())
    
class MyValueFactory(ValueFactory):
    
    def __new__(cls, *args, **kwargs):
        cls.mf_messageId = Field("_messageId")
        cls.mf_inReplyTo = Field("_inReplyTo")
        return ValueFactory.__new__(cls,*args, **kwargs)
        
    def getMessageId(self, msg):
        cls = self.__class__
        return msg.get(cls.mf_messageId)
    
    def setMessageId(self, msg, msgid):
        cls = self.__class__
        msg.put(cls.mf_messageId, msgid)
    
    def getInReplyTo(self, msg):
        cls = self.__class__
        return msg.get(cls.mf_inReplyTo)
    
    def setInReplyTo(self, msg, msgid):
        cls = self.__class__
        msg.put(cls.mf_inReplyTo, msgid)
    
    
        
        
if __name__ == '__main__':
    unittest.main()
