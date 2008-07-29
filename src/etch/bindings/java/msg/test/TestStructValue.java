/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import metreos.util.FlexBuffer;

import org.junit.Test;

import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.TaggedDataOutput;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator_ArrayValue;
import etch.bindings.java.msg.Validator_StructValue;
import etch.bindings.java.msg.Validator_int;

/** Tests StructValue */
public class TestStructValue
{
	private Type mt1 = new Type( "one" );
	private Type mt2 = new Type( "two" );
	private Field mf1 = new Field( "f1" );
	private Field mf2 = new Field( "f2" );
	private Field mf3 = new Field( "f3" );
	private Field mf4 = new Field( "f4" );
	private Field mf5 = new Field( "f5" );
	private Field mf6 = new Field( "f6" );
	
	{
		mt1.putValidator( mf1, Validator_int.get( 0 ) );
		mt1.putValidator( mf2, Validator_int.get( 0 ) );
		mt1.putValidator( mf3, Validator_ArrayValue.get() );
		mt1.putValidator( mf4, Validator_ArrayValue.get() );
		mt1.putValidator( mf5, Validator_StructValue.get() );
		mt1.putValidator( mf6, Validator_StructValue.get() );
	}

	/** */
	@Test
	public void getType()
	{
		StructValue sv = new StructValue( mt1 );
		assertSame( mt1, sv.type() );
		
		sv = new StructValue( mt2 );
		assertSame( mt2, sv.type() );
	}

	/** */
	@Test
	public void isType()
	{
		StructValue sv = new StructValue( mt1 );
		assertTrue( sv.isType( mt1 ) );
		assertFalse( sv.isType( mt2 ) );
		
		sv = new StructValue( mt2 );
		assertTrue( sv.isType( mt2 ) );
		assertFalse( sv.isType( mt1 ) );
	}

	/** */
	@Test
	public void checkType()
	{
		StructValue sv = new StructValue( mt1 );
		sv.checkType( mt1 );
		try { sv.checkType( mt2 ); assertTrue( false ); } catch ( IllegalArgumentException e ) { assertTrue( true ); }
		
		sv = new StructValue( mt2 );
		sv.checkType( mt2 );
		try { sv.checkType( mt1 ); assertTrue( false ); } catch ( IllegalArgumentException e ) { assertTrue( true ); }
	}

	/** */
	@Test
	public void iterator()
	{
		StructValue sv = new StructValue( mt1 );
		assertFalse( sv.iterator().hasNext() );
		
		sv.put( mf1, 1 );
		Iterator<Map.Entry<Field, Object>> i = sv.iterator();
		assertTrue( i.hasNext() );
		Map.Entry<Field, Object> me = i.next();
		assertSame( mf1, me.getKey() );
		assertEquals( 1, me.getValue() );
		assertFalse( i.hasNext() );
		
		Map<Field, Object> vals = new HashMap<Field, Object>();
		vals.put( mf2, 2 );
		vals.put( mf1, 1 );
		
		sv.put( mf2, 2 );
		i = sv.iterator();
		assertTrue( i.hasNext() );
		me = i.next();
		assertTrue( vals.containsKey( me.getKey() ) );
		assertEquals( vals.remove( me.getKey() ), me.getValue() );
		assertTrue( i.hasNext() );
		me = i.next();
		assertTrue( vals.containsKey( me.getKey() ) );
		assertEquals( vals.remove( me.getKey() ), me.getValue() );
		assertFalse( i.hasNext() );
		assertTrue( vals.isEmpty() );
	}

	/** @throws IOException */
	@Test
	public void writeStruct() throws IOException
	{
		StructValue sv = new StructValue( mt1 );
		TaggedDataOutput tdo;
		
		tdo = new FakeTdo( sv );
		sv.writeStruct( tdo );
		
		sv.put( mf1, 123 );
		tdo = new FakeTdo( sv );
		sv.writeStruct( tdo );
		
		sv.put( mf2, 234 );
		tdo = new FakeTdo( sv );
		sv.writeStruct( tdo );
	}

	/** */
	public static class FakeTdo implements TaggedDataOutput
	{
		/**
		 * @param struct
		 */
		public FakeTdo( StructValue struct )
		{
			this.xstruct = struct;
		}
		
		private final StructValue xstruct;

		public void startStruct( StructValue struct ) throws IOException
		{
			assertSame( xstruct, struct );
			assertFalse( started );
			assertFalse( ended );
			assertFalse( closed );
			
			started = true;
		}
		
		private boolean started;
		
		private boolean ended;
		
		private boolean closed;

		public void writeStructElement( Field key, Object value ) throws IOException
		{
			assertTrue( started );
			assertFalse( ended );
			assertFalse( closed );
			items.put( key, value );
		}
		
		private Map<Field, Object> items = new HashMap<Field, Object>();

		public void endStruct( StructValue struct ) throws IOException
		{
			assertSame( xstruct, struct );
			assertTrue( started );
			assertFalse( ended );
			assertFalse( closed );
			
			ended = true;
		}

		/**
		 * @throws IOException
		 */
		public void close() throws IOException
		{
			assertTrue( started );
			assertTrue( ended );
			assertFalse( closed );
			
			closed = true;
			
			assertEquals( xstruct.size(), items.size() );
			assertEquals( xstruct.entrySet(), items.entrySet() );
		}

		public void endArray( ArrayValue array ) throws IOException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public void endMessage( Message msg ) throws IOException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}

		public void startArray( ArrayValue array ) throws IOException
		{
			throw new UnsupportedOperationException( "not implemented" );
		}
		
		public void startMessage( Message msg ) throws IOException
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
