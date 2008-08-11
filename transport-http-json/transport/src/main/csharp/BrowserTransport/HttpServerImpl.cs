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
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.Net;

using Etch.Transport;
using Etch.Util;

using System.Reflection;

namespace BasicHttpListener
{
    public delegate void NewClient(HttpSession session);

    public class HttpServerImpl : Transport<HttpServerHandler>
    {
        private List<HttpListenerResponse> list;

        // used by reference implementation, not etch tool chain
        public event NewClient NewClientConnected;
        public object methodLock = new object();

        public static ConstructorInfo JsonReaderConstructor { get { return jsonReaderConstructor; } }
        public static ConstructorInfo JsonWriterConstructor { get { return jsonWriterConstructor; } }
        public static MethodInfo JsonReaderDeserialize { get { return jsonReaderDeserialize; } }
        public static MethodInfo JsonWriterWrite { get { return jsonWriterWrite; } }

        private static Assembly jsonFxAssembly;
        private static ConstructorInfo jsonReaderConstructor;
        private static ConstructorInfo jsonWriterConstructor;
        private static MethodInfo jsonReaderDeserialize;
        private static MethodInfo jsonWriterWrite;


        #region Field Constants
        public const string VersionField = "version";
        public const string SessionId = "sessionId";
        public const string MethodName = "method";
        public const string Arguments = "args";
        public const string EventMessages = "eventMessages";
        public const string TransactionId = "transactionId";
        public const string Data = "data";

        #endregion 
        
        public enum UriType
        {
            start,
            request,
            @event,
            response,
            invalid,
            comettest
        }


        #region Error Message Constants

        public const string RequestBodyParseFailure = "Unable to parse the JSON request body.";
        public const string NoSessionId = "Unable to find a SessionId in the JSON request body.";
        #endregion

        private static Regex uriChecker = new Regex(@"^http.*/(?<type>(start|request|event|response|comettest))/?.*$", RegexOptions.Compiled | RegexOptions.Singleline);
        private HttpListener listener = null;
        private SessionIdFactory factory = null;
        private AsyncCallback callback;
        private static long asyncTracker = 0;
        private object comettest;
        private HttpServerHandler session;

        public HttpServerImpl( bool secure, short port ) : this( secure, port, new SimpleSessionIdFactory() )
        {
           
        }

        public HttpServerImpl(bool secure, short port, SessionIdFactory factory) : this()
        {
            this.list = new List<HttpListenerResponse>();

            this.factory = factory;

            this.callback = new AsyncCallback(ReceivedRequest);
            
            this.listener = new HttpListener();
            this.listener.Prefixes.Add("http" + (secure ? "s" : "") + "://+:" + port + "/");

            this.comettest = new object();
        }

        protected HttpServerImpl()
        {
            string jsonFxPath = null;
            try
            {
                // Determine path of this dll.  It is expected that JsonFx.Json.dll live in the same directory.
                FileInfo callingAssembly = new FileInfo( Assembly.GetCallingAssembly().Location );
                jsonFxPath = Path.Combine( callingAssembly.DirectoryName, "JsonFx.Json.dll" );

                jsonFxAssembly = Assembly.LoadFile( jsonFxPath );
            }
            catch(FileNotFoundException fnfe)
            {
                string jsonFxDllNotFound = String.Format("Unable to find JsonFx.Json.dll at path {0}! Exception:\n{1}", jsonFxAssembly, fnfe);
                Console.WriteLine( jsonFxDllNotFound );
                throw new Exception( jsonFxDllNotFound );
            }
            catch( Exception e )
            {
                string jsonFxDllNotLoaded = "Unable to load JsonFx.Json.dll! Exception:\n" + e;
                Console.WriteLine( jsonFxDllNotLoaded );
                throw new Exception( jsonFxDllNotLoaded );
            }

            try
            {
                Type readerType = jsonFxAssembly.GetType( "JsonFx.Json.JsonReader", true );
                jsonReaderConstructor = readerType.GetConstructor( BindingFlags.ExactBinding | BindingFlags.Instance | BindingFlags.Public | BindingFlags.CreateInstance, null, CallingConventions.Any, new Type[] { typeof( Stream ) }, null );
                jsonReaderDeserialize = readerType.GetMethod( "Deserialize", new Type[0] );
            }
            catch( Exception e )
            {
                string jsonReaderUnavailable = "JsonFx.Json.JsonReader not found or not of the expected interface in JsonFx.Json.dll. " + e;
                Console.WriteLine( jsonReaderUnavailable );
                throw new Exception( jsonReaderUnavailable );
            }

            try
            {
                Type writerType = jsonFxAssembly.GetType( "JsonFx.Json.JsonWriter", true );
                jsonWriterConstructor = writerType.GetConstructor( BindingFlags.ExactBinding | BindingFlags.Instance | BindingFlags.Public | BindingFlags.CreateInstance, null, CallingConventions.Any, new Type[] { typeof( TextWriter ) }, null );
                jsonWriterWrite = writerType.GetMethod( "Write", new Type[] { typeof(Dictionary<string, object>) } );
            }
            catch( Exception e )
            {
                string jsonWriterUnavailable = "JsonFx.Json.JsonWriter not found or not of the expected interface in JsonFx.Json.dll. " + e;
                Console.WriteLine( jsonWriterUnavailable );
                throw new Exception( jsonWriterUnavailable );
            }
        }


        public void Start()
        {
            listener.Start();

            listener.BeginGetContext(callback, asyncTracker++);
        }

        public void ReceivedRequest(IAsyncResult asyncResult)
        {
            // TODO: Remove LOCK
            //lock(methodLock) // There is no reason for this to be single threaded like this--this is only temporary to prove certain stress tests
            //{
                try
                {
                    listener.BeginGetContext(callback, asyncTracker++);

                    HttpListenerContext context = listener.EndGetContext(asyncResult);       

                    // Serve up 
                    HandleRequest(context.Request, context.Response);
                }
                catch(Exception e)
                {
                    Console.WriteLine("Unexpected error: " + e);
                }
            //}
        }

        public void Stop()
        {
            // notify all open clients of server shutdown

            NotifyShutdown();

            listener.Close();
        }

        private void NotifyShutdown()
        {
            // ...TODO
        }


        /// <summary> Entry point for handling requests and serving responses</summary>
        private void HandleRequest(HttpListenerRequest request, HttpListenerResponse response)
        {
            Console.WriteLine( "request: {0}", request.Url );
            #region Sanity Checks

            // check content-type
            //if(!IsCorrectContentType(request.ContentType))
            //{
            //    SendBadContentTypeResponse(response);
            //    return;
            //}

            // check if is valid URI
            UriType type;
            if(!IsValidURI(request.Url.ToString(), out type))
            {
                SendUnknownUri(request.Url.ToString(), response);
                return;
            }


            #region Hacky Test Code
            if(type == UriType.comettest)
            {
                System.Threading.Thread.Sleep(5000);
                list.Add(response);
                ThreadStorage storage = new ThreadStorage();             
                storage.timer = new System.Threading.Timer(new System.Threading.TimerCallback(timer_Elapsed), storage, 30000, System.Threading.Timeout.Infinite);
                return;
            }  
            #endregion


            response.KeepAlive = true;

            Dictionary<string, object> data = null;
            try
            {
                Object reader = jsonReaderConstructor.Invoke( new object[] { request.InputStream } );
                data = (Dictionary<string, object>) jsonReaderDeserialize.Invoke( reader, null );
                //JsonReader reader = new JsonReader(request.InputStream);
                //data = reader.Deserialize() as Dictionary<string, object>;
            }
            catch(Exception e)
            {
                Console.WriteLine("Exception in parsing request message.\n{0}", e);
                SendMalformedRequest(RequestBodyParseFailure, response);
                return;
            }

            #endregion

            response.ContentType = "application/json";

            // determine if this is a new session, extracting sessionId if found
            if(IsNewSession(type))
            {
                HttpSession session = HttpSessionHandler.CreateSession(this.factory, request.RemoteEndPoint.Address.ToString());
                
                // Should be sending this near bottom, when we know whether or not we have successfully created the Etch session
                SendNewSession( session, response );

                System.Diagnostics.Debug.Assert( this.session != null, "Unable to indicate to HttpServerHandler that a new client has connected." );

                try
                {
                    // TODO: refactor out this check once reference implementation stabilizes--
                    // this event is only consumed by the reference implementation
                    if( NewClientConnected != null )
                    {
                        NewClientConnected( session );
                    }

                    this.session.NewClient( session );
                }
                catch( Exception e )
                {
                    Console.WriteLine( "Unable to notify server of new client.\n" + e );
                }
            }
            else
            {
                String sessionId = ExtractSessionId(data);
                if(sessionId != null)
                {
                    // The version of the message
                    int version = ExtractVersion( data );

                    // if this is any request 
                    HttpSession session = HttpSessionHandler.GetSession(sessionId);

                    // This must be session bound.  Otherwise we have an issue!
                    if(session != null)
                    {
                        Dictionary<string, object> responseData = null;

                        // determine the type of message
                        switch(type)
                        {
                            case UriType.request:

                                String methodName = ExtractMethodName(data);
                                if(methodName != null)
                                {
                                    responseData = session.HandleIncomingChannel(version, methodName, ExtractTransactionId(data), ExtractArguments(data));
                                }
                                else
                                {
                                    SendMalformedRequest("No method defined.", response);
                                }
                                break;

                            case UriType.@event:

                                session.HandleOutgoingChannel(response);
                                break;

                            case UriType.response:

                                String originalMethodName = ExtractMethodName( data );
                                string transactionId = ExtractTransactionId(data);
                                responseData = session.HandleResponseChannel( version, transactionId, originalMethodName, ExtractReturnData(data) );
                                break;
                        }

                        if(type != UriType.@event)
                        {
                            // send the response back
                            StreamWriter streamWriter = null;
                            try
                            {
                                streamWriter = new StreamWriter(response.OutputStream);
                                Object writer = jsonWriterConstructor.Invoke( new object[] { streamWriter } );
                                jsonWriterWrite.Invoke( writer, new object[] { responseData } );
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
                        }
                    }
                    else
                    {
                        SendNoSession(response);
                    }      
                }
            }
        }

        #region Hacky Test Code

        public class ThreadStorage
        {
            public ThreadStorage() { }

            public System.Threading.Timer timer;
        }

        //        void timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        void timer_Elapsed(object sender)
        {
            lock(comettest)
            {
                try
                {
                    if(list.Count > 0)
                    {
                        HttpListenerResponse response = list[0];
                        list.RemoveAt(0);
                        response.ContentType = "text/plain";
                        StreamWriter writer = new StreamWriter(response.OutputStream);
                        writer.WriteLine("oeuoeuoeu");
                        writer.Close();
                        writer.Dispose();
                    }

                    ThreadStorage storage = sender as ThreadStorage;
                    storage.timer.Dispose();

                }
                catch(Exception exp)
                {
                    Console.WriteLine(exp);
                }
            }
        }
        #endregion

        /// <summary> Confirms that the content-type is 'application/json' </summary>
        /// <param name="contentType">The content-type of the request</param>
        /// <returns><code>true</code> if the content-type is 'application/json' </returns>
        private bool IsCorrectContentType(string contentType)
        {
            return String.Compare(contentType, "application/json", true) == 0;
        }

        private UriType DetermineUriType(string uri)
        {
            UriType type = UriType.invalid;
            Match match = uriChecker.Match(uri);
            if(match != null)
            {
                string typeValue = match.Groups["type"].Value;

                if(Enum.IsDefined(typeof(UriType), typeValue))
                {
                    type = (UriType) Enum.Parse(typeof(UriType), typeValue);
                }
            }
            return type;
        }

        /// <summary> Determines if this is a request initiating a new session. </summary>
        /// <param name="url">The type of the request</param>
        private bool IsNewSession(UriType type)
        {
            return type == UriType.start;
        }

        private string ExtractSessionId(Dictionary<string, object> data)
        {
            String sessionId = data[SessionId] as string;

            return sessionId;
        }

        private int ExtractVersion( Dictionary<string, object> data )
        {
            int version = (int) data[VersionField];

            return version;
        }

        private string ExtractMethodName(Dictionary<string, object> data)
        {
            String methodName = data[MethodName] as string;

            return methodName;
        }

        private string ExtractTransactionId(Dictionary<string, object> data)
        {
            string transactionId =  data[TransactionId] as string;

            return transactionId;
        }

        private Dictionary<string, Object> ExtractArguments(Dictionary<string, object> data)
        {
            Dictionary<string, object> arguments = null;
            
            if(data.ContainsKey(Arguments)) 
            {
                arguments = data[Arguments] as Dictionary<string, object>;
            }
         
            return arguments;
        }

        private object ExtractReturnData( Dictionary<string, object> data )
        {
            object value = null;
            if(data.ContainsKey(Data))
            {
                value = data[Data];
            }
            return value;
        }

        private bool IsValidURI(string uri, out UriType type)
        {
            type = DetermineUriType(uri);

            return type != UriType.invalid;
        }

        private void SendBadContentTypeResponse(HttpListenerResponse response)
        {
            response.StatusCode = 500;
            
            // send json response body with error message
            System.Diagnostics.Debug.Assert(false, "Not implemented");
        }

        private void SendNoSession(HttpListenerResponse response)
        {
            Console.WriteLine( "No session exists!" );
            //System.Diagnostics.Debug.Assert(false, "Not implemented");
        }

        private void SendUnknownUri(string uri, HttpListenerResponse response)
        {
             System.Diagnostics.Debug.Assert(false, "Not implemented");
        }

        private void SendNewSession(HttpSession session, HttpListenerResponse response)
        {
            StreamWriter streamie = new StreamWriter(response.OutputStream);
            Dictionary<string, object> data = new Dictionary<string, object>();
            data[SessionId] = session.Id;

            Object writer = jsonWriterConstructor.Invoke( new object[] { streamie } );
            jsonWriterWrite.Invoke( writer, new object[] { data } );
            //JsonWriter writer = new JsonWriter( streamie );
            //writer.Write( data );

            streamie.Dispose();
            response.Close();
        }

        private void SendMalformedRequest(string message, HttpListenerResponse response)
        {
            System.Diagnostics.Debug.Assert(false, "Not implemented");
        }

        #region Transport Members

        public void TransportControl( object control, object value )
        {
            if( control.Equals( TransportConsts.START ) )
            {
                Start();
                return;
            }

            if( control.Equals( TransportConsts.START_AND_WAIT_UP ) )
            {
                Start();
                return;
            }

            if( control.Equals( TransportConsts.STOP ) )
            {
                Stop();
                return;
            }

            if( control is TransportConsts.WaitDown )
            {
                Stop();
                return;
            }

            if( control.Equals( TransportConsts.STOP_AND_WAIT_DOWN ) )
            {
                Stop();
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
            // What should be done here?
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion

        #region Transport<HttpServerHandler> Members

        public HttpServerHandler GetSession()
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        public void SetSession( HttpServerHandler session )
        {
            this.session = session;
        }

        #endregion
    }
}
