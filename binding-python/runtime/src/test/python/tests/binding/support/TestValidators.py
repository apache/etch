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
from etch.binding.msg.Validator import *
from etch.binding.support.Validator_boolean import *
from etch.binding.support.Validator_int import *
from etch.binding.support.Validator_short import *
from etch.binding.support.Validator_byte import *
from etch.binding.support.Validator_string import *
from etch.binding.transport.fmt.TypeCode import *

class Test_EtchSupportValidators(unittest.TestCase):

    def test_boolean(self):
        _test = self._test

        # Check list
        _test(Validator_boolean.get(2), 2, [TypeCode.BOOLEAN_FALSE, TypeCode.BOOLS, TypeCode.ARRAY], [[False, False], [False, False]])
        _test(Validator_boolean.get(2), 2, [TypeCode.BOOLEAN_TRUE, TypeCode.BOOLS, TypeCode.ARRAY], [[True, True], [True,True]])

        # Check tuple
        _test(Validator_boolean.get(2), 2, [TypeCode.BOOLEAN_FALSE, TypeCode.BOOLS, TypeCode.ARRAY], ((False, False), (False, False)))
        _test(Validator_boolean.get(2), 2, [TypeCode.BOOLEAN_TRUE, TypeCode.BOOLS, TypeCode.ARRAY], ((True, True), (True,True)))

    def test_byte(self):
        _test = self._test

        _test(Validator_byte.get(2), 2, [TypeCode.BYTE, TypeCode.BYTES, TypeCode.ARRAY], [[0,0],[0,0]])
        _test(Validator_byte.get(2), 2, [TypeCode.BYTE, TypeCode.BYTES, TypeCode.ARRAY], [[1,1],[1,1]])
        _test(Validator_byte.get(2), 2, [TypeCode.BYTE, TypeCode.BYTES, TypeCode.ARRAY], [[-1,-1],[1,1]])

        _test(Validator_byte.get(2), 2, [TypeCode.BYTE, TypeCode.BYTES, TypeCode.ARRAY], [[Byte(0),Byte(0)],[Byte(0),Byte(0)]])
        _test(Validator_byte.get(2), 2, [TypeCode.BYTE, TypeCode.BYTES, TypeCode.ARRAY], [[Byte(1),Byte(1)],[Byte(1),Byte(1)]])
        _test(Validator_byte.get(2), 2, [TypeCode.BYTE, TypeCode.BYTES, TypeCode.ARRAY], [[Byte(-1),Byte(-1)],[Byte(1),Byte(1)]])

        _test(Validator_byte.get(2), 2, [TypeCode.BYTE, TypeCode.BYTES, TypeCode.ARRAY], [[Byte.MAX_VALUE, Byte.MAX_VALUE],[Byte.MAX_VALUE,Byte.MAX_VALUE]])
        _test(Validator_byte.get(2), 2, [TypeCode.BYTE, TypeCode.BYTES, TypeCode.ARRAY], [[Byte.MIN_VALUE, Byte.MIN_VALUE],[Byte.MIN_VALUE,Byte.MIN_VALUE]])

    def test_short(self):
        _test = self._test

        _test(Validator_short.get(2), 2, [TypeCode.BYTE, TypeCode.SHORTS, TypeCode.ARRAY], [[0,0],[0,0]])
        _test(Validator_short.get(2), 2, [TypeCode.BYTE, TypeCode.SHORTS, TypeCode.ARRAY], [[1,1],[1,1]])
        _test(Validator_short.get(2), 2, [TypeCode.BYTE, TypeCode.SHORTS, TypeCode.ARRAY], [[-1,-1],[-1,-1]])

        _test(Validator_short.get(2), 2, [TypeCode.BYTE, TypeCode.SHORTS, TypeCode.ARRAY], [[Byte.MAX_VALUE, Byte.MAX_VALUE],[Byte.MAX_VALUE, Byte.MAX_VALUE]])
        _test(Validator_short.get(2), 2, [TypeCode.BYTE, TypeCode.SHORTS, TypeCode.ARRAY], [[Byte.MIN_VALUE, Byte.MIN_VALUE],[Byte.MIN_VALUE, Byte.MIN_VALUE]])

        _test(Validator_short.get(2), 2, [TypeCode.SHORT, TypeCode.SHORTS, TypeCode.ARRAY], [[Short.MAX_VALUE, Short.MAX_VALUE],[Short.MAX_VALUE, Short.MAX_VALUE]])
        _test(Validator_short.get(2), 2, [TypeCode.SHORT, TypeCode.SHORTS, TypeCode.ARRAY], [[Short.MIN_VALUE, Short.MIN_VALUE],[Short.MIN_VALUE, Short.MIN_VALUE]])

    def test_int(self):
        _test = self._test

        _test(Validator_int.get(2), 2, [TypeCode.BYTE, TypeCode.INTS, TypeCode.ARRAY], [[0,0],[0,0]])
        _test(Validator_int.get(2), 2, [TypeCode.BYTE, TypeCode.INTS, TypeCode.ARRAY], [[1,1],[1,1]])
        _test(Validator_int.get(2), 2, [TypeCode.BYTE, TypeCode.INTS, TypeCode.ARRAY], [[-1,-1],[-1,-1]])

        _test(Validator_int.get(2), 2, [TypeCode.BYTE, TypeCode.INTS, TypeCode.ARRAY], [[Byte.MAX_VALUE, Byte.MAX_VALUE],[Byte.MAX_VALUE, Byte.MAX_VALUE]])
        _test(Validator_int.get(2), 2, [TypeCode.BYTE, TypeCode.INTS, TypeCode.ARRAY], [[Byte.MIN_VALUE, Byte.MIN_VALUE],[Byte.MIN_VALUE, Byte.MIN_VALUE]])

        _test(Validator_int.get(2), 2, [TypeCode.SHORT, TypeCode.INTS, TypeCode.ARRAY], [[Short.MAX_VALUE, Short.MAX_VALUE],[Short.MAX_VALUE, Short.MAX_VALUE]])
        _test(Validator_int.get(2), 2, [TypeCode.SHORT, TypeCode.INTS, TypeCode.ARRAY], [[Short.MIN_VALUE, Short.MIN_VALUE],[Short.MIN_VALUE, Short.MIN_VALUE]])

        _test(Validator_int.get(2), 2, [TypeCode.INT, TypeCode.INTS, TypeCode.ARRAY], [[Integer.MAX_VALUE, Integer.MAX_VALUE],[Integer.MAX_VALUE, Integer.MAX_VALUE]])
        _test(Validator_int.get(2), 2, [TypeCode.INT, TypeCode.INTS, TypeCode.ARRAY], [[Integer.MIN_VALUE, Integer.MIN_VALUE],[Integer.MIN_VALUE, Integer.MIN_VALUE]])

    def test_long(self):
        _test = self._test

        _test(Validator_long.get(2), 2, [TypeCode.BYTE, TypeCode.LONGS, TypeCode.ARRAY], [[0,0],[0,0]])
        _test(Validator_long.get(2), 2, [TypeCode.BYTE, TypeCode.LONGS, TypeCode.ARRAY], [[1,1],[1,1]])
        _test(Validator_long.get(2), 2, [TypeCode.BYTE, TypeCode.LONGS, TypeCode.ARRAY], [[-1,-1],[-1,-1]])

        _test(Validator_long.get(2), 2, [TypeCode.BYTE, TypeCode.LONGS, TypeCode.ARRAY], [[Byte.MAX_VALUE, Byte.MAX_VALUE],[Byte.MAX_VALUE, Byte.MAX_VALUE]])
        _test(Validator_long.get(2), 2, [TypeCode.BYTE, TypeCode.LONGS, TypeCode.ARRAY], [[Byte.MIN_VALUE, Byte.MIN_VALUE],[Byte.MIN_VALUE, Byte.MIN_VALUE]])

        _test(Validator_long.get(2), 2, [TypeCode.SHORT, TypeCode.LONGS, TypeCode.ARRAY], [[Short.MAX_VALUE, Short.MAX_VALUE],[Short.MAX_VALUE, Short.MAX_VALUE]])
        _test(Validator_long.get(2), 2, [TypeCode.SHORT, TypeCode.LONGS, TypeCode.ARRAY], [[Short.MIN_VALUE, Short.MIN_VALUE],[Short.MIN_VALUE, Short.MIN_VALUE]])

        _test(Validator_long.get(2), 2, [TypeCode.INT, TypeCode.LONGS, TypeCode.ARRAY], [[Integer.MAX_VALUE, Integer.MAX_VALUE],[Integer.MAX_VALUE, Integer.MAX_VALUE]])
        _test(Validator_long.get(2), 2, [TypeCode.INT, TypeCode.LONGS, TypeCode.ARRAY], [[Integer.MIN_VALUE, Integer.MIN_VALUE],[Integer.MIN_VALUE, Integer.MIN_VALUE]])

        _test(Validator_long.get(2), 2, [TypeCode.INT, TypeCode.LONGS, TypeCode.ARRAY], [[Long.MAX_VALUE, Long.MAX_VALUE],[Long.MAX_VALUE, Long.MAX_VALUE]])
        _test(Validator_long.get(2), 2, [TypeCode.INT, TypeCode.LONGS, TypeCode.ARRAY], [[Long.MIN_VALUE, Long.MIN_VALUE],[Long.MIN_VALUE, Long.MIN_VALUE]])

    def test_float(self):
        _test = self._test

        _test(Validator_float.get(2), 2, [TypeCode.FLOAT, TypeCode.FLOATS, TypeCode.ARRAY], [[0,0],[0,0]])
        _test(Validator_float.get(2), 2, [TypeCode.FLOAT, TypeCode.FLOATS, TypeCode.ARRAY], [[1,1],[1,1]])
        _test(Validator_float.get(2), 2, [TypeCode.FLOAT, TypeCode.FLOATS, TypeCode.ARRAY], [[-1,-1],[-1,-1]])
        _test(Validator_float.get(2), 2, [TypeCode.FLOAT, TypeCode.FLOATS, TypeCode.ARRAY], [[Float.MAX_VALUE, Float.MAX_VALUE],[Float.MAX_VALUE, Float.MAX_VALUE]])
        _test(Validator_float.get(2), 2, [TypeCode.FLOAT, TypeCode.FLOATS, TypeCode.ARRAY], [[Float.MIN_VALUE, Float.MIN_VALUE],[Float.MIN_VALUE, Float.MIN_VALUE]])

    def test_double(self):
        _test = self._test

        _test(Validator_double.get(2), 2, [TypeCode.DOUBLE, TypeCode.DOUBLES, TypeCode.ARRAY], [[0,0],[0,0]])
        _test(Validator_double.get(2), 2, [TypeCode.DOUBLE, TypeCode.DOUBLES, TypeCode.ARRAY], [[1,1],[1,1]])
        _test(Validator_double.get(2), 2, [TypeCode.DOUBLE, TypeCode.DOUBLES, TypeCode.ARRAY], [[-1,-1],[-1,-1]])
        _test(Validator_double.get(2), 2, [TypeCode.DOUBLE, TypeCode.DOUBLES, TypeCode.ARRAY], [[Double.MAX_VALUE, Double.MAX_VALUE],[Double.MAX_VALUE, Double.MAX_VALUE]])
        _test(Validator_double.get(2), 2, [TypeCode.DOUBLE, TypeCode.DOUBLES, TypeCode.ARRAY], [[Double.MIN_VALUE, Double.MIN_VALUE],[Double.MIN_VALUE, Double.MIN_VALUE]])

    def test_string(self):
        _test = self._test

        _test(Validator_string.get(2), 2, [TypeCode.EMPTY_STRING, TypeCode.ARRAY, TypeCode.ARRAY], [["",""],["",""]])
        _test(Validator_string.get(2), 2, [TypeCode.STRING, TypeCode.ARRAY, TypeCode.ARRAY], [["abc","abc"], ["abc","abc"]])

    def test_custom(self):
        _test = self._test

        _test(Validator_custom.get( Date, 2), 2, [TypeCode.CUSTOM, TypeCode.ARRAY, TypeCode.ARRAY], [[Date(), Date()], [Date(),Date()]])


    def _test(self, v, n, tcs, o):
        self.assertEqual(True, v.validate(o))
        self.assertEqual(tcs[n], v.checkValue(o))

        if n > 0:
            for k in o:
                self._test(v.elementValidator(), n-1, tcs, k)

if __name__ == '__main__':
    unittest.main()

