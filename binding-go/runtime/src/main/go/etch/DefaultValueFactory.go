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
	ETCH_RUNTIME_EXCEPTION_TYPE_NAME string = "_Etch_RuntimeException"
	ETCH_LIST_TYPE_NAME              string = "_Etch_List"
	ETCH_MAP_TYPE_NAME               string = "_Etch_Map"
	ETCH_SET_TYPE_NAME               string = "_Etch_Set"
	ETCH_DATETIME_TYPE_NAME          string = "_Etch_Datetime"
	ETCH_AUTH_EXCEPTION_TYPE_NAME    string = "_Etch_AuthException"
	ETCH_EXCEPTION_MESSAGE_NAME      string = "_exception"
	MSG_FIELD_NAME                   string = "msg"
	MESSAGE_ID_FIELD_NAME            string = "_messageId"
	IN_REPLY_TO_FIELD_NAME           string = "_inReplyTo"
	RESULT_FIELD_NAME                string = "result"
)

type DefaultValueFactory struct {
	Types                      *TypeMap
	Class2type                 *map[reflect.Type]*Type
	fieldMap                   map[string]*Field
	_mf_msg                    *Field
	_mf__messageId             *Field
	_mf__inReplyTo             *Field
	_mf_result                 *Field
	_mt__Etch_RuntimeException *Type
	_mt__Etch_List             *Type
	_mt__Etch_Map              *Type
	_mt__Etch_Set              *Type
	_mt__Etch_Datetime         *Type
	_mt__Etch_AuthException    *Type
	_mt__exception             *Type
}

func NewDefaultValueFactory() *DefaultValueFactory {
	ret := new(DefaultValueFactory)
	ret.Types = NewTypeMap()
	ret.Class2type = &(map[reflect.Type]*Type{})
	ret.fieldMap = map[string]*Field{}
	ret._mf_msg = NewField(MSG_FIELD_NAME)
	ret._mf__messageId = NewField(MESSAGE_ID_FIELD_NAME)
	ret._mf__inReplyTo = NewField(IN_REPLY_TO_FIELD_NAME)
	ret._mf_result = NewField(RESULT_FIELD_NAME)
	ret._mt__Etch_RuntimeException = ret.Types.GetByName(ETCH_RUNTIME_EXCEPTION_TYPE_NAME)
	ret._mt__Etch_List = ret.Types.GetByName(ETCH_LIST_TYPE_NAME)
	ret._mt__Etch_Map = ret.Types.GetByName(ETCH_MAP_TYPE_NAME)
	ret._mt__Etch_Set = ret.Types.GetByName(ETCH_SET_TYPE_NAME)
	ret._mt__Etch_Datetime = ret.Types.GetByName(ETCH_DATETIME_TYPE_NAME)
	ret._mt__Etch_AuthException = ret.Types.GetByName(ETCH_AUTH_EXCEPTION_TYPE_NAME)
	ret._mt__exception = ret.Types.GetByName(ETCH_EXCEPTION_MESSAGE_NAME)
	return ret
}

func (dvf *DefaultValueFactory) AddField(field *Field) {
	dvf.fieldMap[field.Name] = field
}

func (dvf *DefaultValueFactory) GetFieldByName(name string) *Field {
	return dvf.fieldMap[name]
}

func (dvf *DefaultValueFactory) GetClass2type() *map[reflect.Type]*Type {
	return dvf.Class2type
}


func (dvf *DefaultValueFactory) GetDefaultResponseField() *Field {
	return dvf._mf_result
}

func (dvf *DefaultValueFactory) GetDefaultInReplyToField() *Field {
	return dvf._mf__inReplyTo
}

func (dvf *DefaultValueFactory) GetDefaultMessageIdField() *Field {
	return dvf._mf__messageId
}


func (dvf *DefaultValueFactory) GetTypeById(id int32) *Type {
	ty := dvf.Types.GetById(id)
	return ty
}

func (dvf *DefaultValueFactory) GetTypeByName(name string) *Type {
	ty := dvf.Types.GetByName(name)
	return ty
}

func (dvf *DefaultValueFactory) AddType(ty *Type) {
	Log("DefaultValueFactory: adding type " + fmt.Sprint(ty) + "\n")
	dvf.Types.Add(ty)
}

func ToLong(val interface{}) int64 {
	switch val.(type) {
	case int:
		return int64(val.(int))
	case int8:
		return int64(val.(int8))
	case int16:
		return int64(val.(int16))
	case int32:
		return int64(val.(int32))
	case int64:
		return int64(val.(int64))
	}
	return 0
}

func (dvf *DefaultValueFactory) GetMessageId(theMsg *Message) int64 {
	return ToLong(theMsg.Fields[dvf._mf__messageId])
}

func (dvf *DefaultValueFactory) SetMessageId(msg *Message, msgId int64) {
	Log("DefaultValueFactory SETMESSAGEID " + fmt.Sprint(msgId) + "\n")
	msg.Fields[dvf._mf__messageId] = msgId
}

func (dvf *DefaultValueFactory) GetTypes() []*Type {
	tys := dvf.Types.GetAll()
	newslice := make([]*Type, len(tys))
	copy(newslice, tys)
	return newslice
}

func (dvf *DefaultValueFactory) GetInReplyTo(msg *Message) int64 {
	val := msg.Fields[dvf._mf__inReplyTo]
	Log("DefaultValueFactory GetInReplyTo looking in fields " + fmt.Sprint(msg.Fields) + "/" + fmt.Sprint(&(dvf._mf__inReplyTo)) + "\n")
	if val == nil {
		Log("DefaultValueFactory got nil \n")
		return 0
	}
	Log("DefaultValueFactory GetInReplyTo was " + fmt.Sprint(val) + "\n")
	return ToLong(val)
}

func (dvf *DefaultValueFactory) SetInReplyTo(msg *Message, msgId int64) {
	msg.Fields[dvf._mf__inReplyTo] = msgId
}

func (dvf *DefaultValueFactory) ExportCustomValue(value interface{}) *StructValue {
	ty := reflect.TypeOf(value)
	etchty := (*dvf.Class2type)[ty]
	if etchty.Helper != nil {
		return etchty.Helper.ExportValue(etchty, dvf, value)
	}
	return nil
}

func (dvf *DefaultValueFactory) ImportCustomValue(value *StructValue) interface{} {
	if value.Ty.Helper != nil {
		return value.Ty.Helper.ImportValue(value)
	}
	return nil
}

func (dvf *DefaultValueFactory) GetCustomStructType(obj reflect.Type) *Type {
	return (*dvf.Class2type)[obj]
}


func NewboolArray(length int32) []bool {
	return make([]bool, length)
}
func Newint8Array(length int32) []int8 {
	return make([]int8, length)
}
func Newint16Array(length int32) []int16 {
	return make([]int16, length)
}
func Newint32Array(length int32) []int32 {
	return make([]int32, length)
}
func Newint64Array(length int32) []int64 {
	return make([]int64, length)
}
func Newfloat32Array(length int32) []float32 {
	return make([]float32, length)
}
func Newfloat64Array(length int32) []float64 {
	return make([]float64, length)
}
func NewstringArray(length int32) []interface{} {
	return make([]interface{}, length)
}
func NewInterfaceArray(length int32) []interface{} {
	return make([]interface{}, length)
}

func (dvf *DefaultValueFactory)NewArrayForType(typeCode int8, cTy *Type, length int32) interface{}{
	switch typeCode {
	case BYTE:
		return Newint8Array(length)
	case SHORT:
		return Newint16Array(length)
	case INT:
		return Newint32Array(length)
	case LONG:
		return Newint64Array(length)
	case FLOAT:
		return Newfloat32Array(length)
	case DOUBLE:
		return Newfloat64Array(length)
	case STRING:
		return NewstringArray(length)
	case CUSTOM:
		return cTy.NewArray(length)
	}
	return nil
}