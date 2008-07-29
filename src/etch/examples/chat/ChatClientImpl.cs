using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace etch.examples.chat
{
    public class ChatClientImpl : ChatClient, Session
    {
        private readonly String _whoami;
        private readonly bool _snub;
        private List<String> whoisOnline = new List<string>();
        private ChatServer server;


        /// <summary>
        /// Constructs the ChatClientImpl
        /// </summary>
        /// <param name="whoami"></param>
        /// <param name="snub"></param>
        public ChatClientImpl( String whoami, bool snub )
        {
            _whoami = whoami;
            _snub = snub;
        }

        #region ChatClient Members

        public void whoIsOnline( string[] names )
        {
            foreach ( String name in names )
            {
                Console.WriteLine( "{0}: {1} online (src = whoIsOnline)\n", _whoami, name );
                whoisOnline.Add( name );
            }
        }

        public void statusChange( string name, bool? online )
        {
            if ( (bool) online )
            {
                Console.WriteLine( "{0}: {1} online (src = statusChange)\n", _whoami, name );
                whoisOnline.Add( name );
            }
            else
            {
                Console.WriteLine( " {0}: {1} offline\n", _whoami, name );
                whoisOnline.Remove( name );
            }
        }

        #endregion

        #region Chat Members

        public bool? isLoggedIn()
        {
            // server is always logged in to the client
            return true;
        }

        public void send( string who, string msg )
        {
            Console.WriteLine( "{0}: msg from {1}: {2}\n", _whoami, who, msg );
            if ( _snub )
            {
                server.send( who, String.Format( "{0}, leave me alone!", who ) );
                Console.WriteLine( "snub done" );
            }
        }

        #endregion

        /// <summary>
        /// Sets the server so that we can send messages to it. 
        /// </summary>
        /// <param name="server"></param>
        public void setServer( ChatServer server )
        {
            this.server = server;
        }


        #region Session Members

        public void SessionControl( object control, object value )
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        public void SessionNotify( object eventObj )
        {
            if ( eventObj.Equals( SessionConsts.DOWN ) )
                Console.WriteLine( "(server disconnected)" );
        }

        public object SessionQuery( object query )
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion
    }
}
