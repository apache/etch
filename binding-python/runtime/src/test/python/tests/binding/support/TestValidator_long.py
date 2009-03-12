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
from etch.binding.support.Validator_long import *
from etch.binding.transport.fmt.TypeCode import *

class Test_EtchSupportValidator_long(unittest.TestCase):
        
    def _test_validator(self):
        _test = self._test
        
        # TODO - resolve casting issues in validator tests
        # TODO - resolve arrays 
        _test(0, "long[0]", TypeCode.LONG1, Long,           Long(Byte.MAX_VALUE),  "1")
        _test(0, "long[0]", TypeCode.LONG1, Long,           Long(Byte.MIN_VALUE),  "2" )
        _test(0, "long[0]", TypeCode.LONG2, Long,           Long(Short.MAX_VALUE), "3" )
        _test(0, "long[0]", TypeCode.LONG2, Long,           Long(Short.MIN_VALUE), "4" )
        _test(0, "long[0]", TypeCode.LONG2, Long,           Long(Byte.MAX_VALUE),  "5" )
        _test(0, "long[0]", TypeCode.LONG2, Long,           Long(Byte.MIN_VALUE),  "abc" )
        _test(1, "long[1]", TypeCode.ARRAY, [Long],         [],              1)
        _test(2, "long[2]", TypeCode.ARRAY, [[Long]],       [[]],            [] )
        _test(3, "long[3]", TypeCode.ARRAY, [[[Long]]],     [[[]]],          [[]] )
        _test(4, "long[4]", TypeCode.ARRAY, [[[[Long]]]],   [[[[]]]],        [[[]]] )
        _test(5, "long[5]", TypeCode.ARRAY, [[[[[Long]]]]], [[[[[]]]]],      [[[[]]]] )
    
    def test_getNeg1(self):
        self.assertRaises(IllegalArgumentException, Validator_long.get, -1)
    
    def test_getNeg2(self):
        self.assertRaises(IllegalArgumentException, Validator_long.get(0).elementValidator)
    
    def test_getMaxPlusOne(self):
        self.assertRaises(IllegalArgumentException, Validator_long.get, Validator.MAX_NDIMS + 1)
    
    def _test(self, n, s, tc, clss, good, bad):
        v = Validator_long.get(n)
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

