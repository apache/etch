package org.apache.etch.bindings.java.support;

import java.util.HashSet;
import java.util.Set;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.util.core.Who;

public class ComboStub implements SessionMessage
{
	public ComboStub( SessionMessage stub )
	{
		this.dstub = stub;
	}
	
	private final SessionMessage dstub;

	public boolean sessionMessage( Who sender, Message msg ) throws Exception
	{
		if (dstub.sessionMessage( sender, msg ))
			return true;
		
		for (SessionMessage stub: stubs)
			if (stub.sessionMessage( sender, msg ))
			return true;
		
		return false;
	}

	public void sessionControl( Object control, Object value ) throws Exception
	{
		dstub.sessionControl( control, value );
	}

	public void sessionNotify( Object event ) throws Exception
	{
		dstub.sessionNotify( event );
	}

	public Object sessionQuery( Object query ) throws Exception
	{
		return dstub.sessionQuery( query );
	}

	public void addStub( SessionMessage stub )
	{
		Set<SessionMessage> temp = new HashSet<SessionMessage>( stubs );
		temp.add( stub );
		stubs = temp;
	}
	
	private Set<SessionMessage> stubs = new HashSet<SessionMessage>();
}
