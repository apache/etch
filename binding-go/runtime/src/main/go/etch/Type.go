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

const (
	QUEUED    int = 0
	FREE      int = 0
	MODE_NONE int = 1
)

type Type struct {
	Id            int32
	Name          string
	Validators    map[int32]Validator
	Fields        map[int32]*Field
	RType         *Type
	Helper        ImportExportHelper
	StubHelper    StubHelper
	Timeout       int
	ResponseField *Field
	SuperType     *Type
	Direction     Direction
	AsyncMode     int
	NewArray      func(length int32) interface{}
}

func NewTypeByName(Name string) *Type {
	return NewType(hash(Name), Name)
}

func NewType(Id int32, Name string) *Type {
	return &Type{Id, Name, map[int32]Validator{}, map[int32]*Field{}, nil, nil, nil, 1, nil, nil, BOTH, MODE_NONE, nil}
}

func (ty *Type) PutValidator(field *Field, vldtr Validator) {
	exf := ty.Fields[field.Id]
	if exf == nil {
		ty.Fields[field.Id] = field
	}
	exv := ty.Validators[field.Id]
	if exv != nil {
		//ty.Validators[field] = newComboValidator(exv, vldtr)
		ty.Validators[field.Id] = vldtr
	}
	if exv == nil {
		ty.Validators[field.Id] = vldtr
	}
}

func (ty *Type) ClearValidator(field *Field) {
	ty.Validators[field.Id] = nil, false
}
