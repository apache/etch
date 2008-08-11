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
using System.Reflection;

using Etch.Transport;
using Etch.Util;
using Etch.Support;
using Etch.HttpJsonTransport.TestUtilities;

using etch.examples.types.ComplexData;

namespace etch.examples
{
	///<summary>Main implementation for ComplexDataListener.</summary>
	public class MainComplexDataListener : ComplexDataHelper.ComplexDataServerFactory
	{
        private static string callingAssembly = new FileInfo( Assembly.GetCallingAssembly().Location ).DirectoryName;

        private static FileInfo[] requiredFiles;
        static MainComplexDataListener()
        {
            TransportFactory.Define("javascript", new BrowserTransportFactory());
            requiredFiles = new FileInfo[] { 
                new FileInfo(Path.Combine(callingAssembly, "etch.examples.ComplexData.js")),
                new FileInfo(Path.Combine(callingAssembly, "jquery.js")),
                new FileInfo(Path.Combine(callingAssembly, "json2.js")),
                new FileInfo(Path.Combine(callingAssembly, "transport.js")),
                new FileInfo(Path.Combine(callingAssembly, "complex_data.css")),
                new FileInfo(Path.Combine(callingAssembly, "complex_data.html"))
            };
        }

 		///<summary>Main for ComplexDataListener.</summary>
		///<param name="args">Command Line Arguments</param>
		public static void Main(String[] args)
		{
            // To make this manual test require minimal environmental configuration, 
            // we launch the TrivialHttpServer to serve out all web resources.

            HttpProxyServer proxyServer = new HttpProxyServer( requiredFiles );
            proxyServer.Start( "http://localhost:12221/", new Uri("http://localhost:12222/proxy/") );

            string uri = "javascript://localhost:12222";

            Console.WriteLine( "Open a browser to 'http://localhost:12221/complex_data.html'.\nOnce the page has loaded, hit the 'Connect' button first." );

            Transport<ServerFactory> listener = ComplexDataHelper.NewListener( uri, null, new MainComplexDataListener() );

            // Start the Listener
            listener.TransportControl( TransportConsts.START, null );

            Console.Read();

            listener.TransportControl( TransportConsts.STOP, null );
            proxyServer.Stop();
		}

		///<summary>Return a new instance of ComplexDataServer.</summary>
		///<param name="client">Reference to remote service</param>
		///<returns>Server Implementation</returns>
		public ComplexDataServer NewComplexDataServer( RemoteComplexDataClient client )
		{
			return new ImplComplexDataServer( client );
		}
	}
}