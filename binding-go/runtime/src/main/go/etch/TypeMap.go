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

type TypeMap struct {
	byId   map[int32]*Type
	byName map[string]*Type
}

func NewTypeMap() *TypeMap {
	return &TypeMap{map[int32]*Type{}, map[string]*Type{}}
}

func (tm *TypeMap) GetById(id int32) *Type {
	Log("TypeMap GetById " + fmt.Sprint(id) + " in: " + fmt.Sprint(tm.byId) + "\n")
	val := tm.byId[id]
	return val
}

func (tm *TypeMap) GetByName(name string) *Type {
	//Log("TypeMap GetByName " + name + " in: " + fmt.Sprint(tm.byName) + "\n")

	val := tm.byName[name]
	if val == nil {
		Log("TypeMap adding new Type " + name + "\n")
		val, _ = tm.Add(tm.MakeNew(name))
	}
	return val
}

func (tm *TypeMap) GetByNameNoAdd(name string) *Type {
	return tm.byName[name]
}


func (tm *TypeMap) Add(value *Type) (*Type, string) {
	if tm.byId[value.Id] != nil {
		return nil, "id collision"
	}
	if tm.byName[value.Name] != nil {
		return nil, "name collision"
	}

	Log("TypeMap adding " + fmt.Sprint(value.Id) + " " + value.Name + "\n")
	tm.byId[value.Id] = value
	tm.byName[value.Name] = value
	return value, "ok"
}

func (tm *TypeMap) MakeNew(name string) *Type {
	return NewType(hash(name), name)
}

func (tm *TypeMap) GetAll() []*Type {
	ret := make([]*Type, len(tm.byId))
	i := 0
	for _, v := range tm.byId {
		ret[i] = v
		i++
	}
	return ret
}
