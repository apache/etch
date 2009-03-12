"""
# Copyright 2007-2008 Cisco Systems Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy
# of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#
"""
import unittest
from etch.util.Types import *
from etch.msg.Validator import *
from etch.support.Validator_int import *
from etch.transport.fmt.TypeCode import *

class Test_EtchSupportValidator_int(unittest.TestCase):
        
    def _test_validator(self):
        _test = self._test
        
        # TODO - resolve casting issues in validator tests
        _test(0, "int[0]", TypeCode.INT1, Integer,            Integer(Byte.MAX_VALUE),  "1")
        _test(0, "int[0]", TypeCode.INT1, Integer,            Integer(Byte.MIN_VALUE),  "2" )
        _test(0, "int[0]", TypeCode.INT1, Integer,            Integer(Short.MAX_VALUE), "3" )
        _test(0, "int[0]", TypeCode.INT1, Integer,            Integer(Short.MIN_VALUE), "4" )
        _test(0, "int[0]", TypeCode.INT1, Integer,            Integer(Byte.MAX_VALUE),  "5" )
        _test(0, "int[0]", TypeCode.INT1, Integer,            Integer(Byte.MIN_VALUE),  "abc" )
        _test(1, "int[1]", TypeCode.ARRAY, [Integer],         [],              1)
        _test(2, "int[2]", TypeCode.ARRAY, [[Integer]],       [[]],            [] )
        _test(3, "int[3]", TypeCode.ARRAY, [[[Integer]]],     [[[]]],          [[]] )
        _test(4, "int[4]", TypeCode.ARRAY, [[[[Integer]]]],   [[[[]]]],        [[[]]] )
        _test(5, "int[5]", TypeCode.ARRAY, [[[[[Integer]]]]], [[[[[]]]]],      [[[[]]]] )
    
    def test_getNeg1(self):
        self.assertRaises(IllegalArgumentException, Validator_int.get, -1)
    
    def test_getNeg2(self):
        self.assertRaises(IllegalArgumentException, Validator_int.get(0).elementValidator)
    
    def test_getMaxPlusOne(self):
        self.assertRaises(IllegalArgumentException, Validator_int.get, Validator.MAX_NDIMS + 1)
    
    def _test(self, n, s, tc, clss, good, bad):
        v = Validator_int.get(n)
        self.assertEqual(n, v.getNDims())  
        self.assertEqual(clss, v.getExpectedClass())
        self.assertEqual(s, repr(v))
        self.assertEqual(True, v.validate(good))
        self.assertEqual(False, v.validate(bad))
        self.assertEqual(tc, v.checkValue(good))
        self.assertEqual(None, v.checkValue(bad))
        if n > 0:
            self.assertEqual(n-1, v.elementValidator().getNDims())
           
if __name__=='__main__':
    unittest.main()

