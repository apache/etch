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

type Message struct {
	StructValue
}

func NewMessage(ty *Type, vf ValueFactory, length int) *Message {
	return &Message{StructValue{ty, vf, length, map[*Field]interface{}{}}}
}

func (msg *Message) ReplyWith(rty *Type) *Message {
	//8 is a magic number from Java Binding, investigate further
	rmsg := NewMessage(rty, msg.Vf, 8)
	rmsg.Vf.SetInReplyTo(rmsg, rmsg.Vf.GetMessageId(msg))
	return rmsg
}

func (msg *Message) Reply() *Message {
	return msg.ReplyWith(msg.Ty.RType)
}
