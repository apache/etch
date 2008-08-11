// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;
using System.IO;
using System.Net;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using Etch.Util;
using Etch.Transport;
using Etch.Support;

using Timer = System.Threading.Timer;
namespace BasicHttpListener
{
    public delegate void HttpSessionExpired(HttpSession expired);
    public delegate Dictionary<string, object> NewMessage(HttpSession origin, int version, string methodName, string transactionId, Dictionary<string, object> data);
    public delegate Dictionary<string, object> NewResponse(HttpSession origin, int version, string transactionId, string originalMethodName, object returnData);

    public class HttpSession : JsonSource, Who
    {
        /// <summary> Last known activity </summary>
        private DateTime lastActivity;
        private AutoResetEvent blocker;
        private List<ClientDirectedMessage> pendingMessages;
        private string id;
        private Timer activityTimer;
        private object timerLock;
        private static int connectionActivityTimeout = 15000; // TODO: pull from config/environment
        private TimeSpan timerSanityCheck = TimeSpan.FromMilliseconds(connectionActivityTimeout - 1000);
        private bool paused;
        public string Id { get { return id; } }
        public event HttpSessionExpired SessionExpired;
        public event NewMessage NewMessageReceived;
        public event NewResponse NewResponseReceived;
        public System.Threading.Timer timer;
        private BrowserTransporter transporter;
        private HttpListenerResponse response;
        private string remoteIPAddress;

        /// <summary> Governs whether the session currently has a block occurring</summary>
        private volatile bool blocking;

        private volatile bool sessionExpired;

        public HttpSession( SessionIdFactory factory, string remoteIPAddress )
        {
            this.remoteIPAddress = remoteIPAddress;
            this.blocking = false;
            this.sessionExpired = false;
            this.timer = new Timer(new TimerCallback(EventChannelUnqueue));
            this.paused = false;
            this.blocker = new AutoResetEvent(false);
            this.pendingMessages = new List<ClientDirectedMessage>();
            this.id = factory.GenerateSessionId();
            this.timerLock = new object();
            this.activityTimer = new System.Threading.Timer(new TimerCallback(SessionExpiredCheck));
            this.Refresh();
        }

        private void EventChannelUnqueue(object state)
        {
            lock(timerLock)
            {
                if(!sessionExpired && blocking)
                {
                    blocking = false;

                    timer.Change(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);

                    RespondOnEventChannel(this.response);

                    this.Unpause();
                }
            }
        }

        public void HandleOutgoingChannel(HttpListenerResponse response)
        {
            // Pause the session timer while a connection is held open from the client
            this.Pause();

            // see if there are any pending messages
            if(pendingMessages.Count == 0)
            {
                Console.WriteLine("Entering wait queue for new messages.");

                this.response = response;

                blocking = true;

                // if there are not, wait for a message to show up
                timer.Change(30000, System.Threading.Timeout.Infinite); /* 30000 should come from environmental config */
            }
            else
            {
                RespondOnEventChannel(response);
            }
        }

        private void RespondOnEventChannel(HttpListenerResponse response)
        {
            Console.WriteLine("Responding on event channel");

            ClientDirectedMessage[] messages = PullMessages();

            Dictionary<string, object> responseData = BuildResponse(messages);

            // send the response back
            StreamWriter streamWriter = null;
            try
            {
                streamWriter = new StreamWriter(response.OutputStream);
                Object writer = HttpServerImpl.JsonWriterConstructor.Invoke( new object[] { streamWriter } );
                HttpServerImpl.JsonWriterWrite.Invoke( writer, new object[] { responseData } );
                //JsonWriter writer = new JsonWriter(streamWriter);
                //writer.Write(responseData);
            } 
            catch(Exception e)
            {
                Console.WriteLine("Unable to respond to the session.\n{0}", e);
            }
            finally
            {
                if(streamWriter != null)
                {
                    streamWriter.Dispose();
                }
                response.Close();
            }

            this.Unpause();
        }

        public Dictionary<string, object> HandleIncomingChannel(int version, string methodName, string transactionId, Dictionary<string, object> arguments)
        {
            this.Refresh();

            Console.WriteLine( "Incoming request: {0}, {1}, {2}", version, methodName, transactionId );

            if(NewMessageReceived != null)
            {
                return NewMessageReceived(this, version, methodName, transactionId, arguments);
            }
            else
            {
                return null;
            }

        }

        public Dictionary<string, object> HandleResponseChannel(int version, string transactionId, string originalMethodName, object result)
        {
            this.Refresh();

            if(NewResponseReceived != null)
            {
                return NewResponseReceived(this, version, transactionId, originalMethodName, result);
            }
            else
            {
                return null;
            } 
        }

        public void SendMessageToClient(Who who, Dictionary<string, object> data)
        {
            PrebuiltClientDirectedMethod wrappedMessage = new PrebuiltNewMessage( this.Id, data );

            AddMessage( wrappedMessage );
        }

        public void SendResponseToClient( Who who, Dictionary<string, object> data )
        {
            PrebuiltClientDirectedMethod wrappedMessage = new PrebuiltResponse( this.Id, data );

            AddMessage( wrappedMessage );
        }

        public void AddMessage(ClientDirectedMessage message)
        {
            lock(pendingMessages)
            {
                this.pendingMessages.Add(message);
            }

            lock(timerLock)
            {
                if(blocking)
                {
                    EventChannelUnqueue(null);
                }
            }
        }

        private ClientDirectedMessage[] PullMessages()
        {
            lock(pendingMessages)
            {
                ClientDirectedMessage[] messages = this.pendingMessages.ToArray();
                this.pendingMessages.Clear();
                return messages;
            }  
        }

        private Dictionary<string, object> BuildResponse(ClientDirectedMessage[] messages)
        {
            List<Dictionary<string, object>> list = new List<Dictionary<string, object>>();
 
            foreach(ClientDirectedMessage message in messages)
            {
                list.Add(message.Serialize());
            }

            Dictionary<string, object> responseData = new Dictionary<string,object>();
            responseData[HttpServerImpl.EventMessages] = list;
            return responseData;
        }

        /// <summary>
        ///     Pauses the session timer
        /// </summary>
        private void Pause()
        {
            lock(timerLock)
            {
                this.paused = true;
                lastActivity = DateTime.Now;
                activityTimer.Change(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
            }
        }

        private void Unpause()
        {
            lock(timerLock)
            {
                this.paused = false;
                lastActivity = DateTime.Now;
                activityTimer.Change(connectionActivityTimeout, System.Threading.Timeout.Infinite);
            }
        }

        /// <summary>
        ///     Reset the activity timer and last known activity timestamp
        /// </summary>
        private void Refresh()
        {
            lock(timerLock)
            {
                lastActivity = DateTime.Now;
                activityTimer.Change(connectionActivityTimeout, System.Threading.Timeout.Infinite);
            }
        }

        private void SessionExpiredCheck(object state)
        {
            lock(timerLock)
            {
                // Do a sanity check that this timer didn't fire as a .Change() was being called
                if(!paused && DateTime.Now.Subtract(lastActivity) > timerSanityCheck)
                {
                    EndSession();
                }
            }
        }

        private void EndSession()
        {
            lock( timerLock )
            {
                // TODO: send session ended message to client?
                sessionExpired = true;

                if( SessionExpired != null )
                {
                    SessionExpired( this );
                }

                if( transporter != null )
                {
                    Console.WriteLine( "signalling down" );
                    TodoManager.AddTodo( new TodoDelegateImpl( delegate( TodoManager mgr ) { 
                        transporter.SessionNotify( SessionConsts.DOWN ); },
                                                       delegate( TodoManager mgr, Exception e1 ) { Console.WriteLine( e1.StackTrace ); } ) );
                }
                activityTimer.Dispose();
                timer.Dispose();

                this.NewMessageReceived = null;
                this.NewResponseReceived = null;
            }
        }


        #region Source Members

        public object GetHandler()
        {
            return this.transporter;
        }

        public void SetHandler( object handler )
        {
            this.transporter = (BrowserTransporter) handler;

            this.NewMessageReceived += new NewMessage( SendNewMessageToTransporter );
            this.NewResponseReceived += new NewResponse( SendNewResponseToTransporter );
        }

        private Dictionary<string, object> SendNewResponseToTransporter( HttpSession origin, int version, string transactionId, string originalMethodName, object returnData )
        {
            transporter.ResponseFromClient( this, version, transactionId, originalMethodName, returnData );

            return new Dictionary<string, object>();
        }

        private Dictionary<string, object> SendNewMessageToTransporter( HttpSession origin, int version, string methodName, string transactionId, Dictionary<string, object> data )
        {
            transporter.MessageFromClient( this, version, methodName, transactionId, data );

            return new Dictionary<string, object>();
        }

        #endregion

        #region Transport Members

        public void TransportControl( object control, object value )
        {
            if( control.Equals( TransportConsts.START ) )
            {
                // Not much to do?
                return;
            }

            if( control.Equals( TransportConsts.START_AND_WAIT_UP ) )
            {
                // Not much to do?
                return;
            }

            if( control.Equals( TransportConsts.STOP ) )
            {
                this.EndSession();
                return;
            }

            if( control is TransportConsts.WaitDown )
            {
                this.EndSession();
                return;
            }

            if( control.Equals( TransportConsts.STOP_AND_WAIT_DOWN ) )
            {
                this.EndSession();
                return;
            }

            throw new Exception( "unknown control " + control );
        }

        public void TransportNotify( object eventObj )
        {
            // What should be done here?
            throw new Exception( "The method or operation is not implemented." );
        }

        public object TransportQuery( object query )
        {
            // TODO
            //if( query.Equals( Etch.Transport.Connection<Session>.LOCAL_ADDRESS ) )
            //    return LocalAddress();

            if( query.Equals( Etch.Util.Connection<Session>.REMOTE_ADDRESS ) )
                return RemoteAddress();

            if( query is TransportConsts.WaitUp )
            {
               // WaitUp( ( (TransportConsts.WaitUp) query )._maxDelay );
                return null;
            }

            if( query is TransportConsts.WaitDown )
            {
              //  WaitDown( ( (TransportConsts.WaitDown) query )._maxDelay );
                return null;
            }

            throw new Exception( "unknown query " + query );
        }

        private string RemoteAddress()
        {
            return this.remoteIPAddress;
        }

        #endregion
    }
}
