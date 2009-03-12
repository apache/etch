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
from etch.binding.support.Validator_float import *
from etch.binding.transport.fmt.TypeCode import *

class Test_EtchSupportValidator_float(unittest.TestCase):

    def _test_validator(self):
        _test = self._test
        
        # TODO - resolve casting issues in validator tests
        _test(0, "float[0]", TypeCode.FLOAT4, Float,          Float(Float.MAX_VALUE),  "1")
        _test(0, "float[0]", TypeCode.FLOAT4, Float,          Float(Float.MIN_VALUE),  "2" )
        _test(0, "float[0]", TypeCode.FLOAT4, Float,          Float(Float.MAX_VALUE), "3" )
        _test(0, "float[0]", TypeCode.FLOAT4, Float,          Float(Float.MIN_VALUE), "4" )
        _test(0, "float[0]", TypeCode.FLOAT4, Float,          Float(Float.MAX_VALUE),  "5" )
        _test(0, "float[0]", TypeCode.FLOAT4, Float,          Float(Float.MIN_VALUE),  "abc" )
        _test(1, "float[1]", TypeCode.ARRAY, [Float],         [],              1)
        _test(2, "float[2]", TypeCode.ARRAY, [[Float]],       [[]],            [] )
        _test(3, "float[3]", TypeCode.ARRAY, [[[Float]]],     [[[]]],          [[]] )
        _test(4, "float[4]", TypeCode.ARRAY, [[[[Float]]]],   [[[[]]]],        [[[]]] )
        _test(5, "float[5]", TypeCode.ARRAY, [[[[[Float]]]]], [[[[[]]]]],      [[[[]]]] )
    
    def test_getNeg1(self):
        self.assertRaises(IllegalArgumentException, Validator_float.get, -1)
    
    def test_getNeg2(self):
        self.assertRaises(IllegalArgumentException, Validator_float.get(0).elementValidator)
    
    def test_getMaxPlusOne(self):
        self.assertRaises(IllegalArgumentException, Validator_float.get, Validator.MAX_NDIMS + 1)
    
    def _test(self, n, s, tc, clss, good, bad):
        v = Validator_float.get(n)
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


