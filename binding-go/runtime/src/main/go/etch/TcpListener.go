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
import "os"
import "fmt"

type TcpListener struct {
	address  string
	session  SessionListener
	status   int
	listener *net.TCPListener
}

func NewTcpListener(address string) *TcpListener {
	return &TcpListener{address, nil, DOWN, nil}
}

func (tcpl *TcpListener) openSocket() bool {
	var er os.Error = nil
	addr, _ := net.ResolveTCPAddr("tcp", tcpl.address)
	tcpl.listener, er = net.ListenTCP("tcp", addr)
	if er != nil {
		tcpl.status = DOWN
		return false
	}

	return true
}

func (tcpl *TcpListener) readSocket() {
	tcpl.GetSession().SessionNotify(UP)
	for tcpl.status != DOWN {
		Log("TcpListener before accept\n")
		connection, er := tcpl.listener.AcceptTCP()
		connection.SetLinger(0)
		if er != nil {
			Log("TcpListener DOWN\n")
			tcpl.status = DOWN
			break
		} else {
			tcpl.session.SessionAccepted(connection)
		}
	}
}

func (tcpl *TcpListener) closeSocket() {
	tcpl.listener.Close()
	tcpl.GetSession().SessionNotify(DOWN)
}

func (tcpl *TcpListener) GetSession() Session {
	return tcpl.session
}

func (tcpl *TcpListener) SetSession(sess Session) {
	tcpl.session = sess.(SessionListener)
}

func (tcpl *TcpListener) TransportQuery(obj interface{}) interface{} {
	//nothing to do (java binding allow query of addresses, TODO)
	return nil
}


func (tcpl *TcpListener) TransportControl(ctrl interface{}, value interface{}) {
	Log("TcpListener: transportControl: " + fmt.Sprint(ctrl) + "\n")
	if ctrl == START {
		go tcpl.start()
	} else if ctrl == STOP {
		tcpl.stop()
	}
}

func (tcpl *TcpListener) TransportNotify(value interface{}) {
	//nothing to do
}

func (tcpl *TcpListener) start() {
	ok := tcpl.openSocket()
	if ok == false {
		Log("TcpListener error opening listener socket\n")
		return
	}
	tcpl.status = UP
	tcpl.readSocket()
}

func (tcpl *TcpListener) stop() {
	tcpl.closeSocket()
}

//************************************************************************

type DefaultSessionListener struct {
	transport Transport
	session   ServerFactory
	status    chan interface{}
}

func NewDefaultSessionListener(trans Transport) *DefaultSessionListener {
	return &DefaultSessionListener{trans, nil, make(chan interface{}, 1)}
}

func (dsl *DefaultSessionListener) waitUntil(value int, timeout int) {
	WaitOnChannel(value, timeout, dsl.status)
}

func (dsl *DefaultSessionListener) SessionAccepted(conn *net.TCPConn) bool {
	Log("DefaultSessionListener: SessionAccepted\n")
	connection := NewTcpConnection("", true, nil, conn)
	dsl.session.NewServer(connection)
	return true
}

func (dsl *DefaultSessionListener) SessionQuery(obj interface{}) interface{} {
	return dsl.session.SessionQuery(obj)
}

func (dsl *DefaultSessionListener) SessionControl(ctrl interface{}, value interface{}) interface{} {
	return dsl.session.SessionControl(ctrl, value)
}

func (dsl *DefaultSessionListener) SessionNotify(obj interface{}) {
	defer EtchRecoverAll()
	if obj.(int) == UP {
		dsl.status <- UP
	} else if obj.(int) == DOWN {
		dsl.status <- DOWN
	}
	dsl.session.SessionNotify(obj)
}

func (dsl *DefaultSessionListener) GetSession() Session {
	return dsl.session
}

func (dsl *DefaultSessionListener) SetSession(sess Session) {
	dsl.session = sess.(ServerFactory)
}

func (dsl *DefaultSessionListener) TransportQuery(obj interface{}) interface{} {
	return dsl.transport.TransportQuery(obj)
}

func (dsl *DefaultSessionListener) TransportControl(ctrl interface{}, value interface{}) {
	Log("DefaultSessionListener transportcontrol\n")
	if ctrl.(int) == START_AND_WAIT_UP {
		Log("DefaultSessionListener: transportControl: START_AND_WAIT_UP\n")
		dsl.transport.TransportControl(START, nil)
		dsl.waitUntil(UP, value.(int))
	} else if ctrl.(int) == STOP_AND_WAIT_DOWN {
		dsl.transport.TransportControl(STOP, nil)
		dsl.waitUntil(DOWN, value.(int))
	} else {
		dsl.transport.TransportControl(ctrl, value)
	}
}

func (dsl *DefaultSessionListener) TransportNotify(value interface{}) {
	dsl.transport.TransportNotify(value)
}
