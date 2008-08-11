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

using Etch.Transport;
using Etch.Util;
using Etch.Msg;
using Etch.Support;

namespace Etch.Transport
{
    public class BrowserTransporter : JsonHandler, TransportMessage, SessionMessage
    {
        /// <summary>
        ///     The verison of the Plain Ol' Javascript wire format
        /// </summary>
        public const int VERSION = 1;

        private readonly JsonInputParser jsonInputParser;
        private readonly JsonOutputParser jsonOutputParser;
        private BasicHttpListener.HttpSession httpClientSession;
        private SessionMessage session;

        /// <summary>
        /// Constructs the Messagizer with null handler and tagged data format
        /// constructed from the format factory.
        /// </summary>
        /// <param name="uri">the uri being used to configure the transport stack.</param>
        /// <param name="resources">the associated set of resources for this service.</param>      
        
        public BrowserTransporter(BasicHttpListener.HttpSession httpClientSession, string uri, Resources resources): this(new URL(uri), resources)
        {
            this.httpClientSession = httpClientSession;
            this.httpClientSession.SetHandler( this );
        }


        /// <summary>
        /// Constructs the Messagizer with null handler and tagged data format
        /// constructed from the format factory.
        /// </summary>
        /// <param name="uri">the uri being used to configure the transport stack.</param>
        /// <param name="resources">the associated set of resources for this service.</param>
        private BrowserTransporter(URL uri, Resources resources)
        {
            // Only POJSON is supported at the moment.

            // String format = uri.GetTerm( FORMAT );
          
            // ValueFactory may provide some sort of validating 
            DefaultValueFactory vf = (DefaultValueFactory) resources.Get( TransportConsts.VALUE_FACTORY );
            if ( vf == null )
            {
                throw new ArgumentException( String.Format( "value factory is not defined as '{0}' in resources", TransportConsts.VALUE_FACTORY ) );
            }

            jsonInputParser = new JsonInputParser( vf );
            jsonOutputParser = new JsonOutputParser( vf );
        }

        public override string ToString()
        {
            return String.Format("javascriptTransporter : {0}", httpClientSession);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="buf"></param>
        /// Exception:
        ///     throws Exception
        public void MessageFromClient( Who sender, int version, string methodName, string transactionId, Dictionary<string, object> arguments ) 
	    {   
            // Construct an Etch message from a JSON object representation
            
            Message message = jsonInputParser.ReadRequestMessage( version, methodName, transactionId, arguments );

            // Once the message is formed, send it to the mailbox manager.
            session.SessionMessage( sender, message );
	    }

        public void ResponseFromClient( Who sender, int version, string transactionId, string originalMethodName, object returnData )
        {
            // Construct an Etch message from a JSON object representation
            Message message = jsonInputParser.ReadResponseMessage( version, transactionId, originalMethodName, returnData );

            // Once the message is formed, send it to the mailbox manager.
            session.SessionMessage( sender, message );
        }

        ///////////////////////////
        // SourceHandler methods //
        ///////////////////////////

        public Object SessionQuery( Object query )
        {
            return session.SessionQuery( query );
        }

        public void SessionControl( Object control, Object value )
        {
            session.SessionControl( control, value );
        }

        public void SessionNotify( Object eventObj )
        {
            session.SessionNotify( eventObj );
        }

        ////////////////////
    	// Source methods //
	    ////////////////////


        public Object TransportQuery( Object query )
        {
            return httpClientSession.TransportQuery( query );
        }

        public void TransportControl( Object control, Object value )
        {
            httpClientSession.TransportControl( control, value );
        }

        public void TransportNotify( Object eventObj )
        {
            httpClientSession.TransportNotify( eventObj );
        }

        #region TransportMessage Members

        public void TransportMessage( Who recipient, Message msg )
        {
            Dictionary<string, object> data = null;
            if( msg.InReplyTo == null )
            {
                data = jsonOutputParser.WriteNewMessage( msg );
                httpClientSession.SendMessageToClient( recipient, data );
            }
            else
            {
                data = jsonOutputParser.WriteReplyMessage( msg );
                httpClientSession.SendResponseToClient( recipient, data );
            }
        }

        #endregion

        #region Transport<SessionMessage> Members

        public void SetSession( SessionMessage session )
        {
            this.session = session;
        }

        public SessionMessage GetSession()
        {
            return this.session;
        }
        #endregion

        #region SessionMessage Members

        public bool SessionMessage( Who sender, Message msg )
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion
    }
}
