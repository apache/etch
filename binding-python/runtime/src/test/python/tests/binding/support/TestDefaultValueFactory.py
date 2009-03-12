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
from etch.binding.msg.Type import *
from etch.binding.msg.Field import *
from etch.binding.support.DefaultValueFactory import *
from etch.binding.support.TypeMap import *
from etch.binding.support.Class2TypeMap import *
from etch.binding.support._Etch_AuthException import *
from etch.binding.support._Etch_RuntimeException import *
from etch.binding.support.Validator_long import *
from etch.binding.msg.Message import *
from etch.binding.msg.StructValue import *


class MyValueFactory(DefaultValueFactory):
    typs       = TypeMap()
    class2type = Class2TypeMap()
    DefaultValueFactory.init(typs, class2type)
    _mt_x      = typs.get("x")
    _mt_y      = typs.get("y")
    _mf_a      = Field("a")
    _mf_b      = Field("b")
    typs.lock()
    class2type.lock()

    def __init__(self):
        super(MyValueFactory, self).__init__(self.typs, self.class2type)

class Test_DefaultValueFactory(unittest.TestCase):
    vf = MyValueFactory()

    def _mkTypeSet(self, *typs):
        # 's' Needs to be a set
        s = set([])
        for t in typs:
            s.add(t)
        return s

    def _mkFieldSet(self, *fields):
        s = set([])
        for f in fields:
            s.add(f)
        return s

    def _testType(self, t):
        self.assertEqual(t, self.vf.getType(t.getId()))
        self.assertEqual(t, self.vf.getType(t.getName()))

    def _testTypeField(self, typ, field, value):
        msg = Message(typ, self.vf)
        msg.put(field, value)

    def _testTypeFields(self, typ, *fields):
        aFields = typ.getFields()
        eFields = self._mkFieldSet(*fields)
        self.assertEquals(eFields, aFields)

    def test_types(self):
        eTypes = self._mkTypeSet(
                           self.vf.getType("_Etch_RuntimeException"),
                           self.vf.getType("_Etch_AuthException"),
                           self.vf.getType("_exception"),
                           MyValueFactory._mt_x,
                           MyValueFactory._mt_y)
        self.assertEquals(eTypes, self.vf.getTypes())

        for t in eTypes:
            self._testType(t)

    def test_Etch_RuntimeException_fields(self):
        self._testTypeFields(self.vf.getType("_Etch_RuntimeException"), DefaultValueFactory._mf_msg)
        self._testTypeField(self.vf.getType("_Etch_RuntimeException"),  DefaultValueFactory._mf_msg, "abc" )

    def test_Etch_RuntimeException_msg_badValue(self):
        self.assertRaises(IllegalArgumentException, self._testTypeField, self.vf.getType("_Etch_RuntimeException"), DefaultValueFactory._mf_msg, 123)

    def test_Etch_RuntimeException_badField(self):
        self.assertRaises(IllegalArgumentException, self._testTypeField, self.vf.getType("_Etch_RuntimeException"), MyValueFactory._mf_a, "abc")

    def test_Etch_AuthException_fields(self):
        self._testTypeFields(self.vf.getType("_Etch_AuthException"), DefaultValueFactory._mf_msg)

        self._testTypeField(self.vf.getType("_Etch_AuthException"), DefaultValueFactory._mf_msg, "abc")


    def test_Etch_AuthException_msg_badValue(self):
        self.assertRaises(IllegalArgumentException, self._testTypeField,self.vf.getType("_Etch_AuthException"), DefaultValueFactory._mf_msg, 123)

    def test_exception_fields(self):
        self._testTypeFields(self.vf.getType("_exception"),
                             DefaultValueFactory._mf_result,
                             DefaultValueFactory._mf__messageId,
                             DefaultValueFactory._mf__inReplyTo)
        self._testTypeField(self.vf.getType("_exception"), DefaultValueFactory._mf_result, RuntimeException())
        self._testTypeField(self.vf.getType("_exception"), DefaultValueFactory._mf__messageId, 123)
        self._testTypeField(self.vf.getType("_exception"), DefaultValueFactory._mf__inReplyTo, 123)

    def test_exception_result_badValue(self):
        self.assertRaises(IllegalArgumentException, self._testTypeField, self.vf.getType("_exception"), DefaultValueFactory._mf_result, 123)

    def test_exception_messageId_badValue(self):
        self.assertRaises(IllegalArgumentException, self._testTypeField,self.vf.getType("_exception"), DefaultValueFactory._mf__messageId, True)

    def test_exception_inReplyTo_badValue(self):
        self.assertRaises(IllegalArgumentException, self._testTypeField,self.vf.getType("_exception"), DefaultValueFactory._mf__inReplyTo, "abc")

    def test_getType_locked(self):
        self.assertNotEquals(None, self.vf.getType("x"))
        self.assertNotEquals(None, self.vf.getType("y"))
        self.assertEquals(None, self.vf.getType("a"))
        self.assertEquals(None, self.vf.getType("b"))

    def test_getField_locked(self):
        typ = Type("blah")
        self.assertNotEquals(None, typ.getField("a"))
        self.assertNotEquals(None, typ.getField("b"))
        typ.lock()
        self.assertNotEquals(None, typ.getField("a"))
        self.assertNotEquals(None, typ.getField("b"))
        self.assertEquals(None, typ.getField("x"))
        self.assertEquals(None, typ.getField("y"))

    def test_getField_id_messageId(self):
        self.assertEquals("_messageId", DefaultValueFactory._mf__messageId.getName())
        self.assertEquals(1661383784, DefaultValueFactory._mf__messageId.getId())

    def test_getField_id_inReplyTo(self):
        self.assertEquals("_inReplyTo", DefaultValueFactory._mf__inReplyTo.getName())
        self.assertEquals(-307705434, DefaultValueFactory._mf__inReplyTo.getId())

    def test_getStringEncoding(self):
        enc = self.vf.getStringEncoding()
        s = String(['a','b','c'], enc)
        self.assertEquals("abc", s)

    def test_messageId(self):
        mt_foo = Type("foo")
        mt_foo.putValidator(DefaultValueFactory._mf__messageId, Validator_long.get(0))

        msg = Message(mt_foo, self.vf)
        self.assertEquals(None, self.vf.getMessageId(msg))

        self.vf.setMessageId(msg, 234)

        msgid = self.vf.getMessageId(msg)
        self.assertNotEquals(None, msgid)
        self.assertEquals(234, msgid)

        self.vf.setMessageId(msg, None)

        self.assertEquals(None, self.vf.getMessageId(msg))

    def test_inReplyTo(self):
        mt_foo = Type("foo")
        mt_foo.putValidator(DefaultValueFactory._mf__inReplyTo, Validator_long.get(0))

        msg = Message(mt_foo, self.vf)

        self.assertEquals(None, self.vf.getInReplyTo(msg))
        self.vf.setInReplyTo(msg, 234)

        msgid = self.vf.getInReplyTo(msg)
        self.assertNotEquals(None, msgid)
        self.assertEquals(234, msgid)

        self.vf.setInReplyTo(msg, None)
        self.assertEquals(None, self.vf.getInReplyTo(msg))

    def test_exportCustomValue_RuntimeException(self):
        value = NullPointerException()
        sv    = self.vf.exportCustomValue(value)
        sv.checkType(self.vf.getType("_Etch_RuntimeException"))
        self.assertEquals(1, sv.size())
        self.assertEquals("NullPointerException", sv.get(DefaultValueFactory._mf_msg))

    def test_exportCustomValue_RuntimeException_msg(self):
        value = NullPointerException("foo != null")
        sv    = self.vf.exportCustomValue(value)
        sv.checkType(self.vf.getType("_Etch_RuntimeException"))
        self.assertEquals(1, sv.size())
        self.assertEquals("NullPointerException: foo != null", sv.get(DefaultValueFactory._mf_msg))

    def test_exportCustomValue_Object(self):
        value = object()
        self.assertEquals(None, self.vf.exportCustomValue(value))

    def test_importCustomValue_Etch_RuntimeException(self):
        sv = StructValue(self.vf.getType("_Etch_RuntimeException"))
        e = self.vf.importCustomValue(sv)
        self.assertNotEquals(None, e)
        self.assertEquals(None, e.msg)

    def test_importCustomValue_Etch_RuntimeException_msg(self):
        sv = StructValue(self.vf.getType("_Etch_RuntimeException"))
        sv.put(DefaultValueFactory._mf_msg, "foo")
        e = self.vf.importCustomValue(sv)
        self.assertNotEquals(None, e)
        self.assertEquals("foo", e.msg)

    def test_importCustomValue_Unknown(self):
        sv = StructValue(self.vf.getType("x"))
        self.assertEquals(None, self.vf.importCustomValue(sv))

if __name__ == '__main__':
    unittest.main()
