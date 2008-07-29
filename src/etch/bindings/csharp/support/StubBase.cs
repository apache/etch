///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 25, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved. 


using System;
using Etch.Msg;
using Etch.Transport;

namespace Etch.Support{

    public class StubBase : Session, MessageHandler
    {   
	    /// <summary>Constructs the StubBase.</summary>
	    /// <param name="obj">obj</param>
        /// <param name="queued">thread pool used to run AsyncReceiverMode.QUEUED methods.</param>
        /// <param name="free">thread pool used to run AsyncReceiverMode.FREE methods.</param>
	    public StubBase( MessageSource src, Object obj, Pool queued, Pool free )
	    {
            _src = src;
		    _obj = obj;
            _queued = queued;
            _free = free;
	    }
    	
	    private readonly Object _obj;

        /// <summary>
        /// The thread pool used to run AsyncReceiverMode.QUEUED methods.
        /// </summary>
        protected readonly Pool _queued;

        /// <summary>
        /// The thread pool used to run AsyncReceiverMode.FREE methods.
        /// </summary>
        protected readonly Pool _free;

        protected readonly MessageSource _src;

        public virtual bool Message( Who sender, Message msg )
        {
            return false;
        }

        public Object SessionQuery( Object query )
        {
            if ( _obj is ObjSession )
                return ( ( ObjSession ) _obj )._SessionQuery( query );
            throw new Exception( "unknown query "+query );
        }

        public void SessionControl( Object control, Object value )
	    {
            if ( _obj is ObjSession )
                ( ( ObjSession ) _obj )._SessionControl( control, value );
            else
		        throw new Exception( "unknown control "+control );
	    }

	    public void SessionNotify( Object eventObj )
	    {
            if ( _obj is ObjSession )
                ( ( ObjSession ) _obj )._SessionNotify( eventObj );
	    }
    }
}