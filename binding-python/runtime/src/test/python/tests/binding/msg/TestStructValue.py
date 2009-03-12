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
from etch.binding.msg.StructValue import *
from etch.binding.msg.Type import Type
from etch.binding.msg.Field import Field
from etch.binding.msg.IdName import IdName
#from etch.binding.support.ComboValidator import ComboValidator
from etch.binding.support.Validator_int import Validator_int
from etch.binding.support.Validator_boolean import Validator_boolean
from etch.binding.support.Validator_string import Validator_string

class Test_EtchMsgStructValue(unittest.TestCase):

    def setUp(self):
        #print " "
        pass

        mt1 = Type("one")
        mt2 = Type("two")

        mf1 = Field("f1")
        mf2 = Field("f2")
        mf3 = Field("f3")
        mf4 = Field("f4")
        mf5 = Field("f5")
        mf6 = Field("f6")

        mt1.putValidator(mf1, Validator_boolean.get(0))
        mt1.putValidator(mf2, Validator_boolean.get(1))
        mt1.putValidator(mf3, Validator_int.get(0))
        mt1.putValidator(mf4, Validator_int.get(1))
        mt1.putValidator(mf5, Validator_string.get(0))
        mt1.putValidator(mf6, Validator_string.get(1))

        self._mt1 = mt1
        self._mt2 = mt2
        self._mf1 = mf1
        self._mf2 = mf2
        self._mf3 = mf3
        self._mf4 = mf4
        self._mf5 = mf5
        self._mf6 = mf6

    def test_toString(self):

        sv = StructValue(self._mt1)
        self.assertEquals("one(785945377): {}", repr(sv))

        sv = StructValue(self._mt2)
        self.assertEquals("two(827843303): {}", repr(sv))

        sv = StructValue(self._mt1)
        sv.put(self._mf1, True)
        self.assertEquals("one(785945377): {f1(1512176592): True}", repr(sv))

        sv = StructValue(self._mt1)
        sv.put(self._mf3, 23)
        self.assertEquals("one(785945377): {f3(1512176594): 23}", repr(sv))

        sv = StructValue(self._mt1)
        sv.put(self._mf1, False)
        sv.put(self._mf3, 74)
        self.assertEqual(True, "one(785945377): {f1(1512176592): False, f3(1512176594): 74}" == repr(sv) or "one(785945377): {f3(1512176594): 74, f1(1512176592): False}" == repr(sv) )

    def test_getType(self):
        sv = StructValue(self._mt1)
        self.assertEqual(self._mt1, sv.type())

        sv = StructValue(self._mt2)
        self.assertEqual(self._mt2, sv.type())

    def test_isType(self):
        sv = StructValue(self._mt1)
        self.assertEqual(True, sv.isType(self._mt1))
        self.assertEqual(False, sv.isType(self._mt2))

        sv = StructValue(self._mt2)
        self.assertEqual(True, sv.isType(self._mt2))
        self.assertEqual(False, sv.isType(self._mt1))

    def test_checkType1(self):
        StructValue(self._mt1).checkType(self._mt1)
        StructValue(self._mt2).checkType(self._mt2)

    def test_checkType2(self):
        self.assertRaises(IllegalArgumentException, StructValue(self._mt1).checkType, self._mt2)

    def test_checkType3(self):
        self.assertRaises(IllegalArgumentException, StructValue(self._mt2).checkType, self._mt1)

    def test_get(self):
        sv = StructValue(self._mt1)
        self.assertEqual(True, sv.isEmpty())
        self.assertEqual(0, sv.size())
        self.assertEqual(None, sv.get(self._mf1))

        sv.put(self._mf1, True)
        self.assertEqual(False, sv.isEmpty())
        self.assertEqual(1, sv.size())
        self.assertEqual(True, sv.get(self._mf1))

        sv.put(self._mf1, False)
        self.assertEqual(False, sv.isEmpty())
        self.assertEqual(1, sv.size())
        self.assertEqual(False, sv.get(self._mf1))

        sv.put(self._mf1, None)
        self.assertEqual(True, sv.isEmpty())
        self.assertEqual(0, sv.size())
        self.assertEqual(None, sv.get(self._mf1))

    def test_remove(self):
        sv = StructValue(self._mt1)
        self.assertEqual(True, sv.isEmpty())
        self.assertEqual(0, sv.size())
        self.assertEqual(None, sv.get(self._mf1))

        sv.remove(self._mf1)
        self.assertEqual(True, sv.isEmpty())
        self.assertEqual(0, sv.size())
        self.assertEqual(None, sv.get(self._mf1))

        sv.put(self._mf1, True)
        self.assertEqual(False, sv.isEmpty())
        self.assertEqual(1, sv.size())
        self.assertEqual(True, sv.get(self._mf1))

        sv.remove(self._mf1)
        self.assertEqual(True, sv.isEmpty())
        self.assertEqual(0, sv.size())
        self.assertEqual(None, sv.get(self._mf1))

        sv.remove(self._mf1)
        self.assertEqual(True, sv.isEmpty())
        self.assertEqual(0, sv.size())
        self.assertEqual(None, sv.get(self._mf1))

    def test_put1(self):
        sv = StructValue(self._mt1)
        sv.put(self._mf1, False)
        sv.put(self._mf2, [True, False])
        sv.put(self._mf3, 1)
        sv.put(self._mf4, [1,2])
        sv.put(self._mf5, "a")
        sv.put(self._mf6, ["a", "b"])
        self.assertEqual(6, sv.size())

    def test_put2(self):
        sv = StructValue(self._mt1)
        sv.put(self._mf1, None)
        sv.put(self._mf2, None)
        sv.put(self._mf3, None)
        sv.put(self._mf4, None)
        sv.put(self._mf5, None)
        sv.put(self._mf6, None)
        self.assertEqual(0, sv.size())
    
    def test_put3(self):
        sv = StructValue(self._mt1)
        sv.put(self._mf1, False)
        sv.put(self._mf2, [True, False])
        sv.put(self._mf3, 1)
        sv.put(self._mf4, [1,2])
        sv.put(self._mf5, "a")
        sv.put(self._mf6, ["a", "b"])
        self.assertEqual(6, sv.size())
        
        # Now remove them
        sv.put(self._mf1, None)
        sv.put(self._mf2, None)
        sv.put(self._mf3, None)
        sv.put(self._mf4, None)
        sv.put(self._mf5, None)
        sv.put(self._mf6, None)
        self.assertEqual(0, sv.size())
    
    def test_put5(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf1, Byte(1))
    
    def test_put6(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf1, Short(1))
        
    def test_put7(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf1, 1)
    
    def test_put8(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf1, 1L)
    
    def test_put9(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf1, 1.2)
    
    def test_put10(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf1, Float(1.2))
    
    def test_put11(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf1, "a")
    
    def test_put12(self):
        sv = StructValue(self._mt1)
        sv.put(self._mf2, None)
        
    def test_put13(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf2, [])
    
    def test_put14(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf2, True)
    
    def test_put15(self):
        sv = StructValue(self._mt1)
        self.assertRaises(IllegalArgumentException, sv.put, self._mf2, [[]])
    
    def test_iterator(self):
        # TODO - implement test_iterator
        pass
        
if __name__ == '__main__':
    unittest.main()
