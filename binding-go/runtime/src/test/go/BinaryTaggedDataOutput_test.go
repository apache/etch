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

func TestBinaryTaggedDataOutput() {

	btdo := NewBinaryTaggedDataOutput(nil)

	ty := NewTypeByName("type1")

	f1 := NewField("field1")
	f2 := NewField("field2")

	dv1 := new(dummyVal)
	dv2 := new(dummyVal)

	ty.PutValidator(f1, dv1)
	ty.PutValidator(f2, dv2)

	msg := NewMessage(ty, nil, 2)
	msg.Put(f1, int8(55))
	msg.Put(f2, int8(56))

	buf := NewFlexBuffer()

	btdo.WriteMessage(msg, buf)

	// test written message


	buf.Index = 0
	buf.Length = 20
	retBytes := make([]int8, 20)
	buf.ReadSBytes(&retBytes)

	if retBytes[0] != 3 ||
		retBytes[1] != -122 ||
		retBytes[2] != 15 {
		Error("Error in BTDO")
	}
	Log(fmt.Sprint(retBytes) + "\n")

	Log("TestBinaryTaggedDataOutput done\n")
}
