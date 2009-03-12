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
import types
from etch.python.Types import *
from etch.binding.support.Validator_boolean import *
from etch.binding.msg.Validator import *
from etch.binding.transport.fmt.TypeCode import *

class Test_EtchSupportValidator_boolean(unittest.TestCase):

    def test_validator(self):
        _test = self._test

        # Good scalars
        _test(0, "boolean[0]", TypeCode.BOOLEAN_FALSE, Boolean,           False,          "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_FALSE, types.BooleanType, False,          "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_FALSE, Boolean,           Boolean(False), "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_FALSE, types.BooleanType, Boolean(False), "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  Boolean,           True,           "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, True,           "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  Boolean,           Boolean(True),  "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, Boolean(True),  "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, 1,              "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_FALSE, types.BooleanType, 0,              "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, 1.0,            "1" )
        _test(0, "boolean[0]", TypeCode.BOOLEAN_FALSE, types.BooleanType, 0.0,            "1" )

        # Bad scalars
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, True, 1.1)
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, True, Byte(2))
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, True, Short(3))
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, True, Integer(4))
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, True, Long(5))
        _test(0, "boolean[0]", TypeCode.BOOLEAN_TRUE,  types.BooleanType, True, "6")

        # Good, Empty Array's
        _test(1, "boolean[1]", TypeCode.ARRAY, types.TupleType, list(),              True)
        _test(1, "boolean[1]", TypeCode.ARRAY, types.TupleType, tuple(),             True)

        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  list(),              True)
        _test(2, "boolean[2]", TypeCode.ARRAY, types.ListType,  [list()],            True)
        _test(3, "boolean[3]", TypeCode.ARRAY, types.ListType,  [[list()]],          True)
        _test(4, "boolean[4]", TypeCode.ARRAY, types.ListType,  [[[list()]]],        True)
        _test(5, "boolean[5]", TypeCode.ARRAY, types.ListType,  [[[[list()]]]],      True)

        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  tuple(),             True)
        _test(2, "boolean[2]", TypeCode.ARRAY, types.ListType,  (tuple(),),          True)
        _test(3, "boolean[3]", TypeCode.ARRAY, types.ListType,  ((tuple(),),),       True)
        _test(4, "boolean[4]", TypeCode.ARRAY, types.ListType,  (((tuple(),),),),    True)
        _test(5, "boolean[5]", TypeCode.ARRAY, types.ListType,  ((((tuple(),),),),), True)

        # Bad, Empty Array's
        _test(2, "boolean[2]", TypeCode.ARRAY, types.ListType,  [list()],            list())
        _test(3, "boolean[3]", TypeCode.ARRAY, types.ListType,  [[list()]],          [list()])
        _test(4, "boolean[4]", TypeCode.ARRAY, types.ListType,  [[[list()]]],        [[list()]])
        _test(5, "boolean[5]", TypeCode.ARRAY, types.ListType,  [[[[list()]]]],      [[[list()]]])

        _test(2, "boolean[2]", TypeCode.ARRAY, types.ListType,  (tuple(),),          tuple())
        _test(3, "boolean[3]", TypeCode.ARRAY, types.ListType,  ((tuple(),),),       (tuple(),))
        _test(4, "boolean[4]", TypeCode.ARRAY, types.ListType,  (((tuple(),),),),    ((tuple(),),))
        _test(5, "boolean[5]", TypeCode.ARRAY, types.ListType,  ((((tuple(),),),),), (((tuple(),),),))

        # Good, Non-empty Array's
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,False,True), True)
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,0,1),        True)
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,False,1),    True)
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (1,False,True),    True)
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (1,0,True),        True)
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (1,False,1),       True)
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,0,True),     True)
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (1,0,1),           True)

        # Bad, Non-empty Array's
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,False,True), (True,"0","1"))
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,False,True), (True,False,"1"))
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,False,True), ("1",False,True))
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,False,True), ("1","0",True))
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,False,True), ("1",False,"1"))
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,False,True), (True,"0",True))
        _test(1, "boolean[1]", TypeCode.ARRAY, types.ListType,  (True,False,True), ("1","0","1"))

    def test_getNeg1(self):
        self.assertRaises(IllegalArgumentException, Validator_boolean.get, -1)

    def test_getNeg2(self):
        self.assertRaises(IllegalArgumentException, Validator_boolean.get(0).elementValidator)

    def test_getMaxPlusOne(self):
        self.assertRaises(IllegalArgumentException, Validator_boolean.get, Validator.MAX_NDIMS + 1)

    def _test(self, n, s, tc, clss, good, bad):
        v = Validator_boolean.get(n)

        self.assertEqual(n, v.getNDims())
        if isinstance(v.getExpectedClass(), (types.TupleType, types.ListType)):
            a = False
            for x in v.getExpectedClass():
                if clss == x:
                    a = True
            self.assertEqual(True, a, "Validator not catching expected class")
        else:
            self.assertEqual(clss, v.getExpectedClass(), "Validator not catching expected class")

        self.assertEqual(s, repr(v))
        self.assertEqual(True, v.validate(good))
        self.assertEqual(False, v.validate(bad))
        self.assertEqual(tc, v.checkValue(good))
        self.assertEqual(None, v.checkValue(bad))
        if n > 0:
            self.assertEqual(n-1, v.elementValidator().getNDims())

if __name__=='__main__':
    unittest.main()

