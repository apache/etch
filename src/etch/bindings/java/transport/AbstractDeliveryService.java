package etch.bindings.java.transport;

import java.util.concurrent.TimeoutException;

import metreos.core.Who;
import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.DeliveryService;
import etch.bindings.java.support.Mailbox;
import etch.bindings.java.support.StubBase;

/**
 * Abstraction of DeliveryService functionality.
 */
abstract public class AbstractDeliveryService
	implements DeliveryService, MessageHandler<MailboxMessageSource>
{
	public MailboxMessageSource getSource()
	{
		return src;
	}

	public void setSource( MailboxMessageSource src )
	{
		this.src = src;
	}

	private MailboxMessageSource src;

	/**
	 * Adds a stub to the set of stubs.
	 * @param stb
	 */
	public void addStub( StubBase stb )
	{
		if (this.stub != null)
			throw new UnsupportedOperationException( "only one stub for now" );
		this.stub = stb;
	}
	
	/**
	 * Removes all the stubs in the set of stubs.
	 */
	public void removeStub()
	{
		stub = null;
	}
	
	private StubBase stub;

	public Object sessionQuery( Object query )
	{
		if (stub != null)
			return stub.sessionQuery( query );
		
		throw new UnsupportedOperationException( "unknown query "+query );
	}

	public void sessionControl( Object control, Object value )
	{
		if (stub != null)
			stub.sessionControl( control, value );
		
		throw new UnsupportedOperationException( "unknown control "+control );
	}

	public void sessionNotify( Object event )
	{
		if (stub != null)
			stub.sessionNotify( event );
		else if (event instanceof Throwable)
			((Throwable) event).printStackTrace();
	}

	public boolean message( Who sender, Message msg ) throws Exception
	{
		return stub.message( sender, msg );
	}
	
	public Object transportQuery( Object query ) throws Exception
	{
		return src.transportQuery( query );
	}
	
	public void transportControl( Object control, Object value ) throws Exception
	{
		src.transportControl( control, value );
	}

	public void transportNotify( Object event ) throws Exception
	{
		src.transportNotify( event );
	}
	
	public void send( Message msg ) throws Exception
	{
		src.messagex( null, msg );
	}

	public Mailbox begincall( Message msg )
	{
		try
		{
			return src.message( null, msg, 1, 0 );
		}
		catch ( Exception e )
		{
			throw new RuntimeException( "unexpected exception sending message", e );
		}
	}

	public void endvoidcall( Mailbox mb, Type responseType,
		Field responseField, int timeout ) throws Exception
	{
		endcall( mb, responseType, responseField, timeout );
	}

	public Object endcall( Mailbox mb, Type responseType, Field responseField, int timeout ) throws Exception
	{
		try
		{
			Mailbox.Element mbe = mb.read( timeout );
			if (mbe == null)
				throw new TimeoutException( "timeout waiting for "+responseType );
			Message rmsg = mbe.msg;
			rmsg.checkType( responseType );
			Object r = rmsg.get( responseField );
			if (r instanceof Exception)
			{
				Exception e = (Exception) r;
				e.fillInStackTrace();
				throw e;
			}
			return r;
		}
		finally
		{
			mb.closeDelivery();
		}
	}
	
	@Override
	abstract public String toString();
}
