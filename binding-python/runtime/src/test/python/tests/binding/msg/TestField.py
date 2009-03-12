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
from etch.binding.msg.Field import Field
from etch.binding.msg.IdName import IdName

class Test_EtchMsgField(unittest.TestCase):

    def setUp(self):
        #print " "
        pass

    def test_fieldIntegerString(self):
        self._testMf(1, "one")
        self._testMf(2, "two")
        self._testMf(3, "three")

    def test_fieldString(self):
        self._testMf("one")
        self._testMf("two")
        self._testMf("three")

    def _testMf(self, *args):
        if len(args) == 1:
            name = args[0]
            mf   = Field(name)
            self.assertEquals(IdName.hash(name), mf.getId())
            self.assertEquals(name, mf.getName())
        elif len(args) == 2:
            i = args[0]
            n = args[1]
            mf = Field(i,n)
            self.assertEquals(i, mf.getId())
            self.assertEquals(n, mf.getName())
        else:
            pass

if __name__=='__main__':
    unittest.main()
