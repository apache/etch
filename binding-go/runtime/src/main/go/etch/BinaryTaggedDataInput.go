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

type BinaryTaggedDataInput struct {
	buf *FlexBuffer
	vf  ValueFactory
}


func NewBinaryTaggedDataInput(vf ValueFactory) *BinaryTaggedDataInput {
	return &BinaryTaggedDataInput{nil, vf}
}

func (btdi *BinaryTaggedDataInput) ReadMessage(buf *FlexBuffer) *Message {
	btdi.buf = buf
	theMsg := btdi.startMessage()
	if theMsg == nil { //unknown function
		return nil
	}
	btdi.readKeysAndValues(theMsg)

	btdi.endMessage(theMsg)

	Log("BinaryTaggedDataInput Readmessage: got " + fmt.Sprint(theMsg) + " Fields: ")
	for k, v := range theMsg.Fields{
		Log(" " + fmt.Sprint(k) + " = " + fmt.Sprint(v))
	}	
	Log("\n")
	return theMsg
}

func (btdi *BinaryTaggedDataInput) startMessage() *Message {
	version := btdi.buf.ReadByte()
	if version != VERSION {
		return nil
	}

	ty := btdi.readType()
	if ty == nil {
		Log("BinaryTaggedDataInput: startMessage discarding msg because of unknown type\n")
		//type of msg not known
		return nil
	}
	l := btdi.readLength()
	return NewMessage(ty, btdi.vf, int(l))
}

func (btdi *BinaryTaggedDataInput) readType() *Type {
	obj := btdi.readValue(nil, true)
	switch obj.(type) {
	case int32:
		id := toInteger(obj)
		t := btdi.vf.GetTypeById(id)
		return t
	}
	return nil
}

func (btdi *BinaryTaggedDataInput) readLength() int32 {
	return btdi.readIntegerValue()
}

func (btdi *BinaryTaggedDataInput) readIntegerValue() int32 {
	val := btdi.readValue(nil, false)
	Log("BinaryTaggedDataInput readIntegerValue " + fmt.Sprint(val) + "\n")
	return toInteger(val)
}


func (btdi *BinaryTaggedDataInput) readValue(val Validator, noneOk bool) interface{} {
	byTy := btdi.buf.ReadByte()

	Log("BinaryTaggedDataInput readValue: byty = " + fmt.Sprint(byTy) + "\n")

	switch byTy {
	case NULL:
	case NONE:
		if noneOk {
			return NONE
		} else {
			return nil
		}
	case BYTE:
		return btdi.validateValue(val, btdi.buf.ReadByte())
	case SHORT:
		return btdi.validateValue(val, btdi.buf.ReadShort())
	case INT:
		return btdi.validateValue(val, btdi.buf.ReadInt())
	case LONG:
		return btdi.validateValue(val, btdi.buf.ReadLong())
	case FLOAT:
		return btdi.validateValue(val, btdi.buf.ReadFloat())
	case DOUBLE:
		return btdi.validateValue(val, btdi.buf.ReadDouble())
	case BYTES:
		return btdi.validateValue(val, btdi.readBytes())
	case ARRAY:
		av := btdi.readArray()
		if av == nil {
			return nil
		}
		return btdi.validateValue(val, FromArrayValue(av))
	case EMPTY_STRING:
		return btdi.validateValue(val, "")
	case STRING:
		return btdi.validateValue(val, string(btdi.readBytes()))
	case CUSTOM:
		sv := btdi.readStruct()
		return btdi.validateValue(val, btdi.vf.ImportCustomValue(sv))

	}
	if byTy >= MIN_TINY_INT && byTy <= MAX_TINY_INT {
		return int8(byTy)
	}
	return nil
}

func (btdi *BinaryTaggedDataInput) validateValue(vldtr Validator, val interface{}) interface{} {
	//TODO implement me
	return val
}

func (btdi *BinaryTaggedDataInput) startStruct() *StructValue {
	ty := btdi.readType()
	if ty == nil {
		return nil // type not known, what shall we do ? 
	}
	l := btdi.readLength()
	return NewStructValue(ty, btdi.vf, int(l))
}

func (btdi *BinaryTaggedDataInput) endStruct(sv *StructValue) {
	// nothing to do, readKeysAndValues swallowed the NONE.
}

func (btdi *BinaryTaggedDataInput) readField(ty *Type) *Field {
	//this "true" swallows the NONE at the end of a struct
	obj := btdi.readValue(nil, true)
	Log("BinaryTaggedDataInput: readField : next Field ID is : " + fmt.Sprint(obj) + "\n")
	switch obj.(type) {
	case int32:
		f := ty.Fields[toInteger(obj)]
		return f
	}
	return nil
}

func (btdi *BinaryTaggedDataInput) readArray() *ArrayValue {
	av := btdi.startArray()
	if av == nil {
		return nil
	}
	btdi.readValues(av)
	btdi.endArray(av)
	return av
}


func (btdi *BinaryTaggedDataInput) startArray() *ArrayValue {
	Log("BinaryTaggedDataInput: startArray\n")
	typeCode := btdi.buf.ReadByte()

	Log("BinaryTaggedDataInput: startArray: typeCode = " + fmt.Sprint(typeCode) + "\n")
	var cTy *Type = nil
	if(typeCode == CUSTOM) {
		cTy = btdi.readType()
	}


	Log("BinaryTaggedDataInput: startArray: cTy = " + fmt.Sprint(cTy) + "\n")

	Log("BinaryTaggedDataInput: startArray buf = " + fmt.Sprint(btdi.buf) + "\n")
	Log("BinaryTaggedDataInput: startArray buf = " + fmt.Sprint(btdi.buf.Index) + "\n")

	dim := btdi.readIntegerValue()

	Log("BinaryTaggedDataInput: startArray: dim = " + fmt.Sprint(dim) + "\n")
	if dim != 1 {
		//multidimension not yet supported
		return nil
	}
	len := btdi.readIntegerValue()
	Log("BinaryTaggedDataInput: startArray: len = " + fmt.Sprint(len) + "\n")
	Log("startArray: NewArrayForType " + fmt.Sprint(typeCode) +  " " + fmt.Sprint(cTy) + "\n")
	array := btdi.vf.NewArrayForType(typeCode, cTy, len)

	Log("BinaryTaggedDataInput: startArray: array = " + fmt.Sprint(array) + "\n")
	Log("startArray: NewArrayForType " + fmt.Sprint(array) + "\n")
	return NewArrayValueWith(array, typeCode, cTy)
}

func (btdi *BinaryTaggedDataInput) endArray(av *ArrayValue) {
	// nothing to do, readValues swallowed the NONE.
}

func (btdi *BinaryTaggedDataInput) readValues(av *ArrayValue) {
	i := 0
	for true {
		val := btdi.readValue(nil, true)
		if val == NONE {
			break
		}
		av.Set(i, val)
		i++
	}
}



func (btdi *BinaryTaggedDataInput) readStruct() *StructValue {
	sv := btdi.startStruct()
	if sv == nil {
		return nil
	}
	btdi.readKeysAndValues(sv)
	btdi.endStruct(sv)
	return sv
}

func (btdi *BinaryTaggedDataInput) readBytes() []uint8 {
	l := btdi.readLength()
	bys := make([]uint8, l)
	btdi.buf.ReadBytes(&bys)
	return bys
}

func (btdi *BinaryTaggedDataInput) readKeysAndValues(sv StructValueIntf) {
	ty := sv.GetType()

	if ty == nil {
		Log("readKeys TYPE NOT FOUND!\n")
		return
	}

	for true {
		key := btdi.readField(ty)
		Log("BinaryTaggedDataInput: readKeysAndValues: read field" + fmt.Sprint(key) + "\n")
		if key == nil {
			break
		}
		vldtr := ty.Validators[key.Id]
		if vldtr != nil {
			Log("BinaryTaggedDataInput: readKeysAndValues: I have a validator for" + fmt.Sprint(key) + "\n")
			val := btdi.readValue(vldtr, false)
			Log("BinaryTaggedDataInput: readKeysAndValues: Value of " + fmt.Sprint(key) + " is " + fmt.Sprint(val) + "\n")
			sv.Put(key, val)
		} else {
			Log("BinaryTaggedDataInput: readKeysAndValues: I have NO validator for" + fmt.Sprint(key) + "\n")
			btdi.readValue(nil, false)
		}
	}
}

func (btdi *BinaryTaggedDataInput) endMessage(mes *Message) {
	// nothing to do, readKeysAndValues swallowed the NONE.

}
