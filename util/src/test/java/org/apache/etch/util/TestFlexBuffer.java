/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.util;

import static org.junit.Assert.assertEquals;

import java.io.EOFException;
import java.io.IOException;

import org.junit.Test;

/**
 * Tests FlexBuffer.
 */
public class TestFlexBuffer
{
	private void checkBuf( FlexBuffer buf, int length, int index, int avail )
	{
		assertEquals( "length", length, buf.length() );
		assertEquals( "index", index, buf.index() );
		assertEquals( "avail", avail, buf.avail() );
	}
	
	/** @throws Exception */
	@Test
	public void initial1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		checkBuf( buf, 0, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void initial2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[5] );
		checkBuf( buf, 5, 0, 5 );
	}
	
	/** @throws Exception */
	@Test
	public void initial3() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[5], 2 );
		checkBuf( buf, 2, 0, 2 );
	}
	
	/** @throws Exception */
	@Test
	public void initial4() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[5], 1, 3 );
		checkBuf( buf, 4, 1, 3 );
	}

	/** @throws Exception */
	@Test
	public void setLength0() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 0 );
		checkBuf( buf, 0, 0, 0 );
	}

	/** @throws Exception */
	@Test
	public void setLength1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 1 );
		checkBuf( buf, 1, 0, 1 );
	}

	/** @throws Exception */
	@Test
	public void setLength2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 2 );
		checkBuf( buf, 2, 0, 2 );
	}

	/** @throws Exception */
	@Test
	public void setLength3() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 2 );
		buf.setLength( 0 );
		checkBuf( buf, 0, 0, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void setLength4() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( -1 );
	}

	/** @throws Exception */
	@Test
	public void setIndex0() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setIndex( 0 );
		checkBuf( buf, 0, 0, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void setIndex1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setIndex( 1 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void setIndex2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setIndex( -1 );
	}

	/** @throws Exception */
	@Test
	public void setIndex3() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 5 );
		buf.setIndex( 1 );
		checkBuf( buf, 5, 1, 4 );
	}

	/** @throws Exception */
	@Test
	public void setIndex4() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 5 );
		buf.setIndex( 4 );
		checkBuf( buf, 5, 4, 1 );
	}

	/** @throws Exception */
	@Test
	public void setIndex5() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 5 );
		buf.setIndex( 5 );
		checkBuf( buf, 5, 5, 0 );
	}

	/** @throws Exception */
	@Test
	public void setIndex6() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 5 );
		buf.setIndex( 5 );
		buf.setLength( 0 );
		checkBuf( buf, 0, 0, 0 );
	}

	/** @throws Exception */
	@Test
	public void setIndex7() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 5 );
		buf.setIndex( 5 );
		buf.setLength( 2 );
		checkBuf( buf, 2, 2, 0 );
	}

	/** @throws Exception */
	@Test
	public void reset() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.setLength( 2 );
		buf.setIndex( 1 );
		buf.reset();
		checkBuf( buf, 0, 0, 0 );
	}

	// ------------------------------------------------------
	// cases for put( int ):
	// put once or twice (the put 0 case has already been tested).

	/** @throws Exception */
	@Test
	public void put1a() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.put( 1 );
		checkBuf( buf, 1, 1, 0 );
	}

	/** @throws Exception */
	@Test
	public void put1b() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.put( 1 );
		buf.put( 2 );
		checkBuf( buf, 2, 2, 0 );
	}

	// ------------------------------------------------------
	// cases for put( byte[] buf ):
	// buf.length = null, 0, 1, 2

	private void testPutBytes( int bufLen )
		throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.put( 1 );
		buf.put( bufLen >= 0 ? new byte[bufLen] : null );
		checkBuf( buf, bufLen+1, bufLen+1, 0 );
	}

	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void put2a() throws Exception
	{
		testPutBytes( -1 );
	}

	/** @throws Exception */
	@Test
	public void put2b() throws Exception
	{
		testPutBytes( 0 );
	}

	/** @throws Exception */
	@Test
	public void put2c() throws Exception
	{
		testPutBytes( 1 );
	}

	/** @throws Exception */
	@Test
	public void put2d() throws Exception
	{
		testPutBytes( 2 );
	}
	
	// ------------------------------------------------------
	// cases for put( byte[] buf, int off, int len ):
	// buf.length, off, len // failure reason
	// --- group 3:
	// null, 0, 0 // fail (buf is null)
	// null, 0, 1 // fail (buf is null)
	// null, 1, 0 // fail (buf is null)
	// --- group 4:
	// 0, 0, 0
	// 0, 0, 1 // fail (off+len > buf.length)
	// 0, 1, 0 // fail (off > buf.length)
	// --- group 5:
	// 1, 0, 0
	// 1, 0, 1
	// 1, 0, 2 // fail (off+len > buf.length)
	// 1, 1, 0
	// 1, 1, 1 // fail (off+len > buf.length)
	// 1, 1, 2 // fail (off+len > buf.length)
	// 1, 2, 0 // fail (off > buf.length)
	// 1, 2, 1 // fail (off > buf.length)
	// 1, 2, 2 // fail (off > buf.length)
	// --- group 6:
	// 2, 0, 0
	// 2, 0, 1
	// 2, 0, 2
	// 2, 1, 0
	// 2, 1, 1
	// 2, 1, 2 // fail (off+len > buf.length)
	// 2, 2, 0
	// 2, 2, 1 // fail (off+len > buf.length)
	// 2, 2, 2 // fail (off+len > buf.length)
	
	private void testPutBytesOffLen( int bufLen, int offset, int length )
		throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.put( 1 );
		buf.put( bufLen >= 0 ? new byte[bufLen] : null, offset, length );
		checkBuf( buf, length+1, length+1, 0 );
	}

	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void put3a() throws Exception
	{
		testPutBytesOffLen( -1, 0, 0 ); // fail (buf is null)
	}

	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void put3b() throws Exception
	{
		testPutBytesOffLen( -1, 0, 1 ); // fail (buf is null)
	}

	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void put3c() throws Exception
	{
		testPutBytesOffLen( -1, 1, 0 ); // fail (buf is null)
	}
	
	/** @throws Exception */
	@Test
	public void put4a() throws Exception
	{
		testPutBytesOffLen( 0, 0, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put4b() throws Exception
	{
		testPutBytesOffLen( 0, 0, 1 ); // fail (off+len > buf.length)
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put4c() throws Exception
	{
		testPutBytesOffLen( 0, 1, 0 ); // fail (off > buf.length)
	}

	/** @throws Exception */
	@Test
	public void put5a() throws Exception
	{
		testPutBytesOffLen( 1, 0, 0 );
	}

	/** @throws Exception */
	@Test
	public void put5b() throws Exception
	{
		testPutBytesOffLen( 1, 0, 1 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put5c() throws Exception
	{
		testPutBytesOffLen( 1, 0, 2 ); // fail (off+len > buf.length)
	}

	/** @throws Exception */
	@Test
	public void put5d() throws Exception
	{
		testPutBytesOffLen( 1, 1, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put5e() throws Exception
	{
		testPutBytesOffLen( 1, 1, 1 ); // fail (off+len > buf.length)
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put5f() throws Exception
	{
		testPutBytesOffLen( 1, 1, 2 ); // fail (off+len > buf.length)
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put5g() throws Exception
	{
		testPutBytesOffLen( 1, 2, 0 ); // fail (off > buf.length)
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put5h() throws Exception
	{
		testPutBytesOffLen( 1, 2, 1 ); // fail (off > buf.length)
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put5i() throws Exception
	{
		testPutBytesOffLen( 1, 2, 2 ); // fail (off > buf.length)
	}
	
	/** @throws Exception */
	@Test
	public void put6a() throws Exception
	{
		testPutBytesOffLen( 2, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void put6b() throws Exception
	{
		testPutBytesOffLen( 2, 0, 1 );
	}
	
	/** @throws Exception */
	@Test
	public void put6c() throws Exception
	{
		testPutBytesOffLen( 2, 0, 2 );
	}
	
	/** @throws Exception */
	@Test
	public void put6d() throws Exception
	{
		testPutBytesOffLen( 2, 1, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void put6e() throws Exception
	{
		testPutBytesOffLen( 2, 1, 1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put6f() throws Exception
	{
		testPutBytesOffLen( 2, 1, 2 ); // fail (off+len > buf.length)
	}
	
	/** @throws Exception */
	@Test
	public void put6g() throws Exception
	{
		testPutBytesOffLen( 2, 2, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put6h() throws Exception
	{
		testPutBytesOffLen( 2, 2, 1 ); // fail (off+len > buf.length)
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put6i() throws Exception
	{
		testPutBytesOffLen( 2, 2, 2 ); // fail (off+len > buf.length)
	}

	// ------------------------------------------------------
	// cases for put(int), get():
	// put 0, get 0. // already tested above
	// put 0, get 1. // fails with EOFException
	// put 1, get 0.
	// put 1, get 1.
	// put 1, get 2. // fails with EOFException
	// put 2, get 0.
	// put 2, get 1.
	// put 2, get 2.
	// put 2, get 3. // fails with EOFException
	// put 10000, get 10000.
	
	// just like above, putting byte array instead
	// just like above, getting byte array instead.
	// just like above, putting and getting byte array.
	
	private void testPutGet( int nPuts, int nGets )
		throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		
		for (int i = 0; i < nPuts; i++)
			buf.put( i );
		
		buf.setIndex( 0 );
		
		for (int i = 0; i < nGets; i++)
			assertEquals( (byte) i, (byte) buf.get() );
		
		checkBuf( buf, nPuts, nGets, nPuts-nGets );
	}
	
	private void testPutBytesGet( int nPuts, int nGets )
		throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		
		byte[] buffer = new byte[nPuts];
		for (int i = 0; i < nPuts; i++)
			buffer[i] = (byte) i;
		
		buf.put(  buffer );
		buf.setIndex( 0 );
		
		for (int i = 0; i < nGets; i++)
			assertEquals( (byte) i, (byte) buf.get() );
		
		checkBuf( buf, nPuts, nGets, nPuts-nGets );
	}
	
	private void testPutGetBytes( int nPuts, int nGets )
		throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		
		for (int i = 0; i < nPuts; i++)
			buf.put( i );
		
		buf.setIndex( 0 );
		
		byte[] buffer = new byte[nGets];
		int n = buf.get( buffer );
		assertEquals( Math.min( nPuts, nGets ), n );
		
		for (int i = 0; i < n; i++)
			assertEquals( (byte) i, buffer[i] );
		
		checkBuf( buf, nPuts, n, nPuts-n );
	}
	
	private void testPutBytesGetBytes( int nPuts, int nGets )
		throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		
		byte[] buffer = new byte[nPuts];
		for (int i = 0; i < nPuts; i++)
			buffer[i] = (byte) i;

		buf.put(  buffer );
		buf.setIndex( 0 );
		
		buffer = new byte[nGets];
		int n = buf.get( buffer );
		assertEquals( Math.min( nPuts, nGets ), n );
		
		for (int i = 0; i < n; i++)
			assertEquals( (byte) i, buffer[i] );
		
		checkBuf( buf, nPuts, n, nPuts-n );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void get1a() throws Exception
	{
		testPutGet( 0, 1 ); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void get1b() throws Exception
	{
		testPutGet( 1, 0 );
	}

	/** @throws Exception */
	@Test
	public void get1c() throws Exception
	{
		testPutGet( 1, 1 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void get1d() throws Exception
	{
		testPutGet( 1, 2 ); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void get1e() throws Exception
	{
		testPutGet( 2, 0 );
	}

	/** @throws Exception */
	@Test
	public void get1f() throws Exception
	{
		testPutGet( 2, 1 );
	}

	/** @throws Exception */
	@Test
	public void get1g() throws Exception
	{
		testPutGet( 2, 2 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void get1h() throws Exception
	{
		testPutGet( 2, 3 ); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void get1i() throws Exception
	{
		testPutGet( 10000, 9995 );
	}

	/** @throws Exception */
	@Test
	public void get1j() throws Exception
	{
		testPutGet( 10000, 10000 );
	}

	/** @throws Exception */
	@Test
	public void get1k() throws Exception
	{
		testPutGet( 100000, 100000 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void get2a() throws Exception
	{
		testPutBytesGet( 0, 1 ); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void get2b() throws Exception
	{
		testPutBytesGet( 1, 0 );
	}

	/** @throws Exception */
	@Test
	public void get2c() throws Exception
	{
		testPutBytesGet( 1, 1 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void get2d() throws Exception
	{
		testPutBytesGet( 1, 2 ); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void get2e() throws Exception
	{
		testPutBytesGet( 2, 0 );
	}

	/** @throws Exception */
	@Test
	public void get2f() throws Exception
	{
		testPutBytesGet( 2, 1 );
	}

	/** @throws Exception */
	@Test
	public void get2g() throws Exception
	{
		testPutBytesGet( 2, 2 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void get2h() throws Exception
	{
		testPutBytesGet( 2, 3 ); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void get2i() throws Exception
	{
		testPutBytesGet( 10000, 9995 );
	}

	/** @throws Exception */
	@Test
	public void get2j() throws Exception
	{
		testPutBytesGet( 10000, 10000 );
	}

	/** @throws Exception */
	@Test
	public void get2k() throws Exception
	{
		testPutBytesGet( 100000, 100000 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void get3a() throws Exception
	{
		testPutGetBytes( 0, 1 ); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void get3b() throws Exception
	{
		testPutGetBytes( 1, 0 );
	}

	/** @throws Exception */
	@Test
	public void get3c() throws Exception
	{
		testPutGetBytes( 1, 1 );
	}

	/** @throws Exception */
	@Test
	public void get3d() throws Exception
	{
		testPutGetBytes( 1, 2 );
	}

	/** @throws Exception */
	@Test
	public void get3e() throws Exception
	{
		testPutGetBytes( 2, 0 );
	}

	/** @throws Exception */
	@Test
	public void get3f() throws Exception
	{
		testPutGetBytes( 2, 1 );
	}

	/** @throws Exception */
	@Test
	public void get3g() throws Exception
	{
		testPutGetBytes( 2, 2 );
	}

	/** @throws Exception */
	@Test
	public void get3h() throws Exception
	{
		testPutGetBytes( 2, 3 );
	}

	/** @throws Exception */
	@Test
	public void get3i() throws Exception
	{
		testPutGetBytes( 10000, 9995 );
	}

	/** @throws Exception */
	@Test
	public void get3j() throws Exception
	{
		testPutGetBytes( 10000, 10000 );
	}

	/** @throws Exception */
	@Test
	public void get3k() throws Exception
	{
		testPutGetBytes( 100000, 100000 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void get4a() throws Exception
	{
		testPutBytesGetBytes( 0, 1 ); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void get4b() throws Exception
	{
		testPutBytesGetBytes( 1, 0 );
	}

	/** @throws Exception */
	@Test
	public void get4c() throws Exception
	{
		testPutBytesGetBytes( 1, 1 );
	}

	/** @throws Exception */
	@Test
	public void get4d() throws Exception
	{
		testPutBytesGetBytes( 1, 2 );
	}

	/** @throws Exception */
	@Test
	public void get4e() throws Exception
	{
		testPutBytesGetBytes( 2, 0 );
	}

	/** @throws Exception */
	@Test
	public void get4f() throws Exception
	{
		testPutBytesGetBytes( 2, 1 );
	}

	/** @throws Exception */
	@Test
	public void get4g() throws Exception
	{
		testPutBytesGetBytes( 2, 2 );
	}

	/** @throws Exception */
	@Test
	public void get4h() throws Exception
	{
		testPutBytesGetBytes( 2, 3 );
	}

	/** @throws Exception */
	@Test
	public void get4i() throws Exception
	{
		testPutBytesGetBytes( 10000, 9995 );
	}

	/** @throws Exception */
	@Test
	public void get4j() throws Exception
	{
		testPutBytesGetBytes( 10000, 10000 );
	}

	/** @throws Exception */
	@Test
	public void get4k() throws Exception
	{
		testPutBytesGetBytes( 100000, 100000 );
	}

	/** @throws Exception */
	@Test
	public void get5a() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2, 3 } );
		assertEquals( 1, buf.get() );
		assertEquals( 2, buf.get() );
		assertEquals( 3, buf.get() );
		checkBuf( buf, 3, 3, 0 );
	}

	/** @throws Exception */
	@Test
	public void get5b() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2, 3 }, 2 );
		assertEquals( 1, buf.get() );
		assertEquals( 2, buf.get() );
		checkBuf( buf, 2, 2, 0 );
	}

	/** @throws Exception */
	@Test
	public void get5c() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5 }, 1, 2 );
		assertEquals( 2, buf.get() );
		assertEquals( 3, buf.get() );
		checkBuf( buf, 3, 3, 0 );
	}

	/** @throws Exception */
	@Test
	public void get6() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 5, 6, 7, 8 } );
		byte[] buffer = new byte[] { 11, 12, 13, 14, 15 };
		buf.get( buffer, 1, 3 );
		assertEquals( (byte) 11, buffer[0] );
		assertEquals( (byte) 5, buffer[1] );
		assertEquals( (byte) 6, buffer[2] );
		assertEquals( (byte) 7, buffer[3] );
		assertEquals( (byte) 15, buffer[4] );
	}

	/** @throws Exception */
	@Test
	public void compact() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5 }, 1, 3 );
		buf.compact();
		checkBuf( buf, 3, 0, 3 );
		assertEquals( 2, buf.get() );
		assertEquals( 3, buf.get() );
		assertEquals( 4, buf.get() );
	}

	/** @throws Exception */
	@Test
	public void getByte1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 127, 1, 0, -1, -128 } );
		assertEquals( (byte) 127, buf.getByte() );
		assertEquals( (byte) 1, buf.getByte() );
		assertEquals( (byte) 0, buf.getByte() );
		assertEquals( (byte) -1, buf.getByte() );
		assertEquals( (byte) -128, buf.getByte() );
		checkBuf( buf, 5, 5, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getByte2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.getByte(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getShort1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2, -1, -2 } );
		assertEquals( (short) 0x0102, buf.getShort() );
		assertEquals( (short) 0xfffe, buf.getShort() );
		checkBuf( buf, 4, 4, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getShort2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.getShort(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getInt1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, -1, -2, -3, -4 } );
		assertEquals( 0x01020304, buf.getInt() );
		assertEquals( 0xfffefdfc, buf.getInt() );
		checkBuf( buf, 8, 8, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getInt2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.getInt(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getLong1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			1, 2, 3, 4, 5, 6, 7, 8,
			-1, -2, -3, -4, -5, -6, -7, -8
		} );
		
		assertEquals( 0x0102030405060708L, buf.getLong() );
		assertEquals( 0xfffefdfcfbfaf9f8L, buf.getLong() );
		checkBuf( buf, 16, 16, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getLong2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.getLong(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getFloat1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			0, 0, 0, 0
			, 63, -128, 0, 0
			, 83, -113, 48, -37
			, 100, -98, -112, -120
			, 117, -120, 20, 25
			, -65, -128, 0, 0
			, -45, -113, 48, -37
			, -28, -98, -112, -120
			, -11, -120, 20, 25
			, 50, 83, 79, -7
			, 34, 44, -87, 90
			, 17, -38, -85, 115
			, -78, 83, 79, -7
			, -94, 44, -87, 90
			, -111, -38, -85, 115
		} );

		assertEquals( 0.0f, buf.getFloat() );
		assertEquals( 1.0f, buf.getFloat() );
		assertEquals( 123e10f, buf.getFloat() );
		assertEquals( 234e20f, buf.getFloat() );
		assertEquals( 345e30f, buf.getFloat() );
		assertEquals( -1.0f, buf.getFloat() );
		assertEquals( -123e10f, buf.getFloat() );
		assertEquals( -234e20f, buf.getFloat() );
		assertEquals( -345e30f, buf.getFloat() );
		assertEquals( 123e-10f, buf.getFloat() );
		assertEquals( 234e-20f, buf.getFloat() );
		assertEquals( 345e-30f, buf.getFloat() );
		assertEquals( -123e-10f, buf.getFloat() );
		assertEquals( -234e-20f, buf.getFloat() );
		assertEquals( -345e-30f, buf.getFloat() );
		checkBuf( buf, 15*4, 15*4, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getFloat2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.getFloat(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getDouble1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			0, 0, 0, 0, 0, 0, 0, 0
			, 63, -16, 0, 0, 0, 0, 0, 0
			, 66, 113, -26, 27, 104, -64, 0, 0
			, 85, 48, -73, 88, 68, 89, -6, -80
			, 105, -4, 43, 112, 122, -31, 67, -116
			, -65, -16, 0, 0, 0, 0, 0, 0
			, -62, 113, -26, 27, 104, -64, 0, 0
			, -43, 48, -73, 88, 68, 89, -6, -80
			, -23, -4, 43, 112, 122, -31, 67, -116
			, 62, 74, 105, -1, 27, 85, 80, 81
			, 43, -87, -105, 64, -36, -23, -48, -66
			, 22, -16, -127, 73, 111, 91, -75, -29
			, -66, 74, 105, -1, 27, 85, 80, 81
			, -85, -87, -105, 64, -36, -23, -48, -66
			, -106, -16, -127, 73, 111, 91, -75, -29
		});
				
		assertEquals( 0.0,              buf.getDouble());
		
		assertEquals( 1.0,              buf.getDouble());
		assertEquals( 123e10,           buf.getDouble());
		assertEquals( 234e100,          buf.getDouble());
		assertEquals( 345e200,          buf.getDouble());
		
		assertEquals( -1.0,             buf.getDouble());
		assertEquals( -123e10,          buf.getDouble());
		assertEquals( -234e100,         buf.getDouble());
		assertEquals( -345e200,         buf.getDouble());

		assertEquals( 123e-10,          buf.getDouble());
		assertEquals( 234e-100,         buf.getDouble());
		assertEquals( 345e-200,         buf.getDouble());

		assertEquals( -123e-10,          buf.getDouble());
		assertEquals( -234e-100,         buf.getDouble());
		assertEquals( -345e-200,         buf.getDouble());

		checkBuf( buf, 15*8, 15*8, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getDouble2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.getDouble(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getFully1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5, 6 } );
		byte[] b = new byte[3];
		buf.getFully( b );
		assertEquals( (byte) 1, b[0] );
		assertEquals( (byte) 2, b[1] );
		assertEquals( (byte) 3, b[2] );
		buf.getFully( b );
		assertEquals( (byte) 4, b[0] );
		assertEquals( (byte) 5, b[1] );
		assertEquals( (byte) 6, b[2] );
		checkBuf( buf, 6, 6, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getFully2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2 } );
		byte[] b = new byte[3];
		buf.getFully( b ); // fails with EOFException
	}
	
	// ------------------------------------------------------
	// cases for FlexBuffer put( FlexBuffer ):
	// put, put FlexBuffer with valid buffer
    // put, put Flexbuffer to a Flexbuffer approaching max length, fails with IOException.
	
	/** @throws Exception */
	@Test
	public void putFlexBuffer1() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		FlexBuffer buf = new FlexBuffer();
		buf.put( buf0 );
		checkBuf( buf, buf0.length(), buf0.length(), 0 );
	}

	/** @throws Exception */
	@Test( expected = IOException.class )
	public void putFlexBuffer2() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		
		int max = 4*1024*1024;
		FlexBuffer buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.put( buf0 );
	}
		
	// ------------------------------------------------------
	// cases for FlexBuffer put( FlexBuffer, int ):
	// put, put FlexBuffer with length larger than available bytes, fails with IllegalArgumentException
	// put, put FlexBuffer with valid length value
    // put, put Flexbuffer to a Flexbuffer approaching max length, fails with IOException.
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void putFlexBuffer3() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		
		FlexBuffer buf = new FlexBuffer();
		buf.setIndex(0);
		buf.put( buf0, 3);
	}
	
	/** @throws Exception */
	@Test
	public void putFlexBuffer4() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		FlexBuffer buf = new FlexBuffer();
		buf.put( buf0, 1 );
		checkBuf( buf, 1, 1, 0 );
	}

	/** @throws Exception */
	@Test( expected = IOException.class )
	public void putFlexBuffer5() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		int max = 4*1024*1024;
		FlexBuffer buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.put( buf0, 1);
	}
	
	// ------------------------------------------------------
	// cases for void putByte( byte ):
	// putByte, put byte value and expect getByte to return matched value.
    // putByte to a Flexbuffer approaching max length, fails with IOException.
	
	/** @throws Exception */
	@Test
	public void putByte1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.putByte( (byte) 0x01 );
		checkBuf( buf, 1, 1, 0 );
		buf.setIndex(0);
		assertEquals( (byte) 0x01, buf.getByte() );
		checkBuf( buf, 1, 1, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IOException.class )
	public void putByte2() throws Exception
	{
		int max = 4*1024*1024;
		FlexBuffer buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putByte( (byte) 0x01 );
	}
	
	// ------------------------------------------------------
	// cases for void putDouble( double ):
	// putDouble, put Double and expect getDouble to return matched value.
    // putDouble to a Flexbuffer approaching max length, fails with IOException.
	
	/** @throws Exception */
	@Test
	public void putDouble1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		// some reasonable values
		buf.putDouble( 0.0 );
		buf.putDouble( 1.0 );
		buf.putDouble( 123e10 );
		buf.putDouble( 234e100 );
		buf.putDouble( 345e200 );
		// negative of some reasonable values
		buf.putDouble( -1.0 );
		buf.putDouble( -123e10 );
		buf.putDouble( -234e100 );
		buf.putDouble( -345e200 );
		// tiny
		buf.putDouble( 123e-10 );
		buf.putDouble( 234e-100 );
		buf.putDouble( 345e-200 );
		// tiny and negative
		buf.putDouble( -123e-10 );
		buf.putDouble( -234e-100 );
		buf.putDouble( -345e-200 );
		checkBuf( buf, 15*8, 15*8, 0 );
		
		// verify we can reliably read the data we wrote.
		
		buf.setIndex( 0 );
		assertEquals( 0.0, buf.getDouble() );
		assertEquals( 1.0, buf.getDouble() );
		assertEquals( 123e10, buf.getDouble() );
		assertEquals( 234e100, buf.getDouble() );
		assertEquals( 345e200, buf.getDouble() );
		assertEquals( -1.0, buf.getDouble() );
		assertEquals( -123e10, buf.getDouble() );
		assertEquals( -234e100, buf.getDouble() );
		assertEquals( -345e200, buf.getDouble() );
		assertEquals( 123e-10, buf.getDouble() );
		assertEquals( 234e-100, buf.getDouble() );
		assertEquals( 345e-200, buf.getDouble() );
		assertEquals( -123e-10, buf.getDouble() );
		assertEquals( -234e-100, buf.getDouble() );
		assertEquals( -345e-200, buf.getDouble() );
		checkBuf( buf, 15*8, 15*8, 0 );
		
		// uncomment this code to get a dump of the actual
		// bytes for the above data.
		
//		buf.setIndex( 0 );
//		int i = 0;
//		while (buf.avail() > 0)
//		{
//			System.out.printf( i == 0 ? "%d" : ", %d", buf.getByte() );
//			if (i++ % 8 == 7)
//				System.out.println();
//		}
		
		byte[] expected =
		{
			0, 0, 0, 0, 0, 0, 0, 0
			, 63, -16, 0, 0, 0, 0, 0, 0
			, 66, 113, -26, 27, 104, -64, 0, 0
			, 85, 48, -73, 88, 68, 89, -6, -80
			, 105, -4, 43, 112, 122, -31, 67, -116
			, -65, -16, 0, 0, 0, 0, 0, 0
			, -62, 113, -26, 27, 104, -64, 0, 0
			, -43, 48, -73, 88, 68, 89, -6, -80
			, -23, -4, 43, 112, 122, -31, 67, -116
			, 62, 74, 105, -1, 27, 85, 80, 81
			, 43, -87, -105, 64, -36, -23, -48, -66
			, 22, -16, -127, 73, 111, 91, -75, -29
			, -66, 74, 105, -1, 27, 85, 80, 81
			, -85, -87, -105, 64, -36, -23, -48, -66
			, -106, -16, -127, 73, 111, 91, -75, -29
		};
		
		buf.setIndex( 0 );
		byte[] actual = buf.getAvailBytes();
		
		bytesAssertEquals( expected, actual );
	}
	
	private void bytesAssertEquals( byte[] expected, byte[] actual )
	{
		assertEquals( expected.length, actual.length );
		for (int i = 0; i < expected.length; i++)
			assertEquals( "expected["+i+"]/actual["+i+"]", expected[i], actual[i] );
	}

	/** @throws Exception */
	@Test( expected = IOException.class )
	public void putDouble2() throws Exception
	{
		int max = 4*1024*1024;
		FlexBuffer buf = new FlexBuffer( new byte[max] );
		buf.setIndex( max );
		buf.putDouble( 0.0 );
	}
		
	// ------------------------------------------------------
	// cases for void putFloat( float ):
	// putFloat, put Float and expect getFloat to return matched value.
    // putFloat to a Flexbuffer approaching max length, fails with IOException.
	
	/** @throws Exception */
	@Test
	public void putFloat1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		// some reasonable values
		buf.putFloat( 0.0f );
		buf.putFloat( 1.0f );
		buf.putFloat( 123e10f );
		buf.putFloat( 234e20f );
		buf.putFloat( 345e30f );
		// negative of some reasonable values
		buf.putFloat( -1.0f );
		buf.putFloat( -123e10f );
		buf.putFloat( -234e20f );
		buf.putFloat( -345e30f );
		// tiny
		buf.putFloat( 123e-10f );
		buf.putFloat( 234e-20f );
		buf.putFloat( 345e-30f );
		// tiny and negative
		buf.putFloat( -123e-10f );
		buf.putFloat( -234e-20f );
		buf.putFloat( -345e-30f );
		checkBuf( buf, 15*4, 15*4, 0 );
		
		// verify we can reliably read the data we wrote.
		
		buf.setIndex( 0 );
		assertEquals( 0.0f, buf.getFloat() );
		assertEquals( 1.0f, buf.getFloat() );
		assertEquals( 123e10f, buf.getFloat() );
		assertEquals( 234e20f, buf.getFloat() );
		assertEquals( 345e30f, buf.getFloat() );
		assertEquals( -1.0f, buf.getFloat() );
		assertEquals( -123e10f, buf.getFloat() );
		assertEquals( -234e20f, buf.getFloat() );
		assertEquals( -345e30f, buf.getFloat() );
		assertEquals( 123e-10f, buf.getFloat() );
		assertEquals( 234e-20f, buf.getFloat() );
		assertEquals( 345e-30f, buf.getFloat() );
		assertEquals( -123e-10f, buf.getFloat() );
		assertEquals( -234e-20f, buf.getFloat() );
		assertEquals( -345e-30f, buf.getFloat() );
		checkBuf( buf, 15*4, 15*4, 0 );
		
		// uncomment this code to get a dump of the actual
		// bytes for the above data.
		
//		buf.setIndex( 0 );
//		int i = 0;
//		while (buf.avail() > 0)
//		{
//			System.out.printf( i == 0 ? "%d" : ", %d", buf.getByte() );
//			if (i++ % 8 == 7)
//				System.out.println();
//		}
		
		byte[] expected =
		{
			0, 0, 0, 0
			, 63, -128, 0, 0
			, 83, -113, 48, -37
			, 100, -98, -112, -120
			, 117, -120, 20, 25
			, -65, -128, 0, 0
			, -45, -113, 48, -37
			, -28, -98, -112, -120
			, -11, -120, 20, 25
			, 50, 83, 79, -7
			, 34, 44, -87, 90
			, 17, -38, -85, 115
			, -78, 83, 79, -7
			, -94, 44, -87, 90
			, -111, -38, -85, 115
		};
		
		buf.setIndex( 0 );
		byte[] actual = buf.getAvailBytes();
		
		bytesAssertEquals( expected, actual );
	}
	
	/** @throws Exception */
	@Test( expected = IOException.class )
	public void putFloat2() throws Exception
	{
		int max = 4*1024*1024;
		FlexBuffer buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putFloat( 0.0f );
	}
	
	// ------------------------------------------------------
	// cases for void putInt( int ):
	// putInt, put Int and expect getInt to return matched value.
    // putInt to a Flexbuffer approaching max length, fails with IOException.
	
	/** @throws Exception */
	@Test
	public void putInt1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.putInt( Integer.MAX_VALUE );
		checkBuf( buf, 4, 4, 0 );
		buf.setIndex(0);
		assertEquals( Integer.MAX_VALUE, buf.getInt() );
		checkBuf( buf, 4, 4, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IOException.class )
	public void putInt2() throws Exception
	{
		int max = 4*1024*1024;
		FlexBuffer buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putInt( Integer.MIN_VALUE );
	}
		
	// ------------------------------------------------------
	// cases for void putLong( long ):
	// putLong, put Long and expect getLong to return matched value.
    // putShort to a Flexbuffer approaching max length, fails with IOException.
	
	/** @throws Exception */
	@Test
	public void putLong1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.putLong( Long.MAX_VALUE );
		checkBuf( buf, 8, 8, 0 );
		buf.setIndex(0);
		assertEquals( Long.MAX_VALUE, buf.getLong() );
		checkBuf( buf, 8, 8, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IOException.class )
	public void putLong2() throws Exception
	{
		int max = 4*1024*1024;
		FlexBuffer buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putLong( Long.MIN_VALUE );
	}
		
	// ------------------------------------------------------
	// cases for void putShort( short ):
	// putShort, put short and expect getShort to return matched value.
    // putShort to a Flexbuffer approaching max length, fails with IOException.
	
	/** @throws Exception */
	@Test
	public void putShort1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.putShort( Short.MAX_VALUE );
		checkBuf( buf, 2, 2, 0 );
		buf.setIndex(0);
		assertEquals( Short.MAX_VALUE, buf.getShort() );
		checkBuf( buf, 2, 2, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IOException.class )
	public void putShort2() throws Exception
	{
		int max = 4*1024*1024;
		FlexBuffer buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putShort( (short) 1 );
	}
		
	// ------------------------------------------------------
	// cases for FlexBuffer skip( int, boolean ):
	// skip length < 0, put = false.  				// fails with illegal argument
	// skip length > buffer length, put = false.  	// fails with EOFException
	// skip length = 0, put = false.
	// skip length = buffer length, put = false.
	// skip length < buffer length, put = false.
	// skip length > buffer length, put = true.
	// skip length < buffer length, put = true.
    // skip length = max buffer length, put = true.	// fails with IOException
		
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void skip1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.skip(-1, false);
	}
	
	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void skip2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(3, false);
	}
	
	/** @throws Exception */
	@Test
	public void skip3() throws Exception
	{
		FlexBuffer buf = new FlexBuffer();
		buf.skip(0, false);
		checkBuf( buf, 0, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void skip4() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(2, false);
		checkBuf( buf, 2, 2, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void skip5() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(1, false);
		checkBuf( buf, 2, 1, 1 );
	}
	
	/** @throws Exception */
	@Test
	public void skip6() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(1, true);
		checkBuf( buf, 2, 1, 1 );
	}

	/** @throws Exception */
	@Test
	public void skip7() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(5, true);
		checkBuf( buf, 5, 5, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IOException.class )
	public void skip8() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[] { 1, 2 } );
		int max = 4*1024*1024;
		buf.skip(max+1, true);
	}
}
