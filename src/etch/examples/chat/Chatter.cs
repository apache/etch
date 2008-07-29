using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using System.Globalization;
using Etch.Util;

namespace etch.examples.chat
{
    public class Chatter : ChatClient, Close
    {
        private ChatServer _server;
        private List<String> _whoIsOnline = new List<string>();
        private DateTimeFormatInfo _df = new DateTimeFormatInfo();

        public Chatter()
        {
            _df.MonthDayPattern = "dd/MM/yyyy";
        }

        public static void Main( String[] args )
        {
            String host;
            int port;

            if ( args.Length == 2 )
            {
                host = "127.0.0.1";
                port = int.Parse( args[ 1 ] );
            }
            else if ( args.Length == 0 )
            {
                host = "127.0.0.1";
                port = 4001;
            }
            else
            {
                Console.WriteLine( "usage: chatter host port" );
                Environment.Exit( 0 );
                throw new Exception();
            }

            //TcpConnection conn = new TcpConnection( null, host, port, 0 );
            //Chatter client = new Chatter();
            //Pool queuedPool = new QueuedPool();
            //Pool freePool = new FreePool();
            //ChatServer server = ChatHelper.NewChatServer( conn, client, queuedPool, freePool );
            //client.SetServer( server );

            //conn.Start();
            //conn.WaitUp( 4000 );

            // transport refactor
            URL uri = new URL( "tcp://127.0.0.1:4001" );
            Resources resources = ChatHelper.InitResources( null );

            Chatter client = new Chatter();
            RemoteChatServer server = ChatHelper.NewChatServer( uri, resources, client );
            client.SetServer( server );
            server._StartAndWaitUp( 4000 );

            Console.WriteLine( "( connected, enter commands or help)" );

            while ( true )
            {
                Console.Write( "cmd : " );
                Console.Out.Flush();

                String s = Console.ReadLine();
                if ( s.Length == 0 )
                    continue;

                char[] seps = { ' ', '\t', '\r', '\n' };
                String[] cmds = s.Split( seps );
                
                if ( cmds.Length == 0 )
                    continue;

                if ( cmds[ 0 ].Equals( "help" ) )
                {
                    Do_help();
                    continue;
                }

                if ( cmds[0].Equals( "login") )
                {
                    Do_login( server, cmds );
                    continue;
                }

                if ( cmds[0].Equals( "logout") )
                {
                    server.logout();
                    continue;
                }

                if ( cmds[0].Equals( "quit") )
                {
                    break;
                }

                if ( cmds[ 0 ].Equals( "send" ) )
                {
                    // extract message 
                    // note: cmds[2] = message to be extracted

                    if ( cmds.Length < 3 )
                    {
                        Console.WriteLine( "usage : login <username> <pwd>" );
                        continue;
                    }

                    char[] seps2 = {'\n', '\r'};
                    String msg = "";

                    for ( int i = 2; i < cmds.Length; i++ )
                        msg = msg + " " + cmds[ i ];

                    String[] cmds2 = msg.Split( seps2 );
                    Do_send( server, cmds[ 1 ], cmds2 );
                    continue;
                }

                if ( cmds[ 0 ].Equals( "who" ) )
                {
                    Do_who( client );
                    continue;
                }

                Console.WriteLine( "unknown command" );
                Do_help();
            }

            server.logout();
            ( ( RemoteChatServer ) server )._StopAndWaitDown( 4000 );
        }

        private static void Do_help()
        {
            Console.WriteLine( "commands: " );
	        Console.WriteLine( "  help" );
	        Console.WriteLine( "  login <user> <pwd>" );
	        Console.WriteLine( "  logout" );
	        Console.WriteLine( "  quit" );
 	        Console.WriteLine( "  send <user> <msg>" );
 	        Console.WriteLine( "  who" );
        }

        private static void Do_login( ChatServer server, String[] st )
        {
            try
            {
                String name = st[ 1 ];
                String pwd = st[ 2 ];
                server.login( name, pwd );
                Console.WriteLine( "login successful" );
            }
            catch ( etch.examples.chat.ConstsChat.Failure e )
            {
                Console.WriteLine( "failed : " + e.GetMessage() );
            }
            catch ( IndexOutOfRangeException )
            {
                Console.WriteLine( "usage : login <username> <pwd>" );
            }
            catch ( Exception e )
            {
                Console.WriteLine( e.Message );
            }
        }

        /// <summary>
        /// Send message 
        /// </summary>
        /// <param name="server"></param>
        /// <param name="user">username</param>
        /// <param name="st">message to be sent</param>
        /// 
        private static void Do_send( ChatServer server, String user, String[] st )
        {
            try
            {
                String who = user;
                String msg = st[ 0 ];
                server.send( who, msg );
            }
            catch ( etch.examples.chat.ConstsChat.Failure e )
            {
                Console.WriteLine( "failed : " + e.GetMessage() );
            }
            catch ( _Etch_AuthException )
            {
                Console.WriteLine( "you must login first" );
            }
            catch ( IndexOutOfRangeException )
            {
                Console.WriteLine( "usage: send <user> <msg> " );
            }
            catch ( Exception e )
            {
                
                Console.WriteLine( e.Message );
            }
        }

        private static void Do_who(Chatter client)
        {
            String[] names = client.GetWhoIsOnline();
            foreach ( String name in names )
            {
                Console.WriteLine( " " + name );
            }
        }

        private void SetServer( ChatServer server )
        {
            _server = server;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>a list of who is currently online</returns>
        public String[] GetWhoIsOnline()
        {
            return _whoIsOnline.ToArray();
        }

        #region Close Members

        public void _Close( MessageSource src )
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion

        #region ChatClient Members

        public void whoIsOnline( string[] names )
        {
            foreach ( String name in names )
            {
                Console.WriteLine( name + " is online" );
                _whoIsOnline.Add( name );
            }
        }

        public void statusChange( string name, bool? online )
        {
            if ( ( bool ) online )
            {
                Console.WriteLine( name + " is online" );
                _whoIsOnline.Add( name );
            }
            else
            {
                Console.WriteLine( name + " is offline" );
                _whoIsOnline.Remove( name );
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
            lock ( _df )
            {
                Console.WriteLine( "[ {0}@{1} ] {2}\n", who, DateTime.Today.ToString( _df ), msg );
            }
        }


        #endregion
    }
}
