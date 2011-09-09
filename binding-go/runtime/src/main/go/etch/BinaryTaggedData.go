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
import "reflect"

const (
	VERSION int8 = 3
)

const (
	NULL          int8 = -128
	NONE          int8 = -127
	BOOLEAN_FALSE int8 = -126
	BOOLEAN_TRUE  int8 = -125
	BYTE          int8 = -124
	SHORT         int8 = -123
	INT           int8 = -122
	LONG          int8 = -121
	FLOAT         int8 = -120
	DOUBLE        int8 = -119
	BYTES         int8 = -117
	ARRAY         int8 = -111
	EMPTY_STRING  int8 = -110
	STRING        int8 = -109
	STRUCT        int8 = -108
	CUSTOM        int8 = -107
	ANY           int8 = -106
	MIN_TINY_INT  int8 = -64
	MAX_TINY_INT  int8 = 127
)

type ArrayValue struct {
	TypeCode int8
	CustomTy *Type
	Dim      int
	ArrayVal reflect.Value
}

func NewArrayValueWith(array interface{}, ty int8, cty *Type) *ArrayValue{
	ret := &ArrayValue{ty, cty,1,reflect.ValueOf(array)}
	return ret
}


func NewArrayValue(array interface{}, vf ValueFactory) *ArrayValue{
	//only support 1-dimensional, non-empty arrays up to now...
	//other arrays are likely to crash
	ret := &ArrayValue{0,nil,1,reflect.ValueOf(array)}
	Log("NewArrayValue: " + fmt.Sprint(ret.Get(0)) + "\n")
	ret.TypeCode = getTypeCode(ret.Get(0))
	Log("NewArrayValue: getTypeCode said " + fmt.Sprint(ret.TypeCode) + "\n")
	if ret.TypeCode == CUSTOM {
		ret.CustomTy = vf.GetCustomStructType(reflect.TypeOf(ret.Get(0)))
	}
	return ret
}

func ToArrayValue(o interface{}, vf ValueFactory) *ArrayValue {
	av := NewArrayValue(o, vf)
	return av
}

func FromArrayValue(av *ArrayValue) interface{} {
	return  av.ArrayVal.Interface()
}

func (ar *ArrayValue) Size() int {
	return ar.ArrayVal.Len()
}

func (ar *ArrayValue) Get(i int) interface{} {
	return ar.ArrayVal.Index(i).Interface()
}

func (ar *ArrayValue) Set(i int, value interface{}) {
	ar.ArrayVal.Index(i).Set(reflect.ValueOf(ToAssignableValue(ar.ArrayVal.Index(i),value)))
}


func getTypeCode(val interface{}) int8 {
	if val == nil {
		return NULL
	}

	switch a := val.(type) {
	case []byte:
		return BYTES
	}

	if reflect.TypeOf(val).Kind() == reflect.Slice {
		return ARRAY
	}

	switch a := val.(type) {
	case int8:
		return BYTE
	case int16:
		return SHORT
	case int32:
		return INT
	case int64:
		return LONG
	case float32:
		return FLOAT
	case float64:
		return DOUBLE
	case string:
		return STRING
	default:
		return CUSTOM
	}
	return NONE

}


func checkValue(val interface{}) int8 {
	if val == nil {
		return NULL
	}
	if val == NONE {
		return NONE
	}


	switch a := val.(type) {
	case []byte:
		return BYTES
	}

	if reflect.TypeOf(val).Kind() == reflect.Slice {
		return ARRAY
	}

	switch a := val.(type) {
	case int8:
		return checkByte(val.(int8))
	case int16:
		return checkShort(val.(int16))
	case int32:
		return checkInteger(val.(int32))
	case int64:
		return checkLong(val.(int64))
	case float32:
		return FLOAT
	case float64:
		return DOUBLE
	case string:
		return STRING
	default:
		return CUSTOM
	}
	return NONE

}


func checkLong(val int64) int8 {
	if val <= 4294967295 &&
		val >= -4294967296 {
		return checkInteger(int32(val))
	}
	return LONG
}

func checkInteger(val int32) int8 {
	if val <= 65535 &&
		val >= -65536 {
		return checkShort(int16(val))
	}
	return INT
}


func checkShort(val int16) int8 {
	if val <= 127 &&
		val >= -128 {
		return checkByte(int8(val))
	}
	return SHORT
}

func checkByte(val int8) int8 {
	if val <= MAX_TINY_INT &&
		val >= MIN_TINY_INT {
		return val
	}
	return BYTE
}

func toBytes(val string) []byte {
	return []byte(val)
}

func toByte(val interface{}) int8 {
	switch val.(type) {
	case int:
		return int8(val.(int))
	case int8:
		return val.(int8)
	case int16:
		return int8(val.(int16))
	case int32:
		return int8(val.(int32))
	case int64:
		return int8(val.(int64))
	}
	return 0
}

func toShort(val interface{}) int16 {
	switch val.(type) {
	case int:
		return int16(val.(int))
	case int8:
		return int16(val.(int8))
	case int16:
		return int16(val.(int16))
	case int32:
		return int16(val.(int32))
	case int64:
		return int16(val.(int64))
	}
	return 0
}

func toInteger(val interface{}) int32 {
	switch val.(type) {
	case int:
		return int32(val.(int))
	case int8:
		return int32(val.(int8))
	case int16:
		return int32(val.(int16))
	case int32:
		return int32(val.(int32))
	case int64:
		return int32(val.(int64))
	}
	return 0
}
