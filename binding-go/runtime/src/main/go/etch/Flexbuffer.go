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

import "encoding/binary"
import "os"
import "sync"
//import "fmt"

type FlexBuffer struct {
	buf       []byte
	Index     int
	Length    int
	mutex     *sync.Mutex
}

func NewFlexBuffer() *FlexBuffer {
	return &FlexBuffer{make([]byte, 32), 0, 0, new(sync.Mutex)}
}

func (fb *FlexBuffer) Write(p []byte) (n int, err os.Error) {
	fb.mutex.Lock()
//	Log("Flexbuffer: BeginWrite: " + fmt.Sprint(p) + " " + fmt.Sprint(fb.buf) + " length  " + fmt.Sprint(fb.Length) + " index " + fmt.Sprint(fb.Index) + " datalen " + fmt.Sprint(len(p)) + "\n")
	fb.ensureLength(fb.Index + len(p))
//	Log("Flexbuffer: MiddleWrite: " + fmt.Sprint(p) + " " + fmt.Sprint(fb.buf) + " length  " + fmt.Sprint(fb.Length) + " index " + fmt.Sprint(fb.Index) + " datalen " + fmt.Sprint(len(p)) + "\n")
	copy(fb.buf[fb.Index:fb.Index + len(p)],p)
	fb.Index = fb.Index + len(p)	
	fb.fixLength()
//	Log("Flexbuffer: EndWrite: " + fmt.Sprint(p) + " " + fmt.Sprint(fb.buf) + " length  " + fmt.Sprint(fb.Length) + " index " + fmt.Sprint(fb.Index) + " datalen " + fmt.Sprint(len(p)) + "\n")
//	Log("Flexbuffer: EndWrite: " + fmt.Sprint(fb.avail()) + "\n")
	fb.mutex.Unlock()
	return len(p), nil
}

func (fb *FlexBuffer) ensureLength(newLength int) {
	if  newLength < len(fb.buf) {
		return
	}
	fb.enlargeBuffer(newLength)
}

func (fb *FlexBuffer) enlargeBuffer(desired int) {
	l := len(fb.buf)
	for l < desired {
		l = l * 2
	}
//	Log("New BUFFER SIZE " + fmt.Sprint(l) + "\n")
	newBuf := make([]byte, l)
	copy(newBuf, fb.buf)
	fb.buf = newBuf
}

func (fb *FlexBuffer) fixLength() {
	if(fb.Index > fb.Length) {
		fb.Length = fb.Index
	}
}

func (fb *FlexBuffer) Avail() int {
	return fb.avail()
}

func (fb *FlexBuffer) avail() int {
	return fb.Length - fb.Index
}

func (fb *FlexBuffer) Read(p []byte) (n int, err os.Error) {
	fb.mutex.Lock()

//	Log("Flexbuffer: BeginRead: " + fmt.Sprint(p) + " " + fmt.Sprint(fb.buf) + " " + fmt.Sprint(fb.Length) + " " + fmt.Sprint(fb.Index) + " " + fmt.Sprint(len(p)) + "\n")
	
	toRead := len(p)
	if fb.avail() < toRead {
		toRead = fb.avail()
	}

//	Log("Flexbuffer: MiddleRead: " + fmt.Sprint(p) + " " + fmt.Sprint(fb.buf) + " " + fmt.Sprint(fb.Length) + " " + fmt.Sprint(fb.Index) + " " + fmt.Sprint(toRead) + "\n")

	copy(p, fb.buf[fb.Index:fb.Index+toRead])
	fb.Index = fb.Index + toRead

//	Log("Flexbuffer: EndRead: " + fmt.Sprint(p) + " " + fmt.Sprint(fb.buf) + " " + fmt.Sprint(fb.Length) + " " + fmt.Sprint(fb.Index) + " " + fmt.Sprint(toRead) + "\n")
	fb.mutex.Unlock()
	return toRead, nil
}

func (fb *FlexBuffer) PutFromOtherFull(other *FlexBuffer) {
//	Log("Flexbuffer.PutFromOtherFull " + fmt.Sprint(other.avail()) + "\n")
	fb.PutFromOther(other, int32(other.avail()))
}

func (fb *FlexBuffer) PutFromOther(other *FlexBuffer, l int32) {
//	Log("Flexbuffer.PutFromOther " + fmt.Sprint(l) + " " + fmt.Sprint(other) + "\n")
	buf := make([]byte, l)
	other.Read(buf)
	fb.Write(buf)
}

func (fb *FlexBuffer) Put(val interface{}) {
	binary.Write(fb, binary.BigEndian, val)
}

func (fb *FlexBuffer) PutByte(b int8) {
	binary.Write(fb, binary.BigEndian, b)
}

func (fb *FlexBuffer) PutInt(i int32) {
	binary.Write(fb, binary.BigEndian, i)
}

func (fb *FlexBuffer) PutShort(s int16) {
	binary.Write(fb, binary.BigEndian, s)
}

func (fb *FlexBuffer) PutLong(l int64) {
	binary.Write(fb, binary.BigEndian, l)
}

func (fb *FlexBuffer) PutFloat(f float32) {
	binary.Write(fb, binary.BigEndian, f)
}

func (fb *FlexBuffer) PutDouble(d float64) {
	binary.Write(fb, binary.BigEndian, d)
}

func (fb *FlexBuffer) PutBytes(bytes []uint8) {
	binary.Write(fb, binary.BigEndian, bytes)
}

func (fb *FlexBuffer) PutSBytes(bytes []int8) {
	binary.Write(fb, binary.BigEndian, bytes)
}

func (fb *FlexBuffer) ReadByte() int8 {
	ret := new(int8)
	binary.Read(fb, binary.BigEndian, ret)
	return *ret
}

func (fb *FlexBuffer) ReadInt() int32 {
	ret := new(int32)
	binary.Read(fb, binary.BigEndian, ret)
	return *ret
}

func (fb *FlexBuffer) ReadShort() int16 {
	ret := new(int16)
	binary.Read(fb, binary.BigEndian, ret)
	return *ret
}

func (fb *FlexBuffer) ReadLong() int64 {
	ret := new(int64)
	binary.Read(fb, binary.BigEndian, ret)
	return *ret
}

func (fb *FlexBuffer) ReadFloat() float32 {
	ret := new(float32)
	binary.Read(fb, binary.BigEndian, ret)
	return *ret
}

func (fb *FlexBuffer) ReadDouble() float64 {
	ret := new(float64)
	binary.Read(fb, binary.BigEndian, ret)
	return *ret
}

func (fb *FlexBuffer) ReadBytes(bytes *[]uint8) {
	binary.Read(fb, binary.BigEndian, bytes)
}

func (fb *FlexBuffer) ReadSBytes(bytes *[]int8) {
	binary.Read(fb, binary.BigEndian, bytes)
}
