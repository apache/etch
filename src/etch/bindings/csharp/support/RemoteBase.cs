///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 25, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using Etch.Msg;
using Etch.Transport;

namespace Etch.Support
{
    public class RemoteBase<T> where T : ValueFactory
    {
        /// <summary>Constructs the RemoteBase.</summary>
        /// <param name="svc">svc</param>
        /// <param name="vf">vf </param>
        public RemoteBase(DeliveryService svc, T vf)
        {
            this._svc = svc;
            this._vf = vf;
        }

        /// <summary>The delivery service used to send messages.</summary>
        public DeliveryService _svc;

        /// <summary>The value factory used to create messages and encode/decode types.</summary>
        public T _vf;

        /// <param name="type">type</param>
        /// <returns>a new message</returns>
        public Message _NewMessage(XType type)
        {
            return new Message(type, _vf);
        }

        /// <summary>
        /// Gets a configuration or operational value from the source. The
        /// request is passed down the chain of sources until some source
        /// recognises the query, whereupon it returns the requested value.
        /// </summary>
        /// 
        /// <param name="query">an object representing a query, which could be as
        /// simple as a string, integer, or enum, or more complex such as
        /// a class with instance variables for query terms.</param>
        /// <returns>the requested value, or null if not defined.</returns>
        /// 
        /// Exception:
        ///     throws Exception if the query is not recognised
        ///     by any source (which is to say, if the last source in the source
        ///     chain does not recognise it, it should throw this exception).
        ///     
        public object _TransportQuery( object query )
        {
            return _svc.TransportQuery( query );
        }

        /// <summary>
        /// Sets a configuration or operational value in the source. The
        /// request is passed down the chain of sources until some source
        /// recognises the control, whereupon it stores the specified value
        /// and returns.
        /// </summary>
        /// 
        /// <param name="control">an object representing a control, which could be as
        /// simple as a string, integer, or enum, or more complex such as
        /// a class with instance variables for control terms.</param>
        /// 
        /// <param name="value">the value to set.</param>
        /// 
        /// Exception: 
        ///     throws ArgumentException if the value is not the right
        ///     type or if the value is inappropriate.
        ///     
        ///     throws Exception if the control is not recognised
        ///     by any source (which is to say, if the last source in the source
        ///     chain does not recognise it, it should throw this exception).
        /// 
        public void _TransportControl( object control, object value )
        {
            _svc.TransportControl( control, value );
        }

        /// <summary>
        /// Notifies the chain of sources of the specified event. Unlike query
        /// and control operations above, events are always passed down to the
        /// bottom to allow all sources to notice them.
        /// </summary>
        /// 
        /// <param name="eventObj">a class which represents the event, possibly with
        /// parameters. The simplest event could be a string, integer,
        /// or enum, but any class instance will do (as long as some source
        /// in the chain expects it).</param>
        /// 
        public void _TransportNotify( object eventObj )
        {
            _svc.TransportNotify( eventObj );
        }

        /// <summary>
        /// Start the transport
        /// </summary>
        public void _Start()
        {
            _svc.TransportControl( Etch.Transport.TransportConsts.START, null );
        }

        /// <summary>
        /// Waits for the transport to come up.
        /// </summary>
        /// <param name="maxDelay"></param>
        public void _WaitUp( int maxDelay )
        {
            _svc.TransportQuery( new TransportConsts.WaitUp( maxDelay ) );
        }

        /// <summary>
        /// Starts the transport and waits for it to come up.
        /// </summary>
        /// <param name="maxDelay"></param>
        public void _StartAndWaitUp( int maxDelay )
        {
            _svc.TransportControl( TransportConsts.START_AND_WAIT_UP, maxDelay );
        }

        /// <summary>
        /// Stops the transport.
        /// </summary>
        public void _Stop()
        {
            _svc.TransportControl( TransportConsts.STOP, null );
        }

        /// <summary>
        /// Waits for the transport to go down.
        /// </summary>
        /// <param name="maxDelay"></param>
        public void _WaitDown( int maxDelay )
        {
            _svc.TransportQuery( new TransportConsts.WaitDown( maxDelay ) );
        }

        /// <summary>
        /// Stops the transport and waits for it to go down.
        /// </summary>
        /// <param name="maxDelay">maxdelay in milliseconds</param>
        public void _StopAndWaitDown( int maxDelay )
        {
            _svc.TransportControl( TransportConsts.STOP_AND_WAIT_DOWN, maxDelay );
        }

    }
}