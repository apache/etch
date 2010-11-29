/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
package etch

import "fmt"

type BinaryTaggedDataOutput struct {
	buf *FlexBuffer
	vf  ValueFactory
}

func NewBinaryTaggedDataOutput(vf ValueFactory) *BinaryTaggedDataOutput {
	return &BinaryTaggedDataOutput{nil, vf}
}


func (btdo *BinaryTaggedDataOutput) WriteMessage(msg *Message, buf *FlexBuffer) {
	Log("BinaryTaggedDataOutput: writemessage for " + fmt.Sprint(msg) + "\n")
	btdo.buf = buf
	btdo.startMessage(msg)
	btdo.writeKeysAndValues(msg)
	btdo.endMessage(msg)
	Log("BinaryTaggedDataOutput: writemessage for " + fmt.Sprint(msg) + "done\n")
}

func (btdo *BinaryTaggedDataOutput) startMessage(msg *Message) {
	btdo.buf.PutByte(VERSION)
	btdo.startStruct(msg)
}

func (btdo *BinaryTaggedDataOutput) endMessage(msg *Message) {
	Log("BinaryTaggedDataOutput: endmessage for " + fmt.Sprint(msg) + "\n")
	btdo.endStruct(msg)
}

func (btdo *BinaryTaggedDataOutput) writeArray(av *ArrayValue) {
	btdo.startArray(av)
	btdo.writeValues(av)
	btdo.endArray(av)
}

func (btdo *BinaryTaggedDataOutput) startArray(av *ArrayValue) {

	Log("BinaryTaggedDataOutput startArray 1: " + fmt.Sprint(btdo.buf) + "\n")
	Log("BinaryTaggedDataOutput startArray typeCode = " + fmt.Sprint(av.TypeCode) + "\n")
	btdo.buf.Put(toByte(av.TypeCode))
	Log("BinaryTaggedDataOutput startArray 2: " + fmt.Sprint(btdo.buf) + "\n")
	if av.TypeCode == CUSTOM {
		btdo.writeType(av.CustomTy)
	}
	Log("BinaryTaggedDataOutput startArray 3: " + fmt.Sprint(btdo.buf) + "\n")
	btdo.writeValue(nil, int32(av.Dim))
	Log("BinaryTaggedDataOutput startArray 4: " + fmt.Sprint(btdo.buf) + "\n")
	btdo.writeValue(nil, int32(av.Size()))
	Log("BinaryTaggedDataOutput startArray 5: " + fmt.Sprint(btdo.buf) + "\n")
}


func (btdo *BinaryTaggedDataOutput) writeValues(av *ArrayValue) {
	for i:=0; i < av.Size(); i++ {
		btdo.writeValue(nil, av.Get(i))
	}
}

func (btdo *BinaryTaggedDataOutput) endArray(av *ArrayValue) {
	btdo.writeValue(nil, NONE)	
}


func (btdo *BinaryTaggedDataOutput) writeStruct(sv StructValueIntf) {
	btdo.startStruct(sv)
	btdo.writeKeysAndValues(sv)
	btdo.endStruct(sv)
}

func (btdo *BinaryTaggedDataOutput) startStruct(msg StructValueIntf) {
	btdo.writeType(msg.GetType())
	btdo.writeIntValue(int32(msg.Size()))
}


func (btdo *BinaryTaggedDataOutput) writeType(ty *Type) {
	btdo.writeIntValue(ty.Id)
}

func (btdo *BinaryTaggedDataOutput) writeIntValue(i int32) {
	btdo.writeValue(nil, i)
}

func (btdo *BinaryTaggedDataOutput) writeValue(val Validator, value interface{}) {
	if value == nil {
		Log("BinaryTaggedDataOutput writeValue: skipping nil\n")
		return
	} 

	if val != nil && !val.Validate(value) {
		Log("BinaryTaggedDataOutput writeValue: skipping, because validation failed\n")
		return
	}

	ty := checkValue(value)
	btdo.buf.PutByte(ty)
	switch ty {
	case BYTE:
		btdo.buf.Put(toByte(value))
	case SHORT:
		btdo.buf.Put(toShort(value))
	case INT:
		btdo.buf.Put(toInteger(value))
	case LONG:
		btdo.buf.Put(value)
	case FLOAT:
		btdo.buf.Put(value)
	case DOUBLE:
		btdo.buf.Put(value)
	case BYTES:
		btdo.writeIntValue(int32(len(value.([]byte))))
		btdo.buf.PutBytes(value.([]byte))
	case STRING:
		bytes := toBytes(value.(string))
		btdo.writeIntValue(int32(len(bytes)))
		btdo.buf.PutBytes(bytes)
	case ARRAY:
		btdo.writeArray(ToArrayValue(value.(interface{}), btdo.vf))
	case CUSTOM:
		sv := btdo.vf.ExportCustomValue(value)
		if sv == nil {
			return
		}
		btdo.writeStruct(sv)
	default:
	}
}


func (btdo *BinaryTaggedDataOutput) endStruct(msg StructValueIntf) {
	btdo.writeValue(nil, NONE)
}

func (btdo *BinaryTaggedDataOutput) writeField(f *Field) {
	btdo.writeIntValue(f.Id)
}

func (btdo *BinaryTaggedDataOutput) writeKeysAndValues(sv StructValueIntf) {
	Log("BinaryTaggedDataOutput: writeKeysAndValues for " + fmt.Sprint(sv) + "\n")
	for f, val := range *sv.GetFields() {
		if val != nil {
			Log("BinaryTaggedDataOutput: writeKeysAndValues for " + fmt.Sprint(f) + " = " + fmt.Sprint(val) +"\n")
			btdo.writeField(f)
			btdo.writeValue(nil, val)
		}
	}
}
