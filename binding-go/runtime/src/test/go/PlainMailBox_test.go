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

func TestPlainMailBox() {
	mb := NewPlainMailBox(42)

	if mb.GetMessageId() != 42 {
		Error("Mailbox has wrong ID\n")
	}

	msg := new(Message)

	sender := 23

	go TestPut(mb, msg, sender)

	elem := mb.Read()

	if elem == nil {
		Error("got nil elem from Mailbox\n")
	} else {
		if elem.Msg != msg {
			Error("got wrong elem from Mailbox\n")
		}

		if elem.Who != 23 {
			Error("got wrong sender from Mailbox\n")
		}
	}

	// enable this for build to take 4 seconds longer :)
	//Log("Waiting for Timeout...\n")
	//elem = mb.Read()

	//if elem != nil {
	//	Error("got an elem, but I expected nil\n")
	//}

	Log("TestPlainMailBox done\n")
}


func TestPut(mb Mailbox, msg *Message, sender interface{}) {
	mb.Message(sender, msg)
}
