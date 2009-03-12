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

class Test_EtchUtilTypes(unittest.TestCase):

    def setUp(self):
        print " "


    def test_byte_sizes(self):
        a = Byte(0)

        # +/- 128 boundary
        self.assertEquals( 127, a+127)
        self.assertEquals(-128, a+128)
        self.assertEquals(-127, a+129)

        self.assertEquals(-127, a-127)
        self.assertEquals(-128, a-128)
        self.assertEquals( 127, a-129)

        # +/- 256 boundary
        self.assertEquals(-1, a+255)
        self.assertEquals( 0, a+256)
        self.assertEquals( 1, a+257)

        self.assertEquals( 1, a-255)
        self.assertEquals( 0, a-256)
        self.assertEquals(-1, a-257)

        # +/- 384 boundary
        self.assertEquals( 127, a+383)
        self.assertEquals(-128, a+384)
        self.assertEquals(-127, a+385)

        self.assertEquals(-127, a-383)
        self.assertEquals(-128, a-384)
        self.assertEquals( 127, a-385)

        # +/- 512 boundary
        self.assertEquals(-1, a+511)
        self.assertEquals( 0, a+512)
        self.assertEquals( 1, a+513)

        self.assertEquals( 1, a-511)
        self.assertEquals( 0, a-512)
        self.assertEquals(-1, a-513)

    def test_computehash(self):
        test_keys = (
                     ('_mt__Etch_RuntimeException', 1071849665),
                     ('foo', 711657441 ),
                     ('bar', 677693038 ),
                     ('ZZZZZZZZZZZZZZZZZZZZ', -1181680379),
                     )
        for t in test_keys:
            print "  IdName.hash('%s') -> %d" % (t[0], t[1])
            self.assertEquals(t[1], IdName.hash(t[0]))

if __name__ == '__main__':
    unittest.main()
