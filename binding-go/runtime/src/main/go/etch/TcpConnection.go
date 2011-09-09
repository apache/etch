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

type Connection interface {
	openSocket() bool
	readSocket()
	closeSocket()
	TransportData
}

//we do not distinguish between TcpConnection and TcpTransport like Java does... 
type TcpConnection struct {
	address    string
	session    SessionData
	status     int
	isServer   bool
	connection *net.TCPConn
	buf        *FlexBuffer
}

func NewTcpConnection(address string, isServer bool, session SessionData, socket *net.TCPConn) *TcpConnection {
	return &TcpConnection{address, session, DOWN, isServer, socket, NewFlexBuffer()}
}


func (tcpc *TcpConnection) openSocket() bool {
	var er os.Error = nil
	if tcpc.isServer {
		if tcpc.connection != nil {
			Log("SERVER CONNECTION, GOING UP DIRECTLY\n")
			tcpc.status = UP
			return true
		}
	} else {
		addr, _ := net.ResolveTCPAddr("tcp", tcpc.address)

		tcpc.connection, er = net.DialTCP("tcp", nil, addr)
		tcpc.connection.SetLinger(0)
		if er != nil {
			Log("TcpConnection: error with client socket " + fmt.Sprint(er) + "\n")
			return false
		}
		Log("TcpConnection: open socket client success, before notify, connection is " + fmt.Sprint(tcpc.connection) + " \n")
		tcpc.status = UP
		tcpc.GetSession().SessionNotify(UP)
		Log("TcpConnection: open socket client success\n")
	}

	return true
}


func (tcpc *TcpConnection) readSocket() {
	buffer := make([]byte, 8192)
	for tcpc.status != DOWN {
		Log("TcpConnection in readSocket\n")
		n, er := tcpc.connection.Read(buffer)
		Log("TcpConnection in after readSocket: " + fmt.Sprint(n) + "\n")
		if er != nil {
			Log("TcpConnection readSocket error: " + fmt.Sprint(er) + "\n")
			tcpc.status = DOWN
		}

		tcpc.buf.PutBytes(buffer[0:n])
		tcpc.buf.Index = 0
		Log("TcpConnection: got " + fmt.Sprint(n) + " bytes data " + fmt.Sprint(tcpc.buf.avail()) + " " + fmt.Sprint(tcpc.buf) + "\n")
		tcpc.GetSession().(SessionData).SessionData(nil, tcpc.buf)
		tcpc.buf.Index = 0
		tcpc.buf.Length = 0
	}
}

func (tcpc *TcpConnection) closeSocket() {
	Log("TcpConnection CLOSING SOCKET\n")
	tcpc.connection.Close()	
	tcpc.GetSession().SessionNotify(DOWN)	
	Log("TcpConnection CLOSING SOCKET DOWN, session notify sent\n")
}


func (tcpc *TcpConnection) GetSession() Session {
	return tcpc.session
}

func (tcpc *TcpConnection) SetSession(sess Session) {
	tcpc.session = sess.(SessionData)
}

func (tcpc *TcpConnection) TransportData(recipient interface{}, buf *FlexBuffer) {
	Log("TcpConnection sending data\n")
	avail := buf.avail()
	bytes := make([]byte, buf.avail())
	buf.ReadBytes(&bytes)
	Log("TcpConnection: sending: #" + fmt.Sprint(len(bytes)) + " " + fmt.Sprint(bytes) + "\n")
	n, er := tcpc.connection.Write(bytes)
	if er != nil || n != int(avail) {
		Log("TcpConnection " + fmt.Sprint(tcpc.isServer) + " : error while sending with " + fmt.Sprint(tcpc.connection) +
			" Error was: " + fmt.Sprint(er) + ", sent " + fmt.Sprint(n) + "/" + fmt.Sprint(avail) + " bytes\n")
	}
}

func (tcpc *TcpConnection) TransportQuery(obj interface{}) interface{} {
	//nothing to do (java binding allow query of addresses, TODO)
	return nil
}

func (tcpc *TcpConnection) TransportControl(ctrl interface{}, value interface{}) {
	Log("TcpConnection: transportControl: " + fmt.Sprint(ctrl) + "\n")

	if ctrl == START {
		Log("TcpConnection: transportControl: " + fmt.Sprint(ctrl) + "\n")
		go tcpc.start()
	} else if ctrl == STOP {
		tcpc.stop()
	}
}

func (tcpc *TcpConnection) TransportNotify(value interface{}) {
	//nothing to do
}

func (tcpc *TcpConnection) start() {
	ok := tcpc.openSocket()
	if ok == false {
		return
	}
	tcpc.readSocket()
}

func (tcpc *TcpConnection) stop() {
	Log("TcpConnection: stopping\n")
	tcpc.closeSocket()
}
