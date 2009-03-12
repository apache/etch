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
from etch.binding.support.RemoteBase import *

class What:
    SEND      = "SEND"
    CALL      = "CALL"
    BEGINCALL = "BEGINCALL"
    
class MyDeliveryService(DeliveryService):

    def _setUnittest(self, ut):
        self.unittest = ut

    def send(self, msg):
        self.unittest.assertEqual(None, self.what)
        self.what = What.SEND
        self.xmsg = msg

    def begincall(self, msg):
        self.unittest.assertEqual(None, self.what)
        self.what = What.BEGINCALL
        self.xmsg = msg
        self.xmb  = PlainMailBox(None, 0L, 0, 0, 1)
        return self.xmb

    def endcall(self, mb, responseType, responseField, timeout):
        self.unittest.assertEqual(What.BEGINCALL, self.what)
        self.unittest.assertEqual(self.xmb, mb)
        self.what = What.CALL
        self.xmb  = None
        self.xresponseType = responseType
        self.xtimeout      = timeout
        return 23
    
    def transportControl(self, control, value):
        pass

    def transportNotify(self, event):
        pass
    
    def transportQuery(self, query):
        pass
    
    def addStub(self, stub):
        pass
    
    def getSource(self):
        return None
    
class MyDefaultValueFactory(DefaultValueFactory):
    typs = TypeMap()
    class2type = Class2TypeMap()
    DefaultValueFactory.init(typs, class2type)
    
    def __init__(self):
        super(MyDefaultValueFactory,self).__init__(self.typs, self.class2type)
        

class Test_EtchSupportRemoteBase(unittest.TestCase):
    
    def setUp(self):
        self.svc = MyDeliveryService()
        self.vf  = MyDefaultValueFactory()
        self.rb  = RemoteBase(self.svc, self.vf)
        self.mt  = Type("foo")
        self.rmt = Type("bar")
        self.rmf = Type("baz")
        
    def test_newMessage(self):
        msg = self.rb._newMessage(self.mt)
        msg.checkType(self.mt)
        self.assertEqual(None, self.svc.what)
        self.assertEqual(None, self.svc.xmsg)
        self.assertEqual(None, self.svc.xresponseType)
        self.assertEqual(None, self.svc.xresponseField)
        self.assertEqual(None, self.svc.xtimeout)
        
    def test_send(self):
        msg = self.rb._newMessage(self.mt)
        self.rb._svc.send(self.msg)
        
        self.assertEqual(self.svc.what, What.SEND)
        self.assertEqual(self.svc.xmsg, msg)
        self.assertEqual(self.svc.xresponseType)
        self.assertEqual(self.svc.xresponseField)
        self.assertEqual(self.svc.xtimeout)
    
    def test_call(self):
        msg = self.rb._newMessage(self.mt)
        
        result = self.rb._svc.endcall(self.rb._svc.begincall(msg), self.rmt, self.rmf, 98)
        
        self.assertEqual(self.svc.what , What.CALL)
        self.assertEqual(self.svc.xmsg,  msg)
        self.assertEqual(self.svc.xresponseType, self.rmt)
        self.assertEqual(self.svc.xresponseField, self.rmf)
        self.assertEqual(self.svc.xtimeout, 98)
        self.assertEqual(result, 23)
    
    
if __name__ == '__main__':
    unittest.main()
        
