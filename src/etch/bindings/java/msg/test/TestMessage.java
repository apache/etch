/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import java.util.Set;

import org.junit.Test;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.Validator_int;
import etch.bindings.java.support.Validator_long;

/**
 * Tests Message.
 */
public class TestMessage
{
	private final MyValueFactory vf = new MyValueFactory();
	private final Type mt1 = new Type( 1, "mt" );
	private final Type rmt = new Type( 2, "rmt" );
	private final Field mf1 = new Field( 3, "x" );
	private final Field mf2 = new Field( 4, "y" );
	
	{
		mt1.putValidator( mf1, Validator_int.get( 0 ) );
		mt1.putValidator( mf2, Validator_int.get( 0 ) );
		mt1.putValidator( vf.mf_messageId, Validator_long.get( 0 ) );
		
		rmt.putValidator( vf.mf_messageId, Validator_long.get( 0 ) );
		rmt.putValidator( vf.mf_inReplyTo, Validator_long.get( 0 ) );
	}
	
	/** */
	@Test
	public void getVf()
	{
		Message msg = new Message( mt1, vf );
		assertSame( vf, msg.vf() );
	}

	/** */
	@Test
	public void reply()
	{
		Message msg = new Message( mt1, vf );
		msg.setMessageId( 12345L );

		Message rmsg = msg.reply( rmt );
		assertSame( rmt, rmsg.type() );
		assertSame( vf, rmsg.vf() );
		assertEquals( 12345L, rmsg.getInReplyTo() );
	}

	/** */
	@Test
	public void getMessageId()
	{
		Message msg = new Message( mt1, vf );
		assertNull( msg.getMessageId() );
		msg.setMessageId( 234L );
		assertEquals( 234L, msg.getMessageId() );
	}

	/** */
	@Test
	public void setMessageId()
	{
		Message msg = new Message( mt1, vf );
		assertNull( msg.getMessageId() );
		msg.setMessageId( 234L );
		assertEquals( 234L, msg.getMessageId() );
		msg.setMessageId( 345L );
		assertEquals( 345L, msg.getMessageId() );
	}

	/** */
	@Test
	public void getInReplyTo()
	{
		Message msg = new Message( rmt, vf );
		assertNull( msg.getInReplyTo() );
		msg.setInReplyTo( 234L );
		assertEquals( 234L, msg.getInReplyTo() );
	}

	/** */
	@Test
	public void setInReplyTo()
	{
		Message msg = new Message( rmt, vf );
		assertNull( msg.getInReplyTo() );
		msg.setInReplyTo( 234L );
		assertEquals( 234L, msg.getInReplyTo() );
		msg.setInReplyTo( 345L );
		assertEquals( 345L, msg.getInReplyTo() );
	}

	/** */
	public static class MyValueFactory implements ValueFactory
	{
		///////////////
		// MESSAGEID //
		///////////////
		
		public Long getMessageId( Message msg )
		{
			return (Long) msg.get( mf_messageId );
		}

		public void setMessageId( Message msg, Long msgid )
		{
			msg.put( mf_messageId, msgid );
		}

		/** */
		public final Field mf_messageId = new Field( "_messageId" );

		///////////////
		// INREPLYTO //
		///////////////
		
		public Long getInReplyTo( Message msg )
		{
			return (Long) msg.get( mf_inReplyTo );
		}

		public void setInReplyTo( Message msg, Long msgid )
		{
			msg.put( mf_inReplyTo, msgid );
		}

		/** */
		public final Field mf_inReplyTo = new Field( "_inReplyTo" );

		////////////
		// UNUSED //
		////////////
		
		///CLOVER:OFF

		public StructValue exportCustomValue( Object value )
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public String getStringEncoding()
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Type getType( Integer id )
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Type getType( String name )
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Set<Type> getTypes()
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Object importCustomValue( StructValue sv )
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Type getCustomStructType(Class<?> c) throws UnsupportedOperationException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Type get_mt__Etch_AuthException()
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Type get_mt__Etch_RuntimeException()
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Type get_mt__exception()
		{
			throw new UnsupportedOperationException( "not implemented" );
		}
		
		///CLOVER:ON
	}
}
