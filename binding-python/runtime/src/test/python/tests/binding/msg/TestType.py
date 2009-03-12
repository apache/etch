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
from etch.binding.msg.Type import Type
from etch.binding.msg.Field import Field
from etch.binding.msg.IdName import IdName
from etch.binding.support.ComboValidator import ComboValidator
from etch.binding.support.Validator_int import Validator_int

class Test_EtchMsgType(unittest.TestCase):

    def setUp(self):
        #print " "
        pass

    def test_typeIntegerString(self):
        self._testMt(1, "one")
        self._testMt(2, "two")
        self._testMt(3, "three")

    def test_typeString(self):
        self._testMt("one")
        self._testMt("two")
        self._testMt("three")

    def test_runValidators(self):
        b = Type.getRunValidators()
        Type.setRunValidators(True)
        self.assertEqual(Type.getRunValidators(), True)

        Type.setRunValidators(False)
        self.assertEqual(Type.getRunValidators(), False)

        Type.setRunValidators(b)

    def test_getValidator(self):
        t = Type("t")
        f = Field("f")
        g = Field("g")
        v = Validator_int.get(0)

        t.putValidator(f, v)
        Type.setRunValidators(False)
        self.assertEqual(t.getValidator(f),None)

        Type.setRunValidators(True)
        self.assertEqual(v, t.getValidator(f))

        Type.setRunValidators(False)
        self.assertEqual(t.getValidator(g), None)

        Type.setRunValidators(True)
        self.assertEqual(t.getValidator(g), None)

    def test_putValidatorNull(self):
        t = Type("t")
        f = Field("f")
        v = Validator_int.get(0)

        Type.setRunValidators(True)
        self.assertEqual(t.getValidator(f), None)

        t.putValidator(f, None)
        self.assertEqual(t.getValidator(f), None)

        t.putValidator(f, v)
        self.assertEqual(v, t.getValidator(f))

        t.putValidator(f, v)
        self.assertEqual(ComboValidator, t.getValidator(f).__class__)

        t.clearValidator(f)
        self.assertEqual(t.getValidator(f), None)

    def _testMt(self, *args):
        if len(args) == 1:
            n = args[0]
            i = IdName.hash(n)
        elif len(args) == 2:
            i = args[0]
            n = args[1]
        else:
            raise Exception, "Bad number of arguments"

        mt = Type(i,n)
        self.assertEquals(i, mt.getId())
        self.assertEquals(n, mt.getName())

if __name__ == '__main__':
    unittest.main()
