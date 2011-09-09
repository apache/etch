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

type StubHelper interface {
	Run(svc DeliveryService, impl interface{}, sender interface{}, msg *Message)
}

type EtchStub struct {
	svc  DeliveryService
	Impl interface{}
}

func NewEtchStub(svc DeliveryService, impl interface{}) *EtchStub {
	return &EtchStub{svc, impl}
}

func (es *EtchStub) SessionMessage(sender interface{}, theMsg *Message) bool {
	Log("EtchStub: Server got message\n")
	ty := theMsg.GetType()

	stubhelper := ty.StubHelper
	if stubhelper == nil {
		Log("EtchStub: no handler for " + fmt.Sprint(theMsg) + "\n")
		return false
	}

	execution := func() {
		stubhelper.Run(es.svc, es.Impl, sender, theMsg)
	}

	if ty.AsyncMode != MODE_NONE {
		go execution()
	} else {
		execution()
	}

	return true
}

func (es *EtchStub) SessionQuery(obj interface{}) interface{} {
	defer EtchRecoverAll()
	val := es.Impl.(Session)
	if val != nil {
		return val.SessionQuery(obj)
	}
	return nil
}


func (es *EtchStub) SessionControl(ctrl interface{}, value interface{}) interface{} {
	defer EtchRecoverAll()
	val := es.Impl.(Session)
	if val != nil {
		return val.SessionControl(ctrl, value)
	}
	return nil
}

func (es *EtchStub) SessionNotify(obj interface{}) {
	defer EtchRecoverAll()
	val := es.Impl.(Session)
	if val != nil {
		val.SessionNotify(obj)
	}
}
