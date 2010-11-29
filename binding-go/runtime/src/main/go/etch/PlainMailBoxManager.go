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

type PlainMailboxManager struct {
	session   SessionMessage
	transport TransportMessage
	mailboxes map[int64]Mailbox
}

func NewPlainMailboxManager(session SessionMessage, transport TransportMessage) MailboxManager {
	return &PlainMailboxManager{session, transport, make(map[int64]Mailbox)}
}

var id int64 = int64(42)

func nextId() int64 {
	id = id + 1
	return id
}

func (pmb *PlainMailboxManager) TransportCall(recipient interface{}, msg *Message) Mailbox {
	msg.Vf.SetMessageId(msg, nextId())
	mb := NewPlainMailBox(msg.Vf.GetMessageId(msg))
	pmb.Register(mb)
	pmb.transport.TransportMessage(recipient, msg)
	return mb
}

func (pmb *PlainMailboxManager) TransportMessage(recipient interface{}, msg *Message) {
	msg.Vf.SetMessageId(msg, nextId())
	pmb.transport.TransportMessage(recipient, msg)
}

func (pmb *PlainMailboxManager) Unregister(mb Mailbox) {
	pmb.mailboxes[mb.GetMessageId()] = nil
}

func (pmb *PlainMailboxManager) Register(mb Mailbox) {
	pmb.mailboxes[mb.GetMessageId()] = mb
}

func (pmb *PlainMailboxManager) Redeliver(sender interface{}, msg *Message) {
	pmb.session.SessionMessage(sender, msg)
}

func (pmb *PlainMailboxManager) SessionMessage(sender interface{}, theMsg *Message) bool {

	if theMsg == nil { //e.g. type not known
		//discard message
		Log("PlainMailboxManager: discarding message\n")
		return false
	}

	Log("PlainMailboxManager: Got Msg with ID " + fmt.Sprint(theMsg.Vf.GetMessageId(theMsg)) + "\n")
	theMsgid := theMsg.Vf.GetInReplyTo(theMsg)
	Log("PlainMailboxManager: Msg is reply to " + fmt.Sprint(theMsgid) + "\n")

	if theMsgid != 0 {
		mb := pmb.mailboxes[theMsgid]
		if mb != nil {
			return mb.Message(sender, theMsg)
		}
		Log("PlainMailboxManager: did not find a mailbox for " + fmt.Sprint(theMsgid) + "\n")
		return false
	}
	return pmb.session.SessionMessage(sender, theMsg)
}


func (pmb *PlainMailboxManager) GetSession() Session {
	return pmb.session
}

func (pmb *PlainMailboxManager) SetSession(sess Session) {
	pmb.session = sess.(SessionMessage)
}

func (pmb *PlainMailboxManager) TransportQuery(obj interface{}) interface{} {
	return pmb.transport.TransportQuery(obj)
}

func (pmb *PlainMailboxManager) TransportControl(ctrl interface{}, value interface{}) {
	Log("PlainMailboxManager: transportControl: " + fmt.Sprint(ctrl) + "\n")
	pmb.transport.TransportControl(ctrl, value)
}

func (pmb *PlainMailboxManager) TransportNotify(value interface{}) {
	pmb.transport.TransportNotify(value)
}

func (pmb *PlainMailboxManager) SessionQuery(obj interface{}) interface{} {
	return pmb.session.SessionQuery(obj)
}

func (pmb *PlainMailboxManager) SessionControl(ctrl interface{}, value interface{}) interface{} {
	return pmb.session.SessionControl(ctrl, value)
}

func (pmb *PlainMailboxManager) SessionNotify(obj interface{}) {
	pmb.session.SessionNotify(obj)
}
