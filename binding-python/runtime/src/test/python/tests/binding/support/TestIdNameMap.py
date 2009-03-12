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
from etch.binding.support.IdNameMap import *
from etch.binding.msg.IdName import *

class IdNameMapTest(IdNameMap):

    @staticmethod
    def makeNew(name):
        return IdName(name)


class Test_EtchSupportIdNameMap(unittest.TestCase):

    def setUp(self):
        self.a = IdName(1, "a")
        self.b = IdName(2, "b")
        self.map = IdNameMapTest()
        self.map.add(self.a)
        self.map.add(self.b)

    def test_check_a(self):
        self.assertEqual(2, self.map.size())
        self.check("a", 1, self.a)
        self.assertEqual(2, self.map.size())

    def test_check_b(self):
        self.assertEqual(2, self.map.size())
        self.check("b", 2, self.b)
        self.assertEqual(2, self.map.size())

    def test_get_3(self):
        self.assertEqual(2, self.map.size())
        x = self.map.get(3)
        self.assertEqual(None, x)
        self.assertEqual(2, self.map.size())

    def test_get_3_locked(self):
        self.assertEqual(2, self.map.size())
        self.map.lock()
        x = self.map.get(3)
        self.assertEqual(None, x)
        self.assertEqual(2, self.map.size())

    def test_get_c(self):
        self.assertEqual(2, self.map.size())
        c = self.map.get("c")
        self.check("c", 352988318, c)
        self.assertEqual(3, self.map.size())

    def test_get_d_locked(self):
        self.assertEqual(2, self.map.size())
        self.map.lock()
        d = self.map.get("d")
        self.assertEqual(None, d)
        self.assertEqual(2, self.map.size())

    def test_add_3_c_locked(self):
        self.assertEqual(2, self.map.size())
        self.map.lock()
        self.assertRaises(IllegalStateException, self.map.add, IdName(3, "c"))
        self.assertEquals(2, self.map.size())

    def test_add_id_coll(self):
        self.assertEqual(2, self.map.size())
        self.assertRaises(IllegalArgumentException, self.map.add, IdName(1,"c"))
        self.assertEquals(2, self.map.size())

    def test_add_name_coll(self):
        self.assertEqual(2, self.map.size())
        self.assertRaises(IllegalArgumentException, self.map.add, IdName(1,"a"))
        self.assertEqual(2, self.map.size())

    def test_add_id_name_coll(self):
        self.assertEqual(2, self.map.size())
        self.assertRaises(IllegalArgumentException, self.map.add, IdName(1, "a"))
        self.assertEqual(2, self.map.size())

    def test_values1(self):
        actual = self.map.values()
        self.assertEqual(2, len(actual))
        expected = []
        expected.append(IdName(1, "a"))
        expected.append(IdName(2, "b"))
        self.assertEqual(expected, actual)

        self.map.get("c")

        actual = self.map.values()
        self.assertEqual(3, actual.size())
        expected = []
        expected.append(IdName(1, "a"))
        expected.append(IdName(2, "b"))
        expected.append(IdName("c"))
        self.assertEqual(expected, actual)

    def test_values2(self):
        # code in value factory depends upon the fact that the set
        # returned from .values is a copy
        self.assertNotEquals(id(self.map.values()), id(self.map.values()))


    def check(self, name, id, idName):
        x = self.map.get(id)
        self.assertNotEqual(None, x)
        self.assertEqual(idName, x)
        self.assertEqual(id, x.getId())
        self.assertEqual(name, x.getName())

        x = self.map.get(name)
        self.assertNotEqual(None, x)
        self.assertEqual(idName, x)
        self.assertEqual(id, x.getId())
        self.assertEqual(name, x.getName())

if __name__ == '__main__':
    unittest.main()
