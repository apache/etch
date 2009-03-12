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
from etch.binding.support.ComboValidator import *
from etch.binding.support.Validator_boolean import *
from etch.binding.support.Validator_int import *
from etch.binding.transport.fmt.TypeCode import *

class Test_EtchSupportComboValidator(unittest.TestCase):

    def test_1(self):
        _test = self._test

        _test(0, "(int[0] or boolean[0]", TypeCode.BOOLEAN_TRUE,  True,        Byte(1))
        _test(0, "(int[0] or boolean[0]", TypeCode.BOOLEAN_FALSE, False,       Short(2))
        _test(0, "(int[0] or boolean[0]", TypeCode.INT1,          0,           Long(3))
        _test(0, "(int[0] or boolean[0]", TypeCode.INT1,          1,           Float(4.0))
        _test(0, "(int[0] or boolean[0]", TypeCode.INT1,          -1,          Double(5.0))
        _test(0, "(int[0] or boolean[0]", TypeCode.INT1,          127,         "abc")
        _test(0, "(int[0] or boolean[0]", TypeCode.INT1,          -128,        object())
        _test(0, "(int[0] or boolean[0]", TypeCode.INT2,          32767,       [])
        _test(0, "(int[0] or boolean[0]", TypeCode.INT2,          -32768,      [])
        _test(0, "(int[0] or boolean[0]", TypeCode.INT4,          1000000000,  object())
        _test(0, "(int[0] or boolean[0]", TypeCode.INT4,          -1000000000, Integer)
        _test(1, "(int[1] or boolean[1]", TypeCode.ARRAY,         [],          0)
        _test(1, "(int[1] or boolean[1]", TypeCode.ARRAY,         [],          True)
        _test(1, "(int[1] or boolean[1]", TypeCode.ARRAY,         (),          0)
        _test(1, "(int[1] or boolean[1]", TypeCode.ARRAY,         (),          True)

    def test_2(self):
        v1 = Validator_int.get(1)
        v2 = Validator_boolean.get(0)
        v  = ComboValidator(v1, v2)

        self.assertEqual(True, v.validate([]))
        self.assertEqual(True, v.validate(True))
        self.assertEqual(True, v.validate(False))
        self.assertEqual(False, v.validate(0))

        v = v.elementValidator()

        self.assertEqual(False, v.validate([]))
        self.assertEqual(False, v.validate(True))
        self.assertEqual(False, v.validate(False))
        self.assertEqual(True,  v.validate(0))

    def test_3(self):
        v1 = Validator_boolean.get(0)
        v2 = Validator_int.get(1)
        v  = ComboValidator(v1, v2)

        self.assertEqual(True, v.validate([]))
        self.assertEqual(True, v.validate(True))
        self.assertEqual(True, v.validate(False))
        self.assertEqual(False, v.validate(0))

        v = v.elementValidator()

        self.assertEqual(False, v.validate([]))
        self.assertEqual(False, v.validate(True))
        self.assertEqual(False, v.validate(False))
        self.assertEqual(True, v.validate(0))

    def test_4(self):
        v1 = Validator_boolean.get(0)
        v2 = Validator_int.get(1)
        v  = ComboValidator(v1, v2)

        # TODO - fix these, maybe use boolean and string-array?

        self.assertEqual(True,  v.validate([]))
        self.assertEqual(True,  v.validate(True))
        self.assertEqual(True,  v.validate(False))
        self.assertEqual(False, v.validate(-7))

        v = v.elementValidator()

        self.assertEqual(False, v.validate([]))
        self.assertEqual(False, v.validate(True))
        self.assertEqual(False, v.validate(False))
        self.assertEqual(True,  v.validate(-7))

        self.assertRaised(IllegalArgumentException, v.elementValidator)

    def test_5(self):
        v1 = Validator_boolean.get(1)
        v2 = Validator_int.get(1)
        v  = ComboValidator(v1, v2)

        self.assertEqual(True, v.validate([]))
        self.assertEqual(True, v.validate(()))

        v = v.elementValidator()

        self.assertEqual(True, v.validate(True))
        self.assertEqual(True, v.validate(False))
        self.assertEqual(True, v.validate(7))

        self.assertRaised(IllegalArgumentException, v.elementValidator)

    def _test(self, n, s, tc, good, bad):

        v1 = Validator_int.get(n)
        v2 = Validator_boolean.get(n)
        v = ComboValidator(v1, v2)

        self.assertEqual(s, repr(v))
        self.assertEqual(True, v.validate(good))
        self.assertEqual(False, v.validate(bad))
        self.assertEqual(tc, v.checkValue(good))
        self.assertEqual(None, v.checkValue(bad))
        if n > 0:
            v.elementValidator()

if __name__ == '__main__':
    unittest.main()

