using System;
using System.Collections.Generic;
using System.Text;


using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using System.Collections;
using System.Runtime.CompilerServices;

namespace etch.examples.chat
{
    public class ChatServerImpl : ChatServer, Session
    {

        private readonly ChatClient _client;
        private Dictionary<String, ChatServerImpl> _whoIsOnline;

        /// <summary>
        /// Constructs the ChatServerImpl.
        /// </summary>
        /// <param name="client">the client to use for callbacks.</param>
        /// <param name="whoIsOnline">the database of who is online.</param>
        public ChatServerImpl( ChatClient client, Dictionary<String, ChatServerImpl> whoIsOnline )
        {
            _client = client;
            _whoIsOnline = whoIsOnline;
        }

        #region ChatServer Members

        private string _user;

        [ MethodImpl ( MethodImplOptions.Synchronized ) ]
        public void login( string name, string pwd )
        {
            if ( (bool) isLoggedIn() )
                throw new etch.examples.chat.ConstsChat.Failure( "Already logged in" );

            if ( name == null )
                throw new etch.examples.chat.ConstsChat.Failure( "Name is null" );
            
            if ( name.StartsWith("bad"))
                throw new etch.examples.chat.ConstsChat.Failure( "Username is not valid" );

            if ( name.Length == 0 )
                throw new etch.examples.chat.ConstsChat.Failure( "Username is empty" );

            if ( pwd == null )
                throw new etch.examples.chat.ConstsChat.Failure( "Password is not valid" );

            // TODO check user authentication.

            // add this user to the who is online map.

            lock ( _whoIsOnline ) 
            {
                ChatServerImpl other ;

                // user is logged in already
                if ( _whoIsOnline.ContainsKey( name ) )
                {
                    other = _whoIsOnline[ name ];
                    try
                    {
                        other.Deliver( "SYSTEM", "You have been logged out because you logged in somewhere else." );
                    }
                    catch (Exception)
                    {
                        // ignore failure to send this message
                    }
                    other.logout();
                    other = null;
                }
                // key is not present, i.e., user isn't logged in already
                else
                    _whoIsOnline.Add( name, this );
            }

            // mark as logged in
            _user = name;

            // say we're online
            SayStatusChanged( _user, true );

            // tell the client who's online
            List<String> who;


            lock ( _whoIsOnline )
            {
                who = new List<string>( _whoIsOnline.Keys );
            }

            who.Remove( name );
            _client.whoIsOnline( who.ToArray() );

            Console.WriteLine( "login : " + name );
        }

        [MethodImpl( MethodImplOptions.Synchronized )]
        public void logout()
        {
            if ( _user != null )
            {
                bool tookUsOffline = false;

                lock ( _whoIsOnline )
                {
                    if ( _whoIsOnline.ContainsKey( _user ) )
                    {
                        if ( _whoIsOnline[ _user ] == this )
                        {
                            // remove us from the online map

                            _whoIsOnline.Remove( _user );
                            tookUsOffline = true;
                        }
                    }
                }

                if ( tookUsOffline )
                {
                    // say we're offline
                    SayStatusChanged( _user, false );
                    Console.WriteLine( "logout : " + _user );
                }
            }

            _user = null;
        }

        #endregion

        #region Chat Members

        public bool? isLoggedIn()
        {
            return ( _user != null );
        }

        private void SayStatusChanged( String u, bool isOnline )
        {
            Dictionary<String, ChatServerImpl>.ValueCollection who = _whoIsOnline.Values;
            ChatServerImpl[] cArray = new ChatServerImpl[ _whoIsOnline.Count ];
            who.CopyTo( cArray, 0 );

            foreach ( ChatServerImpl other in cArray )
            {
                if ( other != this )
                    other._client.statusChange( u, isOnline );
            }
        }

        // send
        public void send( string who, string msg )
        {
            String me = _user;
            if ( me == null )
                return;

            ChatServerImpl other;
            lock ( _whoIsOnline )
            {
                if ( _whoIsOnline.ContainsKey( who ) )    
                    other = _whoIsOnline[ who ];
                else
                    other = null;
            }

            if ( other == null )
                throw new etch.examples.chat.ConstsChat.Failure( "user is not online : " + who );

            other.Deliver( me, msg );
        }

        #endregion

        private void Deliver( String from, String msg )
        {
            try 
            {
                _client.send(from, msg );
            }
            catch( ConstsChat.Failure e ) 
            {
                Console.WriteLine( e );
                throw e;
            }
            catch ( Exception e ) 
            {
                Console.WriteLine( e );
                throw e;
            }
        }
    
        #region Session Members

        public void  SessionControl(object control, object value)
        {
 	        throw new Exception("The method or operation is not implemented.");
        }

        public void  SessionNotify(object eventObj)
        {
            if ( eventObj.Equals( SessionConsts.UP ) )
                logout();
        }

        public object  SessionQuery(object query)
        {
 	        throw new Exception("The method or operation is not implemented.");
        }

        #endregion
    }
}
