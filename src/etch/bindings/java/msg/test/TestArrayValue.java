/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import metreos.util.FlexBuffer;

import org.junit.Test;

import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.TaggedDataOutput;

/**
 * Tests ArrayValue.
 */
public class TestArrayValue
{
	/** */
	@Test
	public void addMany()
	{
		ArrayValue av = new ArrayValue().addMany( 11, 22, 33 );
		assertEquals( 3, av.size() );
		assertEquals( 11, av.get( 0 ) );
		assertEquals( 22, av.get( 1 ) );
		assertEquals( 33, av.get( 2 ) );
	}

//	/** @throws IOException */
//	@Test
//	public void read() throws IOException
//	{
//		readHelper();
//		readHelper( 1 );
//		readHelper( 2, 3.3 );
//		readHelper( 4, true, 6 );
//		readHelper( "a", "b", "c", "d" );
//	}

	/** @throws IOException */
	@Test
	public void writeArray() throws IOException
	{
		writeHelper();
		writeHelper( 1 );
		writeHelper( 2, 3.3 );
		writeHelper( 4, true, 6 );
		writeHelper( "a", "b", "c", "d" );
	}
	
//	private void readHelper( Object... vals ) throws IOException
//	{
//		FakeTdi tdi = new FakeTdi( vals );
//		compareLists( tdi.readArrayValue(), vals );
//		tdi.close();
//	}
	
	private void writeHelper( Object... vals ) throws IOException
	{
		ArrayValue av = new ArrayValue();
		for (Object o: vals)
			av.add( o );
		FakeTdo tdo = new FakeTdo( vals );
		av.write( tdo );
		tdo.close();
	}

	/**
	 * Compare a list for having known contents.
	 * @param list
	 * @param objects
	 */
	static void compareLists( List<Object> list, Object... objects )
	{
		assertEquals( objects.length, list.size() );
		Iterator<Object> j = list.iterator();
		for (Object o: objects)
			assertEquals( o, j.next() );
		assertFalse( j.hasNext() );
	}
	
	/**
	 * TaggedDataOutput for testing.
	 */
	public static class FakeTdo implements TaggedDataOutput
	{
		/**
		 * @param vals
		 */
		public FakeTdo( Object[] vals )
		{
			this.vals = vals;
		}
		
		private final Object[] vals;

		public void startArray( ArrayValue array ) throws IOException
		{
			assertFalse( started );
			assertFalse( ended );
			assertNotNull( array );
			
			started = true;
			list = new ArrayList<Object>();
			xarray = array;
		}
		
		private boolean started;
		
		private boolean ended;
		
		private List<Object> list;
		
		private ArrayValue xarray;

		public void writeArrayElement( Object value ) throws IOException
		{
			assertTrue( started );
			assertFalse( ended );
			
			list.add( value );
		}

		public void endArray( ArrayValue array ) throws IOException
		{
			assertTrue( started );
			assertFalse( ended );
			assertEquals( xarray, array );
			
			ended = true;
		}

		/**
		 * @throws IOException
		 */
		public void close() throws IOException
		{
			assertTrue( started );
			assertTrue( ended );
			compareLists( list, vals );
		}

		public void endMessage( Message msg ) throws IOException
		{
			throw new UnsupportedOperationException();
		}

		public void endStruct( StructValue struct ) throws IOException
		{
			throw new UnsupportedOperationException();
		}

		public void startMessage( Message msg ) throws IOException
		{
			throw new UnsupportedOperationException();
		}

		public void startStruct( StructValue struct ) throws IOException
		{
			throw new UnsupportedOperationException();
		}

		public void writeStructElement( Field key, Object value ) throws IOException
		{
			throw new UnsupportedOperationException();
		}

		public void setBuffer( FlexBuffer msgBuf )
		{
			// TODO Auto-generated method stub
			
		}
	}
}
