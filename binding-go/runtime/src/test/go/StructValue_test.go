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

func TestStructValue() {
	t1 := NewType(1, "type1")
	f1 := &Field{IdName{1, "field1"}}
	f2 := &Field{IdName{2, "field2"}}
	dv := new(dummyVal)
	t1.PutValidator(f1, dv)
	sv := NewStructValue(t1, nil, 1)
	sv.Put(f1, "Hallo Welt\n")
	sv.Put(f2, "Hallo Welt2\n")

	if fmt.Sprint(sv.Fields[f1]) != "Hallo Welt\n" {
		Error("wrong value in field")
	}
	if sv.Fields[f2] != nil {
		Error("wrong value in field2")
	}
	Log("TestStructValue done\n")
}
