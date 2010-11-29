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

import "net"

type DeliveryService interface {
	SessionMessage
	TransportMessage
	BeginCall(msg *Message) Mailbox
	EndCall(mb Mailbox, ty *Type) interface{}
}

type MailboxManager interface {
	SessionMessage
	TransportMessage
	TransportCall(recipient interface{}, msg *Message) Mailbox
	Unregister(mb Mailbox)
	Redeliver(sender interface{}, msg *Message)
}

type ServerFactory interface {
	Session
	Transport
	NewServer(conn *TcpConnection)
}

type SessionListener interface {
	Session
	SessionAccepted(conn *net.TCPConn) bool
}

type SessionMessage interface {
	Session
	SessionMessage(sender interface{}, msg *Message) bool
}

type SessionPacket interface {
	Session
	SessionPacket(sender interface{}, buf *FlexBuffer)
}

type SessionData interface {
	Session
	SessionData(sender interface{}, buf *FlexBuffer)
}

type TransportMessage interface {
	Transport
	TransportMessage(recipient interface{}, msg *Message)
}

type TransportPacket interface {
	Transport
	TransportPacket(recipient interface{}, buf *FlexBuffer)
}

type TransportData interface {
	Transport
	TransportData(recipient interface{}, buf *FlexBuffer)
}

type Session interface {
	SessionQuery(obj interface{}) interface{}
	SessionControl(ctrl interface{}, value interface{}) interface{}
	SessionNotify(obj interface{})
}

type Transport interface {
	GetSession() Session
	SetSession(sess Session)
	TransportQuery(obj interface{}) interface{}
	TransportControl(ctrl interface{}, value interface{})
	TransportNotify(value interface{})
}
