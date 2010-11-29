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
package goetchtest

import "fmt"
import . "etch"

type dummyVal struct{}

func (dv *dummyVal) Validate(value interface{}) bool {
	return true
}
func (dv *dummyVal) ValidateValue(value interface{}) interface{} {
	return value
}

func TestType() {
	t1 := NewType(1, "type1")
	f1 := &Field{IdName{1, "field1"}}
	f2 := &Field{IdName{2, "field2"}}
	dv1 := new(dummyVal)
	dv2 := new(dummyVal)
	dv3 := new(dummyVal)

	t1.PutValidator(f1, dv1)
	t1.PutValidator(f2, dv2)
	t1.PutValidator(f2, dv3)

	i := int32(1)
	for k, v := range t1.Fields {
		if k != i {
			Error("got wrong key")
		}
		if v.Id != i {
			Error("got wrong value")
		}
		i = i + 1
	}

	if i != 3 {
		Error("wrong number of Validators: " + fmt.Sprint(i))
	}

	t1.ClearValidator(f1)
	v := t1.Validators[f1.Id]
	if v != nil {
		Error("clear Validator not working")
	}

	v2 := t1.Validators[f2.Id]
	if v2 == nil {
		Error("Validator for f2 erased")
	}
	if v2 != dv3 {
		Error("wrong Validator for f2")
	}
	Log("TestType done\n")
}
