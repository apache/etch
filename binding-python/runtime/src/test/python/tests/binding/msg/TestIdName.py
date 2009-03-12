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
from etch.binding.msg.IdName import IdName

class Test_EtchMsgIdName(unittest.TestCase):

    def setUp(self):
        #print " "
        pass

    def test_getId(self):
        self.assertEquals(1, IdName(1, "foo").getId())
        self.assertEquals(1, IdName(1, "bar").getId())
        self.assertEquals(2, IdName(2, "foo").getId())
        self.assertEquals(2, IdName(2, "bar").getId())

    def test_getName(self):
        self.assertEquals("foo", IdName(1, "foo").getName())
        self.assertEquals("bar", IdName(1, "bar").getName())
        self.assertEquals("foo", IdName(2, "foo").getName())
        self.assertEquals("bar", IdName(2, "bar").getName())

    def test_repr(self):
        self.assertEquals("foo(1)", repr(IdName(1, "foo")))
        self.assertEquals("bar(1)", repr(IdName(1, "bar")))
        self.assertEquals("foo(2)", repr(IdName(2, "foo")))
        self.assertEquals("bar(2)", repr(IdName(2, "bar")))

    def test_equals(self):
        self.assertEquals(IdName(1,"foo"), IdName(1, "foo"))
        self.assertEquals(IdName(1,"bar"), IdName(1, "bar"))
        self.assertEquals(IdName(2,"foo"), IdName(2, "foo"))
        self.assertEquals(IdName(2,"bar"), IdName(2, "bar"))

        self.assertNotEquals(IdName(1,"foo"), IdName(2,"foo"))  # id changed
        self.assertNotEquals(IdName(1,"foo"), IdName(1,"bar"))  # name changed
        self.assertNotEquals(IdName(1,"foo"), IdName(2,"bar"))  # id and name changed

        self.assertNotEquals(IdName(1,"foo"), None)
        self.assertNotEquals(IdName(1,"foo"), 1)
        self.assertNotEquals(IdName(1,"foo"), True)

    def test_hashCodeObject(self):

        self.assertEquals(id(IdName(1, "foo")), id(IdName(1,"foo")))
        self.assertEquals(id(IdName(1, "bar")), id(IdName(1,"bar")))
        self.assertEquals(id(IdName(2, "foo")), id(IdName(2,"foo")))
        self.assertEquals(id(IdName(2, "bar")), id(IdName(2,"bar")))

    def test_hash(self):

        self.assertEquals(  1511848646, IdName.hash( "ab" ) )
        self.assertEquals(   669497117, IdName.hash( "abc" ) )
        self.assertEquals( -1994190713, IdName.hash( "abcd" ) )
        self.assertEquals(  -802680418, IdName.hash( "abcde" ) )
        self.assertEquals(  1266308680, IdName.hash( "abcdef" ) )
        self.assertEquals(  -379372513, IdName.hash( "abcdefg" ) )
        self.assertEquals( -1416967159, IdName.hash( "abcdefgh" ) )
        self.assertEquals(    53556896, IdName.hash( "abcdefghi" ) )
        self.assertEquals(    -4427318, IdName.hash( "abcdefghij" ) )

if __name__ == '__main__':
    unittest.main()
