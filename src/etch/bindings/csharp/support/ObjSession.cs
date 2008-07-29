using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Support
{
    /// <summary>
    /// Out-of-band query, control, and notification interface for sessions.
    /// This is exactly the same as etch.bindings.java.transport.Session except
    /// the names are changed to prevent collisions with service defined names.
    /// </summary>
    public interface ObjSession
    {
        /// <summary>
        /// Gets a configuration or operational value from the handler. The
        /// request is passed up the chain of sessions until some session
        /// recognises the query, whereupon it returns the requested value.
        /// </summary>
        /// <param name="query">an object representing a query, which could be as
        /// simple as a string, integer, or enum, or more complex such as
        /// a class with instance variables for query terms.</param>
        /// <returns>the requested value, or null if not defined.</returns>
        /// Exception: 
        ///     UnsupportedOperationException if the query is not recognised
        ///     by any session (which is to say, if the last session in the session
        ///     chain does not recognise it, it should throw this exception). Typically
        ///     this would be a service's client or server implementation.
        ///     
        Object _SessionQuery( Object query );

        /// <summary>
        /// Sets a configuration or operational value in the session. The
        /// request is passed up the chain of sessions until some session
        /// recognises the control, whereupon it stores the specified value
        /// and returns.
        /// </summary>
        /// <param name="control">an object representing a control, which could be as
        /// simple as a string, integer, or enum, or more complex such as
        /// a class with instance variables for control terms.</param>
        /// <param name="value">the value to set.</param>
        /// Exception:
        ///     ArgumentException if the value is not the right
        ///     type or if the value is inappropriate.
        ///     
        ///     UnsupportedOperationException if the control is not recognised
        ///     by any session (which is to say, if the last session in the session
        ///     chain does not recognise it, it should throw this exception). Typically
        ///     this would be a service's client or server implementation.
        ///
        void _SessionControl( Object control, Object value );

        /// <summary>
        /// Notifies the chain of sessions of the specified event. Unlike query
        /// and control operations above, events are always passed up to the
        /// top to allow all sessions to notice them.
        /// </summary>
        /// <param name="eventObj">event a class which represents the event, possibly with
        /// parameters. The simplest event could be a string, integer,
        /// or enum, but any class instance will do (as long as some session
        /// in the chain expects it).</param>
        void _SessionNotify( Object eventObj );

    }
}
