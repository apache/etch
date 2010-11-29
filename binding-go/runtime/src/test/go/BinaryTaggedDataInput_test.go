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

import . "etch"
import "fmt"

func TestBinaryTaggedDataInput() {
	bys := []int8{3, -122, 15, 55, -87, 114, 2, -122, -10, -53, -14, -99, 56, -122, -10, -53, -14, -100, 55, -127}

	buf := NewFlexBuffer()
	buf.Put(bys)

	dvf := NewDefaultValueFactory()
	ty := dvf.GetTypeByName("type1")
	f1 := NewField("field1")
	f2 := NewField("field2")

	dv1 := new(dummyVal)
	dv2 := new(dummyVal)
	ty.PutValidator(f1, dv1)
	ty.PutValidator(f2, dv2)

	btdi := NewBinaryTaggedDataInput(dvf)
	
	buf.Index = 0
	// test written message
	msg := btdi.ReadMessage(buf)

	if msg.GetType().Id != ty.Id {
		Error("BTDI Error with msg id")
	}

	if msg.Length != 2 {
		Error("BTDI Error with msg length")
	}

	if msg.Fields[f1].(int8) != 55 {
		Error("BTDI Error with f1: " + fmt.Sprint(msg.Fields[f1]) + "\n")
	}

	if msg.Fields[f2].(int8) != 56 {
		Error("BTDI Error with f2: " + fmt.Sprint(msg.Fields[f2]) + "\n")
	}

	Log(fmt.Sprint(msg) + "\n")

	Log("TestBinaryTaggedDataInput done\n")
}
