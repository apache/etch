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

type DefaultDeliveryService struct {
	transport MailboxManager
	session   SessionMessage
	status    chan interface{}
}

func NewDefaultDeliveryService(transport MailboxManager, session SessionMessage) DeliveryService {
	return &DefaultDeliveryService{transport, session, make(chan interface{}, 1)}
}

func (dds *DefaultDeliveryService) BeginCall(msg *Message) Mailbox {
	return dds.transport.TransportCall(nil, msg)
}

func (dds *DefaultDeliveryService) handleError() {
	x := recover()
	Log("DefaultDeliveryService: Panic below: " + fmt.Sprint(x) + "\n")
}

func (dds *DefaultDeliveryService) EndCall(mb Mailbox, ty *Type) interface{} {
	elem := mb.Read()
	if elem != nil {
		msg := elem.Msg
		val := msg.GetFields()
		return (*val)[ty.ResponseField]
	}
	//e.g. timeout
	return nil
}

func (dds *DefaultDeliveryService) SessionMessage(sender interface{}, msg *Message) bool {
	return dds.session.SessionMessage(sender, msg)
}

func (dds *DefaultDeliveryService) TransportMessage(recipient interface{}, msg *Message) {
	dds.transport.TransportMessage(recipient, msg)
}

func (dds *DefaultDeliveryService) SessionQuery(obj interface{}) interface{} {
	return dds.session.SessionQuery(obj)
}

func (dds *DefaultDeliveryService) SessionControl(ctrl interface{}, value interface{}) interface{} {
	return dds.session.SessionControl(ctrl, value)
}

func (dds *DefaultDeliveryService) GetSession() Session {
	return dds.session
}

func (dds *DefaultDeliveryService) SetSession(sess Session) {
	dds.session = sess.(SessionMessage)
}

func (dds *DefaultDeliveryService) SessionNotify(obj interface{}) {
	
	defer EtchRecoverAll()
	if obj.(int) == UP {
		dds.status <- UP
	} else if obj.(int) == DOWN {
		dds.status <- DOWN
	}
	dds.session.SessionNotify(obj)
}

func (dds *DefaultDeliveryService) TransportQuery(obj interface{}) interface{} {
	return dds.transport.TransportQuery(obj)
}

func (dds *DefaultDeliveryService) waitUntil(value int, timeout int) {
	WaitOnChannel(value, timeout, dds.status)
}

func (dds *DefaultDeliveryService) TransportControl(ctrl interface{}, value interface{}) {
	Log("DefaultDeliveryService: transportControl: " + fmt.Sprint(ctrl) + "\n")
	defer EtchRecoverAll()
	if ctrl.(int) == START_AND_WAIT_UP {
		Log("DefaultDeliveryService: transportControl: START_AND_WAIT_UP\n")
		dds.transport.TransportControl(START, nil)
		Log("DefaultDeliveryService: WAITING FOR UP\n")
		dds.waitUntil(UP, value.(int))
		Log("DefaultDeliveryService: UP WAITING DONE\n")
	} else if ctrl.(int) == STOP_AND_WAIT_DOWN {
		Log("DefaultDeliveryService: transportControl: START_AND_WAIT_DOWN\n")
		dds.transport.TransportControl(STOP, nil)
		Log("DefaultDeliveryService: WAITING FOR DOWN\n")
		dds.waitUntil(DOWN, value.(int))
		Log("DefaultDeliveryService: DOWN WAITING DONE\n")
	} else {
		dds.transport.TransportControl(ctrl, value)
	}
}

func (dds *DefaultDeliveryService) TransportNotify(value interface{}) {
	dds.transport.TransportNotify(value)
}
