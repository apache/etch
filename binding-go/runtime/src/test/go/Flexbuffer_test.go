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

func TestFlexBuffer() {
	fb := NewFlexBuffer()

	fb.Index = 0
	fb.Length = 0
	fb.PutInt(42)

	fb.Index = 0
	if fb.Avail() != 4 {
		Error("Error in Flexbuffer avail int " + fmt.Sprint(fb.Avail()) + "\n")
	}
	resInt := fb.ReadInt()
	if resInt != 42 {
		Error("Error in Flexbuffer int " + fmt.Sprint(resInt) + "\n")
	}

	fb.Index = 0
	fb.Length = 0
	fb.PutShort(int16(42))

	fb.Index = 0
	if fb.Avail() != 2 {
		Error("Error in Flexbuffer avail short " + fmt.Sprint(fb.Avail()) + "\n")
	}
	resShort := fb.ReadShort()
	if resShort != int16(42) {
		Error("Error in Flexbuffer short")
	}

	fb.Index = 0
	fb.Length = 0
	fb.PutLong(int64(42))

	fb.Index = 0
	if fb.Avail() != 8 {
		Error("Error in Flexbuffer avail long " + fmt.Sprint(fb.Avail()) + "\n")
	}
	resLong := fb.ReadLong()
	if resLong != int64(42) {
		Error("Error in Flexbuffer long")
	}

	fb.Index = 0
	fb.Length = 0
	fb.PutFloat(float32(42.0))


	fb.Index = 0
	if fb.Avail() != 4 {
		Error("Error in Flexbuffer avail float " + fmt.Sprint(fb.Avail()) + "\n")
	}
	resFloat := fb.ReadFloat()
	if resFloat != float32(42.0) {
		Error("Error in Flexbuffer float")
	}

	fb.Index = 0
	fb.Length = 0
	fb.PutDouble(float64(42.0))


	fb.Index = 0
	if fb.Avail() != 8 {
		Error("Error in Flexbuffer avail double" + fmt.Sprint(fb.Avail()) + "\n")
	}
	resDouble := fb.ReadDouble()
	if resDouble != float64(42.0) {
		Error("Error in Flexbuffer double")
	}

	fb.Index = 0
	fb.Length = 0
	fb.PutBytes([]uint8{1, 2, 3})

	fb.Index = 0
	if fb.Avail() != 3 {
		Error("Error in Flexbuffer avail bytes " + fmt.Sprint(fb.Avail()) + "\n")
	}

	retBytes := make([]uint8, 3)
	fb.ReadBytes(&retBytes)
	if retBytes[0] != 1 ||
		retBytes[1] != 2 ||
		retBytes[2] != 3 {
		Error("Error in Flexbuffer bytes")
	}

//	fb.Index = 0
//	for i := 0; i < 2048; i++ {
//		fb.PutByte(int8(i))
//	}
//
//	fb.Index = 0
//	if fb.Avail() != 2048 {
//		Error("Error in Flexbuffer avail " + fmt.Sprint(fb.Avail()) + "\n")
//	}
//	for i := 0; i < 2048; i++ {
//		res := fb.ReadByte()
//		if res != int8(i) {
//			Error("Error in Flexbuffer: " + fmt.Sprint(i) + ": " + fmt.Sprint(res) + "\n")
//		}
//
//	}
//
	Log("TestFlexBuffer done\n")
}
