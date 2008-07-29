using System;
using System.Text;
using System.Collections.Generic;
using Etch.Msg;
using Etch.Support;

namespace Etch.Transport
{
    abstract public class DefaultMessageHandler<T> : MessageHandler<MailboxMessageSource> 
        where T : StubBase
    {

        /// <summary>
        /// Constructs the default message handler
        /// </summary>
        public DefaultMessageHandler()
        {
            // nothing to do.
        }

        abstract protected T NewStub( MailboxMessageSource src );

        public abstract override string ToString();

        private T stub;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="src"></param>
        /// <param name="sender"></param>
        /// <param name="msg"></param>
        /// Exception:
        ///     throws Exception
        public bool Message( Who sender, Message msg )
        {
            return stub.Message(sender, msg);
        }

        public MailboxMessageSource GetSource()
        {
            return _src;
        }

        public void SetSource( MailboxMessageSource src )
        {
            _src = src;
        }

        private MailboxMessageSource _src;

        public Object SessionQuery( Object query )
        {
            throw new Exception( " unknown query " + query );
        }

        public void SessionControl( Object control, Object value )
        {
            throw new Exception( " unknown control " + control );
        }

        public void SessionNotify( Object eventObj )
        {
            if ( eventObj.Equals( SessionConsts.UP ) )
            {
                stub = NewStub( _src );
                return;
            }

            if ( eventObj.Equals( SessionConsts.DOWN ) )
            {
                stub = null;
                return;
            }
        }
    }
}
