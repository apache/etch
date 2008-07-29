using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;
using Etch.Msg;

namespace Etch.Transport
{
    public abstract class AbstractDeliveryService : DeliveryService, MessageHandler<MailboxMessageSource>
    {

        private MailboxMessageSource _src;

        public MailboxMessageSource GetSource()
        {
            return _src;
        }

        public void SetSource( MailboxMessageSource src )
        {
            _src = src; 
        }

        private StubBase _stub;

        /// <summary>
        /// Adds a stub to a list of stub
        /// </summary>
        /// <param name="stb"></param>
        public void AddStub( StubBase stb )
        {
            if ( _stub != null )
                throw new Exception( "Unsupported -- only one stub for now" );
            _stub = stb;
        }

        /// <summary>
        /// Removes all the stubs in the set of stubs
        /// </summary>
        public void RemoveStub()
        {
            _stub = null;
        }

        public Object SessionQuery( Object query )
        {
            if ( _stub != null )
                return _stub.SessionQuery( query );

            throw new Exception( "Unknown query " + query );
        }

        public void SessionControl( Object control, Object value )
        {
            if ( _stub != null )
                _stub.SessionControl( control, value );

            throw new Exception( "Unknown control " + control );
        }

        public void SessionNotify( Object eventObj )
        {
            if ( _stub != null )
                _stub.SessionNotify( eventObj );
            else if ( eventObj is Exception )
                Console.WriteLine( ( ( Exception ) eventObj ).StackTrace );
        }

        public Boolean Message( Who sender, Message msg )
        {
            return _stub.Message( sender, msg );
        }

        public Object TransportQuery( Object query )
        {
            return _src.TransportQuery( query );
        }

        public void TransportControl( Object control, Object value )
        {
            _src.TransportControl( control, value );
        }

        public void TransportNotify( Object eventObj )
        {
            _src.TransportNotify( eventObj );
        }

        public void Send( Message msg )
        {
            _src.Messagex( null, msg );
        }

        public Mailbox BeginCall( Message msg )
        {
            try
            {
                return _src.Message( null, msg, 1, 0 );
            }
            catch ( Exception e )
            {
                throw new Exception( "unexpected exception sending message", e );
            }
        }

        public Object EndCall( Mailbox mb, XType responseType, Field responseField,
            int timeout )
        {
            try
            {
                Element mbe = mb.Read( timeout );
                if ( mbe == null )
                    throw new TimeoutException( "timeout waiting for " + responseType );
                Message rmsg = mbe.msg;
                rmsg.CheckType( responseType );
                Object r = rmsg.Get( responseField );
                if ( r is Exception )
                {
                    Exception e = ( Exception ) r;
                    throw e;
                }
                return r;
            }
            finally
            {
                mb.CloseDelivery();
            }
        }

        public abstract override string ToString();
    }
}
