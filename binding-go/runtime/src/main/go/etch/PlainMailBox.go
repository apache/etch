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

type PlainMailBox struct {
	messageId int64
	queue     chan interface{}
}


func NewPlainMailBox(msgId int64) *PlainMailBox {
	ret := &PlainMailBox{msgId, nil}
	ret.queue = make(chan interface{}, 1)
	return ret
}

func (pmb *PlainMailBox) GetMessageId() int64 {
	return pmb.messageId
}


func (pmb *PlainMailBox) Message(sender interface{}, msg *Message) bool {
	pmb.queue <- &Element{sender, msg}
	return true

}

func (pmb *PlainMailBox) Read() *Element {
	val := WaitOnChannel(nil, 4000, pmb.queue)
	if val == nil {
		return nil
	}
	return val.(*Element)
}

func (pmb *PlainMailBox) CloseDelivery() bool {
	//this looses all queue contents, TODO
	close(pmb.queue)
	return true
}

func (pmb *PlainMailBox) CloseRead() bool {

	return pmb.CloseDelivery()
}
