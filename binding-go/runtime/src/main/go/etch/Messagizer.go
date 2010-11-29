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

type Messagizer struct {
	buf       *FlexBuffer
	transport TransportPacket
	sess      SessionMessage
	tdi       TaggedDataInput
	tdo       TaggedDataOutput
}

func NewMessagizer(vf ValueFactory, transport TransportPacket) *Messagizer {
	buf := NewFlexBuffer()

	return &Messagizer{buf, transport, nil, NewBinaryTaggedDataInput(vf), NewBinaryTaggedDataOutput(vf)}

}

func (m *Messagizer) SessionQuery(obj interface{}) interface{} {
	return m.sess.SessionQuery(obj)
}

func (m *Messagizer) SessionControl(ctrl interface{}, value interface{}) interface{} {
	return m.sess.SessionControl(ctrl, value)
}

func (m *Messagizer) SessionNotify(obj interface{}) {
	m.sess.SessionNotify(obj)
}

func (m *Messagizer) GetSession() Session {
	return m.sess
}

func (m *Messagizer) SetSession(sess Session) {
	m.sess = sess.(SessionMessage)
}

func (m *Messagizer) TransportQuery(obj interface{}) interface{} {
	return m.transport.TransportQuery(obj)
}

func (m *Messagizer) TransportControl(ctrl interface{}, value interface{}) {
	Log("Messagizer: transportControl: " + fmt.Sprint(ctrl) + "\n")
	m.transport.TransportControl(ctrl, value)
}

func (m *Messagizer) TransportNotify(value interface{}) {
	m.transport.TransportNotify(value)
}

func (m *Messagizer) TransportMessage(recipient interface{}, msg *Message) {
	//leave space for header
	m.buf.Index = 8
	m.tdo.WriteMessage(msg, m.buf)
	m.buf.Index = 0
	m.transport.TransportPacket(recipient, m.buf)
}

func (m *Messagizer) SessionPacket(sender interface{}, buf *FlexBuffer) {
	msg := m.tdi.ReadMessage(buf)
	m.sess.SessionMessage(sender, msg)
}
