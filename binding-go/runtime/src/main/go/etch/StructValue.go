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


type StructValueIntf interface {
	Put(field *Field, value interface{})
	GetType() *Type
	GetFields() *map[*Field]interface{}
	Size() int
}

type StructValue struct {
	Ty     *Type
	Vf     ValueFactory
	Length int
	Fields map[*Field]interface{}
}

func NewStructValue(ty *Type, vf ValueFactory, length int) *StructValue {
	return &StructValue{ty, vf, length, map[*Field]interface{}{}}
}

func (sv *StructValue) Size() int {
	return sv.Length
}

func (sv *StructValue) GetType() *Type {
	return sv.Ty
}

func (sv *StructValue) GetFields() *map[*Field]interface{} {
	return &(sv.Fields)
}

func (sv *StructValue) Put(field *Field, value interface{}) {
	if value == nil {
		sv.Fields[field] = nil
		return
	}
	val := sv.Ty.Validators[field.Id]
	if val != nil && val.Validate(value) {
		sv.Fields[field] = value
	}
}
