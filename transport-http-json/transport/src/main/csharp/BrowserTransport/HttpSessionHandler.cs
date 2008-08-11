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
using System.Collections.Generic;
using System.Text;

namespace BasicHttpListener
{
    public class HttpSessionHandler
    {
        private static object sessionLock = new object();

        private static Dictionary<string, HttpSession> activeSessions = new Dictionary<string, HttpSession>();

        public static HttpSession CreateSession(SessionIdFactory factory, string remoteIPAddress)
        {
            HttpSession session = new HttpSession( factory, remoteIPAddress );
            session.SessionExpired += new HttpSessionExpired(SessionExpired);
            activeSessions[session.Id] = session;
            return session;
        }

        public static HttpSession GetSession(string sessionId)
        {
            HttpSession session = null;

            // first find the session, and if found, refresh it
            lock(sessionLock)
            {
                if(activeSessions.ContainsKey(sessionId))
                {
                    session = activeSessions[sessionId];
                }
            }

            return session;
        }

        public static void SessionExpired(HttpSession expired)
        {
            lock(sessionLock)
            {
                Console.WriteLine("Expiring session: " + expired.Id);

                if(activeSessions.ContainsKey(expired.Id))
                {
                    activeSessions.Remove(expired.Id);
                }
            }
        }

    }
}
