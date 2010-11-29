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

const (
	SIG         uint32 = uint32(0xdeadbeef)
	HEADER_SIZE int    = 8
)

type Packetizer struct {
	transport     TransportData
	maxPktSize    int
	saveBuf       *FlexBuffer
	nextPacketLen int
	session       SessionPacket
}

func NewPacketizer(transport TransportData, maxPktSize int) *Packetizer {
	return &Packetizer{transport, maxPktSize, NewFlexBuffer(), 0, nil}
}

func (p *Packetizer) SessionData(sender interface{}, buf *FlexBuffer) {
	Log("Packetizer.SessionPacket: available initially " + fmt.Sprint(p.saveBuf.avail()) + "\n")
	i := p.saveBuf.Index
	p.saveBuf.PutFromOtherFull(buf)
	p.saveBuf.Index = i

	if p.nextPacketLen != 0 {
		Log("Packetizer.SessionPacket: got new Packet\n")
		if int(p.saveBuf.avail()) > p.nextPacketLen {
			p.processPacket(sender)
		}
	} else {
		Log("Packetizer.SessionPacket: looking for header\n")
		if p.saveBuf.avail() >= HEADER_SIZE {
			Log("Packetizer.SessionPacket: got new header\n")
			p.nextPacketLen = p.processHeader()
			Log("Packetizer.SessionPacket: available after header " + fmt.Sprint(p.saveBuf.avail()) + "\n")

			if p.saveBuf.avail() >= p.nextPacketLen {
				p.processPacket(sender)
			}
		} else {
			Log("Packetizer.SessionPacket: not yet a header, only "+ fmt.Sprint(p.saveBuf.avail())+" bytes in savebuf\n")
		}
	}
}

func (p *Packetizer) processPacket(sender interface{}) {
	Log("Packetizer.processPacket\n")

	Log("Packetizer: old Index in processpaket " + fmt.Sprint(p.saveBuf.Index) + "\n")

	packet := make([]uint8, p.nextPacketLen)

	fullLength := p.saveBuf.Length
	currentPacketLength := p.saveBuf.Index + p.nextPacketLen
	p.saveBuf.Length = currentPacketLength

	p.saveBuf.ReadBytes(&packet)

	//there might be bytes after our full packet in the buffer
	p.saveBuf.Length = fullLength
	p.saveBuf.Index = currentPacketLength

	Log("Packetizer: resetting Index to " + fmt.Sprint(p.saveBuf.Index) + "\n")

	p.nextPacketLen = 0

	packetBuf := NewFlexBuffer()
	packetBuf.PutBytes(packet)
	packetBuf.Index = 0

	if p.session != nil {
		p.session.SessionPacket(sender, packetBuf)
	}
}


func (p *Packetizer) processHeader() int {
	//SIG, no check here :)
	p.saveBuf.ReadInt()
	//LENGTH
	l := p.saveBuf.ReadInt()
	Log("Packetizer.processHeader Header length " + fmt.Sprint(l) + "\n")
	return int(l)
}

func (p *Packetizer) SessionQuery(obj interface{}) interface{} {
	return p.session.SessionQuery(obj)
}

func (p *Packetizer) SessionControl(ctrl interface{}, value interface{}) interface{} {
	return p.session.SessionControl(ctrl, value)
}

func (p *Packetizer) SessionNotify(obj interface{}) {
	p.session.SessionNotify(obj)
}

func (p *Packetizer) TransportPacket(recipient interface{}, buf *FlexBuffer) {
	dataSize := buf.avail()
	pktSize := dataSize
	if pktSize > p.maxPktSize {
		return
	}
	i := SIG
	buf.PutInt(int32(i))
	buf.PutInt(int32(dataSize - HEADER_SIZE))
	buf.Index = 0
	p.transport.TransportData(recipient, buf)
}

func (p *Packetizer) GetSession() Session {
	return p.session.(Session)
}

func (p *Packetizer) SetSession(sess Session) {
	p.session = sess.(SessionPacket)
}

func (p *Packetizer) TransportQuery(obj interface{}) interface{} {
	return p.transport.TransportQuery(obj)
}

func (p *Packetizer) TransportControl(ctrl interface{}, value interface{}) {
	Log("Packetizer: transportControl: " + fmt.Sprint(ctrl) + "\n")
	p.transport.TransportControl(ctrl, value)
}

func (p *Packetizer) TransportNotify(value interface{}) {
	p.transport.TransportNotify(value)
}
