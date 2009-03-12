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
from etch.binding.msg.Type import *

# TODO - finish this

class Test_EtchTransportArrayValue(unittest.TestCase):

    def test_constructor1(self):
        av = ArrayValue(0)
        self.assertEqual(0, av.typeCode())
        self.assertEqual(None, av.customStructType())
        self.assertEqual(0, av.dim())

    def test_constructor2(self):
        t1 = Type("t1")
        av = ArrayValue(0, 1, t1, 2)
        self.assertEqual(1, av.typeCode())
        self.assertEqual(t1, av.customStructType())
        self.assertEqual(2, av.dim())

if __name__ == '__main__':
    unittest.main()
