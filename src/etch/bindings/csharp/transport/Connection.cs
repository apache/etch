using System;
using System.Text;
using System.Net;
using System.Collections.Generic;

namespace Etch.Transport
{
    /// <summary>
    /// Implementation of runner which handles a network connection.
    /// </summary>
    abstract public class Connection<H> : Runner, 
        Source, RunnerHandler where H : SourceHandler<Source>
    {

        #region QUERIES

        /// <summary>
        /// Source query to get the local address
        /// </summary>
        public static readonly String LOCAL_ADDRESS = "LOCAL_ADDRESS";

	    /// <summary>
	    /// Source query to get the remote address.
	    /// </summary>
	    public static readonly String REMOTE_ADDRESS = "REMOTE_ADDRESS";

        #endregion  

        #region EVENTS
        
        /// <summary>
        /// Handler event that the connection is up. 
        /// </summary>
	    private static readonly String UP = "up";
    	
	    /// <summary>
	    /// Handler event that the connection is down
	    /// </summary>
	    private readonly static String DOWN = "down";

	    /// <summary>
	    /// Host name to specify to select listening on all interfaces.
        /// The value is "0.0.0.0".
	    /// </summary>
	    public static readonly String ALL_INTFS = "0.0.0.0";

        protected static String TranslateHost( String s )
        {
            if ( s.Equals( ALL_INTFS ) )
                return null;
            return s;
        }

        #endregion


        /// <summary>
        /// Constructs the Connection.
        /// </summary>
        /// <param name="handler">the handler for connection events.</param>
        public Connection( H handler )
        {
            base.SetHandler( (RunnerHandler) this);
            this.handler = handler;
        }

        protected H handler;

        public Object GetHandler()
        {
            return handler;
        }

        public void SetHandler( Object handler )
        {
            this.handler = ( H ) handler;
            ( ( H ) handler ).SetSource( this );
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
            try
            {
                handler.SessionNotify( e );
            }
            catch ( Exception e1 )
            {
                Console.WriteLine( e1.StackTrace );
            }
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
            if ( !OpenSocket( !first ) )
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
            catch ( Exception e )
            {

                if ( e is System.Net.Sockets.SocketException  && 
                    e.Message.Contains( " did not properly respond ") )
                    
                    return true;
                
                FireException( "run", e );
                Close( true );
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

        public Object TransportQuery( Object query )
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

            throw new Exception( "unknown query " + query );
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

            if ( control is TransportConsts.WaitDown )
            {
                WaitDown( ( ( TransportConsts.WaitDown ) control )._maxDelay );
                return;
            }

            if ( control.Equals( TransportConsts.STOP_AND_WAIT_DOWN ) )
            {
                Stop();
                WaitDown( ( int ) value );
                return;
            }

            throw new Exception( "unknown control " + control );
        }

        public void TransportNotify( Object eventObj )
        {
            //ignore
        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        private void FireUp()
	    {
            status.Set( UP );
            handler.SessionNotify( SessionConsts.UP );
	    }

        private void FireDown() 
	    {
            status.Set( DOWN );
            handler.SessionNotify( SessionConsts.DOWN );
	    }

        /// <summary>
        /// Waits until the connection is up.
        /// </summary>
        /// <param name="maxDelay">time in milliseconds to wait.</param>
        /// Exception:
        ///     throws ThreadInterruptedException
        public void WaitUp( int maxDelay ) 
	    {
		    status.WaitUntilEq( UP, maxDelay );
	    }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="maxDelay">time in milliseconds to wait.</param>
        /// Exception:
        ///     throws InterruptedException
        public void WaitDown( int maxDelay ) 
	    {
		    status.WaitUntilEq( DOWN, maxDelay );
	    }

        private Monitor<String> status = new Monitor<String>( "status", DOWN );
    }
}
