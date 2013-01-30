// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.Net;
using System.Net.Sockets;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Implementation of runner which handles a network connection.
    /// </summary>
    abstract public class Connection<H> : Runner, 
        Transport<H>, RunnerHandler where H : Session
    {

        #region QUERIES

        /// <summary>
        /// Source query to get the local address
        /// </summary>
        public const String LOCAL_ADDRESS = "LOCAL_ADDRESS";

	    /// <summary>
	    /// Source query to get the remote address.
	    /// </summary>
	    public const String REMOTE_ADDRESS = "REMOTE_ADDRESS";

        #endregion  

        #region EVENTS
        
        
	    /// <summary>
	    /// Host name to specify to select listening on all interfaces.
        /// The value is "0.0.0.0".
	    /// </summary>
	    public const String ALL_INTFS = "0.0.0.0";

        /// <summary>
        /// Translates host name per well know names.
        /// </summary>
        /// <param name="s">input host name</param>
        /// <returns></returns>
        protected static String TranslateHost( String s )
        {
            if ( s != null && s.Equals( ALL_INTFS ) )
                return null;
            return s;
        }

        #endregion


        /// <summary>
        /// Constructs the Connection.
        /// </summary>
        public Connection()
        {
            SetHandler(this);
        }
      
        public void Started( )
        { 
            // nothing to do.
        }

        public void Stopped( )
        {
            // nothing to do.
        }

        public void Exception( String what, Exception e )
        {
            TodoManager.AddTodo(new TodoDelegateImpl(
                delegate(TodoManager mgr)
                {
                    session.SessionNotify(e);
                },
                delegate(TodoManager mgr, Exception e1)
                {
                    Console.WriteLine(e);
                    if (e1 != e)
                        Console.WriteLine(e1);
                }));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="first"></param>
        /// <returns></returns>
        /// Exception:
        ///     throws Exception
        protected override bool Run0( bool first )
        {
            bool ok = OpenSocket(!first);
            if ( !ok )
                return false;

            try
            {
                SetUpSocket();
            }
            catch ( Exception e )
            {
                FireException( "setup", e );
                Close( true );
                return true;
            }

            try
            {
                FireUp();
                ReadSocket();
                return true;
            }
            catch (SocketException e)
            {
                // TODO ignore "socket closed" condition
                FireException("run", e);
                Close(true);
                return true;
            }
            catch (Exception e)
            {
                FireException("run", e);
                Close(true);
                return true;
            }
            finally
            {
                FireDown();
                Close( false );
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="reconnect">reconnect true if we are trying to reconnect, false if this
        /// is the first time.</param>
        /// <returns>true if we should reconnect, false if we should stop.</returns>
        /// Exception:
        ///     throws Exception
        abstract protected bool OpenSocket( bool reconnect );
        
        /// <summary>
        /// Sets up a newly opened socket. This may involve setting socket
        /// options and opening input and output streams.
        /// </summary>
        /// Exception:
        ///     throws Exception
        abstract protected void SetUpSocket();

        /// <summary>
        /// Performs the usual and customary operations on a socket, such
        /// as read or accept.
        /// </summary>
        /// Exception:
        ///     throws Exception
        abstract protected void ReadSocket();

        /// <summary>
        /// Terminates operations on the socket.
        /// </summary>
        /// <param name="reset">true if the socket should be terminated immediately.</param>
        /// Exception:
        ///     throws Exception
        abstract public void Close( bool reset );

        public void Close()
        {
            Close( false );
        }

        public virtual Object TransportQuery( Object query )
        {
            if ( query.Equals( LOCAL_ADDRESS ) )
                return LocalAddress();

            if ( query.Equals( REMOTE_ADDRESS ) )
                return RemoteAddress();

            if ( query is TransportConsts.WaitUp )
            {
                WaitUp( ( ( TransportConsts.WaitUp ) query )._maxDelay );
                return null;
            }

            if ( query is TransportConsts.WaitDown )
            {
                WaitDown( ( ( TransportConsts.WaitDown ) query )._maxDelay );
                return null;
            }

            throw new NotSupportedException("unknown query: " + query);
        }

        abstract public EndPoint LocalAddress();

        abstract public EndPoint RemoteAddress();

        public void TransportControl( Object control, Object value )
        {
            if ( control.Equals( TransportConsts.START ) )
            {
                Start();
                return;
            }

            if ( control.Equals( TransportConsts.START_AND_WAIT_UP ) )
            {
                Start();
                WaitUp( ( int ) value );
                return;
            }

            if ( control.Equals( TransportConsts.STOP ) )
            {
                Stop();
                return;
            }

            if ( control.Equals( TransportConsts.STOP_AND_WAIT_DOWN ) )
            {
                Stop();
                WaitDown( ( int ) value );
                return;
            }

            if (control.Equals( TransportConsts.RESET ))
            {
                Close(true);
                return;
            }

            throw new NotSupportedException( "unknown control: " + control );
        }

        public void TransportNotify( Object eventObj )
        {
            //ignore
        }

        private void FireUp()
	    {
            status.Set(SessionConsts.UP);
            TodoManager.AddTodo(new TodoDelegateImpl(
                delegate(TodoManager mgr)
                {
                    session.SessionNotify(SessionConsts.UP);
                },
                delegate(TodoManager mgr, Exception e1)
                {
                    Console.WriteLine(e1);
                }));
          
	    }

        private void FireDown() 
	    {
            status.Set(SessionConsts.DOWN);
            TodoManager.AddTodo(new TodoDelegateImpl(
                delegate(TodoManager mgr)
                {
                    session.SessionNotify(SessionConsts.DOWN);
                },
               delegate(TodoManager mgr, Exception e1)
               {
                   Console.WriteLine(e1);
               }));

        }

        public H GetSession()
        {
            return this.session;
        }

        public void SetSession(H session)
        {
            this.session = session;
        }

        /// <summary>
        /// The session for the connection.
        /// </summary>
        protected H session;

        /// <summary>
        /// Waits until the connection is up.
        /// </summary>
        /// <param name="maxDelay">time in milliseconds to wait.</param>
        /// Exception:
        ///     throws ThreadInterruptedException
        public void WaitUp( int maxDelay ) 
	    {
		    status.WaitUntilEq( SessionConsts.UP, maxDelay );
	    }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="maxDelay">time in milliseconds to wait.</param>
        /// Exception:
        ///     throws InterruptedException
        public void WaitDown( int maxDelay ) 
	    {
            status.WaitUntilEq(SessionConsts.DOWN, maxDelay);
	    }

        private readonly Monitor<String> status = new Monitor<String>( "status", SessionConsts.DOWN );
    }
}
