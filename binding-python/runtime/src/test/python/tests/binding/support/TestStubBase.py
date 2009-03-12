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
from etch.binding.support.StubBase import *

class What:
    UNWANTED = 'UNWANTED'
    
class MyMessageSource(MessageSource):
    def messagex(self, recipient, msg):
        raise Exception, "not needed"

class MyUnwanted(Unwanted):
    def _unwanted(self, sender, msg):
        assert(self.what == None)
        self.what    = What.UNWANTED
        self.xsender = sender
        self.xmsg    = msg
    
class Test_EtchSupportStubBase(unittest.TestCase):
    
    def test_nothing(self):
        pass

    def test_unwanted2(self):
        obj = MyUnwanted()
        src = MyMessageSource()
        sb  = StubBase(src, obj, None, None)
        
        sender = Who()
        msg    = Message()
        sb.message(sender, msg)
        
        self.assertEqual(obj.what, What.UNWANTED)
        self.assertEqual(sender, obj.xsender)
        self.assertEqual(msg, obj.xmsg)
        
    def test_unwanted4(self):
        obj = object()
        src = MyMessageSource()
        sb  = StubBase(src, obj, None, None)
        
        
        sender = Who()
        msg    = Message(None, None)
        
        self.assertRaises(UnsupportedOperationException, sb.message, sender, msg)
    
    
        
if __name__ == '__main__':
    unittest.main()
