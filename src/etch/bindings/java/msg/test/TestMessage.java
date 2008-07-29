/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import metreos.util.FlexBuffer;

import org.junit.Test;

import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.TaggedDataOutput;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator_int;
import etch.bindings.java.msg.Validator_long;
import etch.bindings.java.msg.ValueFactory;

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

	/** @throws IOException */
	@Test
	public void writeMessage() throws IOException
	{
		Message msg = new Message( mt1, vf );
		writeHelper( msg );
		
		msg.put( mf1, 123 );
		writeHelper( msg );
		
		msg.put( mf2, 234 );
		writeHelper( msg );
	}
	
	private void writeHelper( Message msg ) throws IOException
	{
		TaggedDataOutput tdo = new FakeTdo( msg );
		msg.writeMessage( tdo );
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
		
		public Field addField( Field field )
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Type addType( Type type )
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public StructValue exportCustomValue( Object value )
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Field getField( Integer id )
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Field getField( String name )
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public Collection<Field> getFields()
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

		public Collection<Type> getTypes()
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

		public Class<?> getCustomType(Type type) throws UnsupportedOperationException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}
	}

	/** */
	public static class FakeTdo implements TaggedDataOutput
	{
		/** @param msg */
		public FakeTdo( Message msg )
		{
			this.xmsg = msg;
		}
		
		private final Message xmsg;
		
		public void startMessage( Message msg ) throws IOException
		{
			assertSame( xmsg, msg );
			assertFalse( started );
			assertFalse( ended );
			
			started = true;
		}
		
		private boolean started;
		
		private boolean ended;

		public void writeStructElement( Field key, Object value ) throws IOException
		{
			assertTrue( started );
			assertFalse( ended );
			items.put( key, value );
		}
		
		private Map<Field, Object> items = new HashMap<Field, Object>();

		public void endMessage( Message msg ) throws IOException
		{
			assertSame( xmsg, msg );
			assertTrue( started );
			assertFalse( ended );
			
			ended = true;
		}

		/** @throws IOException */
		public void close() throws IOException
		{
			assertTrue( started );
			assertTrue( ended );
			
			assertEquals( xmsg.size(), items.size() );
			assertEquals( xmsg.entrySet(), items.entrySet() );
		}

		public void endArray( ArrayValue array ) throws IOException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public void endStruct( StructValue struct ) throws IOException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public void startArray( ArrayValue array ) throws IOException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public void startStruct( StructValue struct ) throws IOException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public void writeArrayElement( Object value ) throws IOException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public void setBuffer( FlexBuffer msgBuf )
		{
			// TODO Auto-generated method stub
			
		}
	}
}
