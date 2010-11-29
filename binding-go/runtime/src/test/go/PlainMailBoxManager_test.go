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


func TestPlainMailBoxManager() {
	dt := new(dummyTransportMessage)
	ds := new(dummySessionMessage)

	mbm := NewPlainMailboxManager(ds, dt)

	dvf := NewDefaultValueFactory()
	ty := NewTypeByName("type1")
	f1 := NewField("field1")
	f2 := NewField("field2")
	dv1 := new(dummyVal)
	dv2 := new(dummyVal)
	ty.PutValidator(f1, dv1)
	ty.PutValidator(f2, dv2)
	msg := NewMessage(ty, dvf, 2)
	msg.Put(f1, int8(55))
	msg.Put(f2, int8(56))

	mb := mbm.TransportCall(23, msg)

	if mb == nil {
		Error("PlainMailboxManager: Got null mailbox on transportcall\n")
	}

	msgIDmb := mb.GetMessageId()
	msgIDmsg := msg.Vf.GetMessageId(msg)

	//this relies on this test being the first one to call nextId...
	if msgIDmb != msgIDmsg || msgIDmb != 43 {
		Error("PlainMailboxManager: Got wrong message id in transportcall, " + fmt.Sprint(msgIDmb) + " vs. " + fmt.Sprint(msgIDmsg) + "\n")
	}

	msg2 := msg.Reply()
	mbm.SessionMessage(44, msg2)

	elem := mb.Read()

	if elem.Msg != msg2 {
		Error("PlainMailboxManager: Got wrong reply\n")
	}

	Log("TestPlainMailBoxManager done. \n")
}


type dummySessionMessage struct {
	msg *Message
}

func (ds *dummySessionMessage) SessionMessage(sender interface{}, msg *Message) bool {
	Log("dummySessionMessage sessionmessage\n")
	ds.msg = msg
	return true
}

func (ds *dummySessionMessage) SessionQuery(obj interface{}) interface{} {
	return nil
}

func (ds *dummySessionMessage) SessionNotify(obj interface{}) {
}

func (ds *dummySessionMessage) SessionControl(ctrl interface{}, value interface{}) interface{} {
	return nil
}


type dummyTransportMessage struct{}

func (dt *dummyTransportMessage) GetSession() Session {
	return nil
}

func (dt *dummyTransportMessage) SetSession(sess Session) {
}


func (dt *dummyTransportMessage) TransportMessage(recipient interface{}, msg *Message) {

}

func (dt *dummyTransportMessage) TransportQuery(obj interface{}) interface{} {
	return nil
}

func (dt *dummyTransportMessage) TransportControl(ctrl interface{}, value interface{}) {
}

func (dt *dummyTransportMessage) TransportNotify(value interface{}) {
}
