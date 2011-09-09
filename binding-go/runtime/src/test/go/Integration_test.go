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
import "time"

type dummyValueFactory struct {
	ValueFactory
	Mf_f1 *Field
	Mf_f2 *Field
}

func NewDummyValueFactory() *dummyValueFactory {
	ret := &dummyValueFactory{NewDefaultValueFactory(), nil, nil}

	var ty *Type = NewTypeByName("type1")
	var rty *Type = NewTypeByName("returntype1")

	ret.Mf_f1 = NewField("field1")
	ret.Mf_f2 = NewField("field2")

	dv2 := new(dummyVal)
	ty.PutValidator(ret.Mf_f1, dv2)
	ty.PutValidator(ret.Mf_f2, dv2)
	ty.PutValidator(ret.GetDefaultMessageIdField(), dv2)
	ty.RType = rty

	rty.ResponseField = ret.GetDefaultResponseField()
	rty.PutValidator(ret.GetDefaultResponseField(), dv2)
	rty.PutValidator(ret.GetDefaultInReplyToField(), dv2)
	rty.PutValidator(ret.GetDefaultMessageIdField(), dv2)

	ret.AddType(ty)
	ret.AddType(rty)

	return ret
}

//*****************************************************************************************

type dummyServerFactory struct {
	session   Session
	transport Transport
}

func NewDummyServerFactory(transport Transport) *dummyServerFactory {
	val := &dummyServerFactory{nil, transport}
	return val
}


func (dsf *dummyServerFactory) NewServer(connection *TcpConnection) {
	Log("dummyServerFactory: newServer\n")
	ptzr := NewPacketizer(connection, 10*1024)
	connection.SetSession(ptzr)
	dvf := NewDummyValueFactory()
	msgzr := NewMessagizer(dvf, ptzr)
	ptzr.SetSession(msgzr)
	mbm := NewPlainMailboxManager(nil, msgzr)
	msgzr.SetSession(mbm)
	dsvc := NewDefaultDeliveryService(mbm, nil)
	mbm.SetSession(dsvc)
	stub := NewEtchStub(dsvc, nil)
	dsvc.SetSession(stub)
	stub.Impl = new(dummyImpl)

	dvf.GetTypeByName("type1").StubHelper = new(dummyHelper)

	Log("dummyServerFactory: STARTING TRANSPORT CONTROL\n")
	//no wait up on server
	dsvc.TransportControl(START, 4000)
}


func (dsf *dummyServerFactory) SessionQuery(obj interface{}) interface{} {
	return nil
}

func (dsf *dummyServerFactory) SessionControl(ctrl interface{}, value interface{}) interface{} {
	return nil
}

func (dsf *dummyServerFactory) SessionNotify(obj interface{}) {
	Log("dummyServerFactory sessionnotify\n")
}

func (dsf *dummyServerFactory) GetSession() Session {
	return nil
}

func (dsf *dummyServerFactory) SetSession(sess Session) {

}

func (dsf *dummyServerFactory) TransportQuery(obj interface{}) interface{} {
	return nil
}

func (dsf *dummyServerFactory) TransportControl(ctrl interface{}, value interface{}) {
	Log("dummyServerFactory transportcontrol\n")
	dsf.transport.TransportControl(ctrl, value)
}

func (dsf *dummyServerFactory) TransportNotify(value interface{}) {

}

//****************************************************

func toInteger(val interface{}) int32 {
	switch val.(type) {
	case int:
		return int32(val.(int))
	case int8:
		return int32(val.(int8))
	case int16:
		return int32(val.(int16))
	case int32:
		return int32(val.(int32))
	case int64:
		return int32(val.(int64))
	}
	return 0
}

type dummyHelper struct{}

func (dh *dummyHelper) Run(svc DeliveryService, impl interface{}, sender interface{}, msg *Message) {
	Log("dummyHelper got message: " + fmt.Sprint(msg) + "\n")
	rmsg := msg.Reply()
	di := impl.(*dummyImpl)
	f1 := msg.Vf.(*dummyValueFactory).Mf_f1
	f2 := msg.Vf.(*dummyValueFactory).Mf_f2

	ret := di.dummyFunc(toInteger(msg.Fields[f1]), toInteger(msg.Fields[f2]))
	rmsg.Put(msg.Vf.GetDefaultResponseField(), ret)
	svc.TransportMessage(sender, rmsg)
}

type dummyImpl struct{}

func (di *dummyImpl) dummyFunc(param1 int32, param2 int32) int32 {
	Log("HEUREKA, this is the call\n")
	return param1 + param2
}


func TestIntegration() {
	//start server
	Log("****************************************************** STARTING SERVER\n")
	tl := NewTcpListener("127.0.0.1:4001")
	dsl := NewDefaultSessionListener(tl)
	tl.SetSession(dsl)
	sf := NewDummyServerFactory(dsl)
	dsl.SetSession(sf)
	sf.TransportControl(START_AND_WAIT_UP, 4000)
	Log("****************************************************** STARTING SERVER DONE\n")
	Log("****************************************************** INITIALIZING CLIENT\n")

	//start client	
	tcpClientConn := NewTcpConnection("127.0.0.1:4001", false, nil, nil)
	_, cdvf, cdsvc := buildStack(tcpClientConn)
	Log("****************************************************** STARTING CLIENT TRANSPORT CONTROL WAITUP\n")
	cdsvc.TransportControl(START_AND_WAIT_UP, 4000)
	Log("****************************************************** STARTING CLIENT TRANSPORT CONTROL WAITUP DONE\n")
	msg := NewMessage(cdvf.GetTypeByName("type1"), cdvf, 2)
	msg.Put(cdvf.(*dummyValueFactory).Mf_f1, int8(55))
	msg.Put(cdvf.(*dummyValueFactory).Mf_f2, int8(56))

	start := time.Nanoseconds()
	for i := 0; i < 10; i++ {
		doCall(msg, cdsvc, cdvf.GetTypeByName("returntype1"), i)
	}
	end := time.Nanoseconds()
	fmt.Printf("10 calls took " + fmt.Sprint((end-start)/1e6) + " ms\n")
	Log("Integration Test done\n")
}


func doCall(msg *Message, cdsvc DeliveryService, rty *Type, i int) {
	Log("****************************************************** " + fmt.Sprint(i) + " CLIENT BEGIN CALL\n")
	mb := cdsvc.BeginCall(msg)

	if mb == nil {
		Error("IntegrationTest: BeginCall gives null mailbox\n")
	}

	Log("****************************************************** " + fmt.Sprint(i) + " CLIENT END CALL\n")
	retVal := cdsvc.EndCall(mb, rty)
	Log("****************************************************** " + fmt.Sprint(i) + " CLIENT END CALL DONE\n")
	if retVal != int8(111) {
		Error("IntegrationTest: Answer was wrong " + fmt.Sprint(retVal) + "\n")
	} else {
	Log("****************************************************** " + fmt.Sprint(i) + " CALL SUCCESS\n")
	}
}

func buildStack(conn *TcpConnection) (*EtchStub, ValueFactory, DeliveryService) {
	ptzr := NewPacketizer(conn, 10*1024)
	conn.SetSession(ptzr)
	dvf := NewDummyValueFactory()
	msgzr := NewMessagizer(dvf, ptzr)
	ptzr.SetSession(msgzr)
	mbm := NewPlainMailboxManager(nil, msgzr)
	msgzr.SetSession(mbm)
	dsvc := NewDefaultDeliveryService(mbm, nil)
	mbm.SetSession(dsvc)
	stub := NewEtchStub(dsvc, nil)
	dsvc.SetSession(stub)
	return stub, dvf, dsvc
}
