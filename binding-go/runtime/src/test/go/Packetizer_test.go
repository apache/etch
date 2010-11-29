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
import "bytes"

func TestPacketizer() {
	dt := new(dummyTransport)
	ds := new(dummySessionPacket)

	ptzr := NewPacketizer(dt, 10*1024)
	ptzr.SetSession(ds)
	fb := NewFlexBuffer()
	//skip header
	fb.Index = 8
	fb.PutBytes([]uint8{1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
	fb.Index = 0
	ptzr.TransportPacket(1, fb)

	dt.Buf.Index = 0
	retBytes := make([]uint8, 18)
	dt.Buf.ReadBytes(&retBytes)
	Log("bytes from Packetizer->Transport: " + fmt.Sprint(retBytes) + "\n")

	if bytes.Compare(retBytes[0:4], []uint8{0xde, 0xad, 0xbe, 0xef}) != 0 {
		Error("Error in Packetizer: wrong sig\n")
	}

	if bytes.Compare(retBytes[4:8], []uint8{0, 0, 0, 10}) != 0 {
		Error("Error in Packetizer: wrong length " + fmt.Sprint(retBytes[4:8]) +"\n")
	}

	fbs := NewFlexBuffer()
	fbs.PutSBytes([]int8{-34, -83, -66, -17, 0, 0, 0, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
	fbs.Index = 0
	ptzr.SessionData(1, fbs)

	retBytes2 := make([]uint8, 10)
	ds.Buf.ReadBytes(&retBytes2)
	//{-34,-83,-66,-17,0,0,0,10,1,2,3,4,5,6,7,8,9,10}
	Log("bytes from Packetizer->Session: " + fmt.Sprint(retBytes2) + "\n")

	if bytes.Compare(retBytes2[0:10], []uint8{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) != 0 {
		Error("Error in Packetizer: wrong packet\n")
	}

}

type dummySessionPacket struct {
	Buf *FlexBuffer
}

func (ds *dummySessionPacket) SessionPacket(sender interface{}, buf *FlexBuffer) {
	ds.Buf = buf
}

func (ds *dummySessionPacket) SessionQuery(obj interface{}) interface{} {
	return nil
}

func (ds *dummySessionPacket) SessionNotify(obj interface{}) {
}

func (ds *dummySessionPacket) SessionControl(ctrl interface{}, value interface{}) interface{} {
	return nil
}


type dummyTransport struct {
	Buf *FlexBuffer
}


func (dt *dummyTransport) TransportData(recipient interface{}, buf *FlexBuffer) {
	Log("DummyTransport TransportData\n")
	dt.Buf = buf
}

func (dt *dummyTransport) GetSession() Session {
	return nil
}

func (dt *dummyTransport) SetSession(sess Session) {
}

func (dt *dummyTransport) TransportQuery(obj interface{}) interface{} {
	return nil
}

func (dt *dummyTransport) TransportControl(ctrl interface{}, value interface{}) {
}

func (dt *dummyTransport) TransportNotify(value interface{}) {
}
