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
import time

from etch.binding.transport.fmt.binary.BinaryTaggedDataOutput import BinaryTaggedDataOutput
from etch.binding.support.DefaultValueFactory import DefaultValueFactory
from etch.binding.support.TypeMap import TypeMap
from etch.binding.support.Class2TypeMap import Class2TypeMap
from etch.binding.msg.Type import Type
from etch.pyhon.Types import *
from etch.util.PacketSource import PacketSource

class TestBinaryTaggedDataInOut(unittest.TestCase):
    DOUBLE_MIN_NORMAL = "0x1.0p-1022"
    FLOAT_MIN_NORMAL  = "0x1.0p-126f"

    def setUp(self):
        print "\n"
        print "TestBinaryTaggedDataInOut.setUp"
        self.__vf     = ValueFactoryFake()
        self.__mt_foo = self.__vf.getType("foo")
        self.__mf_x   = Field("x")

    def test_check_value(self):
        btdo = BinaryTaggedDataOutput ( None, None )

        # positive small bytes
        self.assertEquals(0,btdo.checkValue(Byte(0)))
        self.assertEquals(1,btdo.checkValue(Byte(1)))
        self.assertEquals(31,btdo.checkValue(Byte(31)))

        # positive small shorts
        self.assertEquals(32,btdo.checkValue(Short(0)))
        self.assertEquals(33,btdo.checkValue(Short(1)))
        self.assertEquals(63,btdo.checkValue(Short(31)))

        # positive small ints
        self.assertEquals(64, btdo.checkValue(0))
        self.assertEquals(65, btdo.checkValue(1))
        self.assertEquals(95, btdo.checkValue(31))

        # positive small longs
        self.assertEquals(96, btdo.checkValue(0L))
        self.assertEquals(97, btdo.checkValue(1L))
        self.assertEquals(127, btdo.checkValue(31L))

        # negative small bytes
        self.assertEquals(-128, btdo.checkValue(Byte(-1)))
        self.assertEquals(-127, btdo.checkValue(Byte(-2)))
        self.assertEquals(-113, btdo.checkValue(Byte(-16)))

        # negative small shorts
        self.assertEquals(-112, btdo.checkValue(Short(-1)))
        self.assertEquals(-111, btdo.checkValue(Short(-2)))
        self.assertEquals(-97, btdo.checkValue(Short(-16)))

        # negative small ints
        self.assertEquals(-96, btdo.checkValue(-1))
        self.assertEquals(-79, btdo.checkValue(-2))
        self.assertEquals(-65, btdo.checkValue(-16))

        # negative small longs
        self.assertEquals(-80, btdo.checkValue(-1L))
        self.assertEquals(-79, btdo.checkValue(-2L))
        self.assertEquals(-65, btdo.checkValue(-16L))

        # null value
        self.assertEquals(-1, btdo.checkValue(None))

        # boolean values
        self.assertEquals(-2, btdo.checkValues(False))
        self.assertEquals(-3, btdo.checkValues(True))

        # normal bytes
        self.assertEquals(-4, btdo.checkValue(Byte(-45)))
        self.assertEquals(-4, btdo.checkValue(Byte(45)))

        # normal shorts
        self.assertEquals(-5, btdo.checkValue(Short(-45)))
        self.assertEquals(-5, btdo.checkValue(Short(45)))

        self.assertEquals(-6, btdo.checkValue(Short(-4545)))
        self.assertEquals(-6, btdo.checkValue(Short(4545)))

        # normal ints
        self.assertEquals(-7, btdo.checkValues(-45))
        self.assertEquals(-7, btdo.checkValue(45))

        self.assertEquals(-8, btdo.checkValue(-4545))
        self.assertEquals(-8, btdo.checkValue(4545))

        self.assertEquals(-9, btdo.checkValue(-45454545))
        self.assertEquals(-9, btdo.checkValue(45454545))

        # normal longs
        self.assertEquals(-10, btdo.checkValues(-45L))
        self.assertEquals(-10, btdo.checkValue(45L))

        self.assertEquals(-11, btdo.checkValue(-4545L))
        self.assertEquals(-11, btdo.checkValue(4545L))

        self.assertEquals(-12, btdo.checkValue(-45454545L))
        self.assertEquals(-12, btdo.checkValue(45454545L))

        self.assertEquals(-12, btdo.checkValue(-4545454545454545L))
        self.assertEquals(-12, btdo.checkValue(4545454545454545L))

        # float
        self.assertEquals(-14, btdo.checkValue(Float(3.14159)))

        # double
        self.assertEquals(-15, btdo.checkValue(3.14159))

        # byte array
        self.assertEquals(-16, btdo.checkValue(ByteArray(0)))
        self.assertEquals(-16, btdo.checkValue(ByteArray(1)))

        # empty string
        self.assertEquals(-17, btdo.checkValue(""))
        self.assertEquals(-17, btdo.checkValue(''))
        self.assertEquals(-17, btdo.checkValue(u''))
        self.assertEquals(-17, btdo.checkValue(""""""))

        # short string
        self.assertEquals(-18, btdo.checkValue("a"))
        self.assertEquals(-18, btdo.checkValue('a'))

        # non-empty string
        self.assertEquals(-18, btdo.checkValue("abc"))
        self.assertEquals(-18, btdo.checkValue('abc'))
        self.assertEquals(-18, btdo.checkValue(u"abc"))

        # struct
        self.assertEquals(-19, btdo.checkValue(StructValue(Type("foo"))))

        # custom
        self.assertEquals(-21, btdo.checkValue(Date()))

        # none
        self.assertEquals(-22, btdo.checkValue(BinaryTaggedData.NONE))

    def xxtest_boolean(self):
        self._testx( ((False, True), (True, False)), Validator_boolean.get(2) )

    def xxtest_byte(self):
        self._testx( ((Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE), (23,)), Validator_byte.get(2) )

    def xxtest_short(self):
        self._testx( ((Short.MIN_VALUE, Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE, Short.MAX_VALUE), (23,)), Validator_short.get(2) )

    def xxtest_int(self):
        self._testx( ((Integer.MIN_VALUE, Short.MIN_VALUE, Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE, Short.MAX_VALUE, Integer.MAX_VALUE), (23,)), Validator_int.get(2))

    def xxtest_long(self):
        self._testx( ((Long.MIN_VALUE, Integer.MIN_VALUE, Short.MIN_VALUE, Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE, Short.MAX_VALUE, Integer.MAX_VALUE, Long.MAX_VALUE), (23,)), Validator_long(2))

    def xxtest_float(self):
        self._testx( ((-1, 0, 1, Float.MIN_VALUE, TestBinaryTaggedDataInOut.FLOAT_MIN_NORMAL, Float.MAX_VALUE, Float.NaN, Float.NEGATIVE_INFINITY, Float.POSITIVE_INFINITY, -0.0, 1.1, 3.141592653589793238462643383279), (23,)), Validator_float.get(2))

    def xxtest_double(self):
        self._testx( ((-1, 0, 1, Double.MIN_VALUE, TestBinaryTaggedDataInOut.DOUBLE_MIN_NORMAL, Double.MAX_VALUE, Double.NaN, Double.NEGATIVE_INFINITY, Double.POSITIVE_INFINITY, -0.0, 1.1, 3.141592653589793238462643383279), (23,)), Validator_double.get(2))

    def xxtest_string(self):
        self._testx( (('', "", 'a', "a", 'ab', "ab", 'abc', "abc"), (23,)), Validator_string.get(2))

    def __add_setup(self):
        add = self.__vf.getType("add")

        x   = Field("x")
        add.putValidator(x, Validator_int.get(0))

        y   = Field("y")
        add.putValidator(y, Validator_int.get(0))

        _mf__messageId = DefaultValueFactory._mf__messageId
        add.putValidator(_mf__messageId, Validator_long.get(0))

        msgid = 0x0123456789abcdefL

        return (add,x,y,_mf__messageId, msgid)

    def xxtest_add(self):
        (add,x,y,_mf__messageId,msgid) = self.__add_setup()

        msg = Message(add, self.__vf)
        msg.put(x, 1)
        msg.put(y, 2)
        msg.put(_mf__messageId, msgid)

        msg = Message(add, self.__vf)
        msg.put(x, 1000000000)
        msg.put(y, 2000000000)
        msg.put( _mf__messageId, msgid)

    def xxtest_add_in(self):
        (add,x,y,_mf__messageId,msgid) = self.__add_setup()

        buf = ByteArray([
                           1,                 # version
                          -9,                 # INT4
                          39, -23, -73, -100, # add
                          -9,                 # INT4
                          99,   6, -76,  104,
                         -13,                 # LONG8
                           1,  35,  69,  103, -119, -85, -51, -17,
                          -9,                 # INT4
                          21,  10,  44,  -76, # y
                          66,                 # INT4=2
                          -9,                 # INT4
                          21,  10,  44,  -77, # x
                          65,                 # INT4=1
                         -22,                 # NONE
                         ])
        msg = self.bytes2msg(buf)
        msg.checkType(add)

        self.assertEquals(3, msg.size())
        self.assertEquals(1, msg.get(x))
        self.assertEquals(2, msg.get(y))
        self.assertEquals(msgid, msg.get(_mf__messageId))

        buf = ByteArray([])
        msg = self.bytes2msg(buf)
        msg.checkType(add)
        self.assertEquals(3, msg.size())
        self.assertEquals(1000000000, msg.get(x))
        self.assertEquals(2000000000, msg.get(y))
        self.assertEquals(msgid, msg.get(_mf__messageId))

    def xxtest_add_perf(self):
        (add,x,y,_mf__messageId,msgid) = self.__add_setup()

        msg = Message(add, self.__vf)
        msg.put(x, 123456789)
        msg.put(y, 876543210)
        msg.put(_mf__messageId, msgid)

        m  = Messagizer(None, self.__vf)
        ps = MyPacketSource()
        m.setSource(ps)
        m.messagex(None, msg)

        n = 900973
        for i in range(0,3):
            testPerf("test_add_perf", i, m, msg, n)

    def xxtest_sum_perf(self):
        sum = self.__vf.getType("sum")

        values = Field("values")
        sum.putValidator(values, Validator_int.get(1))

        _mf__messageId = DefaultValueFactory._mf__messageId
        sum.putValidator(_mf__messageId, Validator_long.get(0))

        msgid = 0x0123456789abcdefL

        arr = [None, None]
        for i in range(0,len(arr)):
            arr[i] = 123456789

        msg = Message(sum, self.__vf)
        msg.put(values, arr)
        msg.put(_mf__messageId, msgid)

        m = Messagizer(None, self.__vf)
        ps = MyPacketSource()
        m.setSource(ps)
        m.messagex(None, msg)

        n = 509520
        for i in range(0,3):
            testPerf("test_sum_perf", i, m, msg, n)

    @staticmethod
    def xxtestPerf(name, iter, m, msg, n):
        t0 = time.time()

        for i in range(0,n):
            m.messagex(None, msg)

        t1 = time.time()
        t  = (t1-t0)
        r  = n / t
        Log.report(name, "iteration", iter, "time", t, "count", n, "rate", r)

    def msg2bytes(self, msg):
        buf  = FlexBuffer()
        btdo = BinaryTaggedDataOutput(buf, self.__vf)
        btdo.writeMessage(msg)
        buf.setIndex(0)
        return buf.getAvailBytes()

    def bytes2msg(self, buf):
        return BinaryTaggedDataInput.readMessage(self.__vf, FlexBuffer(buf))

    def _testx(self, x, v):
        if isinstance(x, ([],())):
            for i in x:
                self._testx(i,v.elementValidator())
        y = self._test(x, v)
        self.localAssertEquals(x,y)

    def _test(self, x, v):
        mt_foo = self.__mt_foo
        mf_x   = self.__mf_x
        vf     = self.__vf

        mt_foo.clearValidator(mf_x)
        mt_foo.putValidator(mf_x, v)

        msg = Message(mt_foo, vf)
        msg.put(mf_x, x)

        bufx = self.msg2bytes(msg)
        msg2 = self.bytes2msg(bufx)

        msg2.checkType(mt_foo)
        self.assertEquals(1, msg2.size())
        msg.containsKey(mf_x)
        return msg2.get(mf_x)

    def localAssertEquals(self, a, b):
        if not a or not b:
            self.assertEquals(a,b)
            return

        if a.__class__ == b.__class__:
            if isinstance(a, types.ArrayType) and isinstance(b, types.ArrayType):
                self.localAssertArrayEquals(a,b)
            else:
                self.assertEquals(a,b)
        else:
            self.assertEquals(a,b)

    def localAssertArrayEquals(self, a, b):
        an = len(a)
        bn = len(b)

        self.localAssertEquals(an, bn)

        for i in range(0,an):
            self.localAssertEquals(a[i], b[i])

class ValueFactoryFake(DefaultValueFactory):
    typs = TypeMap()
    class2type = Class2TypeMap()
    DefaultValueFactory.init(typs, class2type)
    
    def __init__(self):
        super(ValueFactoryFake,self).__init__(self.typs, self.class2type)

class MyPacketSource(PacketSource):

    def headerSize(self):
        return 0

    def packet(self, recipient, buf):
        pass

    def getHandler(self):
        return None

    def setHandler(self, handler):
        pass

    def transportControl(self, control, value):
        pass

    def transportNotify(self, event):
        pass

    def transportQuery(self, query):
        return None

if __name__=='__main__':
    unittest.main()
