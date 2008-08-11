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

using System.Net;
    
namespace Etch.HttpJsonTransport.TestUtilities
{
    /// <summary> A trivial, embeddable HTTP server that can serve up files and also proxy HTTP requests. 
    ///           Its purpose in life is to facilite running tests without having an involved test environment.
    /// 
    ///           The mode of operation is basically this:
    ///                 1) You give the Http server a set of files that it will attempt to load into memory, upon initialization.
    ///                 2) When an HTTP request comes in, it will look to see if the incoming request URI seems to match a 
    ///                     loaded file.  If it does, the file gets served out.  If their is no match for the request, then 
    ///                     the HTTP server proxies the HTTP request to pre-configured HTTP proxy URL. That's about it!</summary>
    public class HttpProxyServer
    {
        private HttpListener listener;
        private AsyncCallback callback;
        private long asyncTracker = 0;
        private WebFile[] knownFiles;
        private Uri destinationUri;

        public class WebFile
        {
            public FileInfo file;
            public string data;
        }

        public HttpProxyServer( FileInfo[] files )
        {
            this.knownFiles = CheckFiles( files );
            this.listener = new HttpListener();
            this.callback = new AsyncCallback( ReceivedRequest );
        }

        /// <summary> Invoke to open the Http Server Listener port 
        ///  Example: proxy.Start( "http://localhost:12221/", new Uri("http://localhost:12222/proxy/") );</summary>
        /// <param name="serverUrl">The URL that the server listens on.</param>
        /// <param name="destinationUri">The URL that the server should proxy requests on to.</param>
        public void Start( string serverUrl, Uri destinationUri )
        {
            this.destinationUri = destinationUri;
            this.listener.Prefixes.Add( serverUrl );
            this.listener.Start();
            this.listener.BeginGetContext( callback, asyncTracker++ );
        }

        /// <summary> Invoke to stop and cleanup the Http Server Listener </summary>
        public void Stop()
        {
            try
            {
                listener.Stop();
                listener.Close();
            }
            catch( Exception e )
            {
                Console.WriteLine( "Stop the HttpListener for the Http Proxy. " + e );
            }
        }

        /// <summary> Responsible for handing off the request to <code>HandleRequest</code> 
        ///           as well as informing the HTTP Listener class to begin </summary>
        public void ReceivedRequest( IAsyncResult asyncResult )
        {
            try
            {
                listener.BeginGetContext( callback, asyncTracker++ );

                HttpListenerContext context = listener.EndGetContext( asyncResult );

                HandleRequest( context.Request, context.Response );
            }
            catch( Exception e )
            {
                Console.WriteLine( "Unexpected error in request handler in HttpProxy server: " + e );
            }
        }

        /// <summary> Contains most all logic pertaining to the handling of a new HTTP request </summary>
        private void HandleRequest( HttpListenerRequest request, HttpListenerResponse response )
        {
            WebFile intendedFile;
            string proxiedUrl;
            Console.WriteLine( "Request: " + request.Url.AbsolutePath );
            if( IsFileRequest( request.Url.AbsolutePath, out intendedFile ) )
            {
                // serve out this file
                response.StatusCode = 200;
                StreamWriter writer = new StreamWriter( response.OutputStream );
                writer.Write( intendedFile.data );
                writer.Dispose();
                response.Close();
            }
            else if( IsProxyRequest( request.Url, out proxiedUrl ) )
            {
                HttpWebRequest newRequest = (HttpWebRequest) HttpWebRequest.Create( proxiedUrl );
                newRequest.Timeout = 35000;
                newRequest.ProtocolVersion = request.ProtocolVersion;
                newRequest.Method = request.HttpMethod;

                foreach( string header in request.Headers.AllKeys )
                {
                    string loweredHeader = header.ToLower();

                    string value = request.Headers[header];

                    switch( loweredHeader )
                    {
                        case "accept":
                            newRequest.Accept = value;
                            break;

                        case "content-type":
                            newRequest.ContentType = value;
                            break;

                        case "connection":
                            if( String.Compare( value, "Keep-Alive", true ) == 0 )
                            {
                                newRequest.KeepAlive = true;
                            }
                            else
                            {
                                newRequest.KeepAlive = false;
                            }
                            break;

                        case "content-length":
                            // skip
                            break;

                        case "expect":
                            newRequest.Expect = value;
                            break;

                        case "host":
                            // skip
                            break;

                        case "if-modified-since":
                            newRequest.IfModifiedSince = DateTime.Parse( value );
                            break;

                        case "referer":
                            newRequest.Referer = value;
                            break;

                        case "transfer-encoding":
                            newRequest.TransferEncoding = value;
                            break;

                        case "user-agent":
                            newRequest.UserAgent = value;
                            break;

                        default:
                            newRequest.Headers[header] = value;
                            break;

                    }
                }

                char[] buffer = new char[2000];
                int index = 0;

                if( newRequest.Method == "POST" )
                {
                    StreamReader originalReader = new StreamReader( request.InputStream );
                    StreamWriter newWriter = new StreamWriter( newRequest.GetRequestStream() );


                    while( originalReader.Peek() > -1 )
                    {
                        int read = 0;
                        read = originalReader.ReadBlock( buffer, index, buffer.Length );
                        index += read;
                        newWriter.Write( buffer, 0, read );
                    }

                    originalReader.Close();
                    originalReader.Dispose();
                    newWriter.Close();
                    newWriter.Dispose();
                }

                HttpWebResponse newResponse = (HttpWebResponse) newRequest.GetResponse();
                response.StatusCode = (int) newResponse.StatusCode;

                foreach( string header in newResponse.Headers.AllKeys )
                {
                    string loweredHeader = header.ToLower();

                    string value = newResponse.Headers[header];

                    switch( loweredHeader )
                    {
                        case "transfer-encoding":
                            // skip
                            break;

                        default:
                            response.Headers.Add( header, value );
                            break;
                    }
                }

                StreamReader newReader = new StreamReader( newResponse.GetResponseStream() );
                StreamWriter originalWriter = new StreamWriter( response.OutputStream );

                index = 0;
                while( newReader.Peek() > -1 )
                {
                    int read = 0;
                    read = newReader.ReadBlock( buffer, index, buffer.Length );
                    index += read;
                    originalWriter.Write( buffer, 0, read );
                }

                newReader.Close();
                newReader.Dispose();
                originalWriter.Close();
                originalWriter.Dispose();
            }
            else
            {
                response.StatusCode = (int) HttpStatusCode.NotFound;
                response.Close();
            }
        }

        /// <summary> Checks to see if the HTTP request is intended for proxying to a backend service </summary>
        private bool IsProxyRequest( Uri originalUri, out string proxiedUrl )
        {
            proxiedUrl = null;
            bool isProxyRequest = false;

            // example:
            // destinationUri: http://localhost:12222/proxy/

            // destinationPath example: /proxy/
            // intendPath example: /proxy/start

            string destinationPath = destinationUri.PathAndQuery;
            string intendedPath = originalUri.PathAndQuery;

            if( intendedPath.StartsWith( destinationPath ) )
            {
                // we have ourselves a proxy request!
                isProxyRequest = true;

                // rip off the proxy prefix
                string extraPath = intendedPath.Substring( destinationPath.Length );
                proxiedUrl = destinationUri.ToString() + extraPath;
            }

            return isProxyRequest;
        }

        /// <summary> Checks to see if HTTP request is for a known file. </summary>
        private bool IsFileRequest( string originalUrl, out WebFile intendedFile )
        {
            intendedFile = null;
            bool foundFile = false;

            foreach( WebFile file in knownFiles )
            {
                // a file request will end with the file name as the originalUri
                if( originalUrl.EndsWith( file.file.Name ) )
                {
                    intendedFile = file;
                    foundFile = true;
                    break;
                }
            }

            return foundFile;
        }

        /// <summary> Loads all files into memory.  Throws an exception if any file is missing. </summary>
        private WebFile[] CheckFiles( FileInfo[] files )
        {
            List<WebFile> fileData = new List<WebFile>();

            foreach( FileInfo file in files )
            {
                if( file.Exists )
                {
                    WebFile webFile = new WebFile();
                    webFile.file = file;
                    StreamReader reader = new StreamReader( file.Open( FileMode.Open ) );
                    string fullText = reader.ReadToEnd();
                    webFile.data = fullText;
                    reader.Close();
                    fileData.Add( webFile );
                }
                else
                {
                    throw new Exception( String.Format( "Unable to open file {0}", file.FullName ) );
                }
            }

            return fileData.ToArray();
        }
    }
}
