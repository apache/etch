package metreos.util.test;

import static org.junit.Assert.assertEquals;

import java.io.EOFException;
import java.io.IOException;

import metreos.util.FlexBuffer;

import org.junit.Test;

/**
 * Tests FlexBuffer.
 */
public class TestFlexBuffer
{
	private FlexBuffer buf = new FlexBuffer();
	
	private void checkBuf( int length, int index, int avail )
	{
		assertEquals( "length", length, buf.length() );
		assertEquals( "index", index, buf.index() );
		assertEquals( "avail", avail, buf.avail() );
	}
	
	/** @throws Exception */
	@Test
	public void initial1() throws Exception
	{
		checkBuf( 0, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void initial2() throws Exception
	{
		buf = new FlexBuffer( new byte[5] );
		checkBuf( 5, 0, 5 );
	}
	
	/** @throws Exception */
	@Test
	public void initial3() throws Exception
	{
		buf = new FlexBuffer( new byte[5], 2 );
		checkBuf( 2, 0, 2 );
	}
	
	/** @throws Exception */
	@Test
	public void initial4() throws Exception
	{
		buf = new FlexBuffer( new byte[5], 1, 3 );
		checkBuf( 4, 1, 3 );
	}

	/** @throws Exception */
	@Test
	public void setLength0() throws Exception
	{
		buf.setLength( 0 );
		checkBuf( 0, 0, 0 );
	}

	/** @throws Exception */
	@Test
	public void setLength1() throws Exception
	{
		buf.setLength( 1 );
		checkBuf( 1, 0, 1 );
	}

	/** @throws Exception */
	@Test
	public void setLength2() throws Exception
	{
		buf.setLength( 2 );
		checkBuf( 2, 0, 2 );
	}

	/** @throws Exception */
	@Test
	public void setLength3() throws Exception
	{
		buf.setLength( 2 );
		buf.setLength( 0 );
		checkBuf( 0, 0, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void setLength4() throws Exception
	{
		buf.setLength( -1 );
	}

	/** @throws Exception */
	@Test
	public void setIndex0() throws Exception
	{
		buf.setIndex( 0 );
		checkBuf( 0, 0, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void setIndex1() throws Exception
	{
		buf.setIndex( 1 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void setIndex2() throws Exception
	{
		buf.setIndex( -1 );
	}

	/** @throws Exception */
	@Test
	public void setIndex3() throws Exception
	{
		buf.setLength( 5 );
		buf.setIndex( 1 );
		checkBuf( 5, 1, 4 );
	}

	/** @throws Exception */
	@Test
	public void setIndex4() throws Exception
	{
		buf.setLength( 5 );
		buf.setIndex( 4 );
		checkBuf( 5, 4, 1 );
	}

	/** @throws Exception */
	@Test
	public void setIndex5() throws Exception
	{
		buf.setLength( 5 );
		buf.setIndex( 5 );
		checkBuf( 5, 5, 0 );
	}

	/** @throws Exception */
	@Test
	public void setIndex6() throws Exception
	{
		buf.setLength( 5 );
		buf.setIndex( 5 );
		buf.setLength( 0 );
		checkBuf( 0, 0, 0 );
	}

	/** @throws Exception */
	@Test
	public void setIndex7() throws Exception
	{
		buf.setLength( 5 );
		buf.setIndex( 5 );
		buf.setLength( 2 );
		checkBuf( 2, 2, 0 );
	}

	/** @throws Exception */
	@Test
	public void reset() throws Exception
	{
		buf.setLength( 2 );
		buf.setIndex( 1 );
		buf.reset();
		checkBuf( 0, 0, 0 );
	}

	// ------------------------------------------------------
	// cases for put( int ):
	// put once or twice (the put 0 case has already been tested).

	/** @throws Exception */
	@Test
	public void put1a() throws Exception
	{
		buf.put( 1 );
		checkBuf( 1, 1, 0 );
	}

	/** @throws Exception */
	@Test
	public void put1b() throws Exception
	{
		buf.put( 1 );
		buf.put( 2 );
		checkBuf( 2, 2, 0 );
	}

	// ------------------------------------------------------
	// cases for put( byte[] buf ):
	// buf.length = null, 0, 1, 2

	private void testPutBytes( int bufLen )
		throws Exception
	{
		buf.put( 1 );
		buf.put( bufLen >= 0 ? new byte[bufLen] : null );
		checkBuf( bufLen+1, bufLen+1, 0 );
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
		buf.put( 1 );
		buf.put( bufLen >= 0 ? new byte[bufLen] : null, offset, length );
		checkBuf( length+1, length+1, 0 );
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
		for (int i = 0; i < nPuts; i++)
			buf.put( i );
		
		buf.setIndex( 0 );
		
		for (int i = 0; i < nGets; i++)
			assertEquals( (byte) i, (byte) buf.get() );
		
		checkBuf( nPuts, nGets, nPuts-nGets );
	}
	
	private void testPutBytesGet( int nPuts, int nGets )
		throws Exception
	{
		byte[] buffer = new byte[nPuts];
		for (int i = 0; i < nPuts; i++)
			buffer[i] = (byte) i;
		
		buf.put(  buffer );
		buf.setIndex( 0 );
		
		for (int i = 0; i < nGets; i++)
			assertEquals( (byte) i, (byte) buf.get() );
		
		checkBuf( nPuts, nGets, nPuts-nGets );
	}
	
	private void testPutGetBytes( int nPuts, int nGets )
		throws Exception
	{
		for (int i = 0; i < nPuts; i++)
			buf.put( i );
		
		buf.setIndex( 0 );
		
		byte[] buffer = new byte[nGets];
		int n = buf.get( buffer );
		assertEquals( Math.min( nPuts, nGets ), n );
		
		for (int i = 0; i < n; i++)
			assertEquals( (byte) i, buffer[i] );
		
		checkBuf( nPuts, n, nPuts-n );
	}
	
	private void testPutBytesGetBytes( int nPuts, int nGets )
		throws Exception
	{
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
		
		checkBuf( nPuts, n, nPuts-n );
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
		buf = new FlexBuffer( new byte[] { 1, 2, 3 } );
		assertEquals( 1, buf.get() );
		assertEquals( 2, buf.get() );
		assertEquals( 3, buf.get() );
		checkBuf( 3, 3, 0 );
	}

	/** @throws Exception */
	@Test
	public void get5b() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2, 3 }, 2 );
		assertEquals( 1, buf.get() );
		assertEquals( 2, buf.get() );
		checkBuf( 2, 2, 0 );
	}

	/** @throws Exception */
	@Test
	public void get5c() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5 }, 1, 2 );
		assertEquals( 2, buf.get() );
		assertEquals( 3, buf.get() );
		checkBuf( 3, 3, 0 );
	}

	/** @throws Exception */
	@Test
	public void get6() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 5, 6, 7, 8 } );
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
		buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5 }, 1, 3 );
		buf.compact();
		checkBuf( 3, 0, 3 );
		assertEquals( 2, buf.get() );
		assertEquals( 3, buf.get() );
		assertEquals( 4, buf.get() );
	}

	/** @throws Exception */
	@Test
	public void getByte1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 127, 1, 0, -1, -128 } );
		assertEquals( (byte) 127, buf.getByte() );
		assertEquals( (byte) 1, buf.getByte() );
		assertEquals( (byte) 0, buf.getByte() );
		assertEquals( (byte) -1, buf.getByte() );
		assertEquals( (byte) -128, buf.getByte() );
		checkBuf( 5, 5, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getByte2() throws Exception
	{
		buf.getByte(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getShort1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2, -1, -2 } );
		assertEquals( (short) 0x0102, buf.getShort() );
		assertEquals( (short) 0xfffe, buf.getShort() );
		checkBuf( 4, 4, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getShort2() throws Exception
	{
		buf.getShort(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getInt1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, -1, -2, -3, -4 } );
		assertEquals( 0x01020304, buf.getInt() );
		assertEquals( 0xfffefdfc, buf.getInt() );
		checkBuf( 8, 8, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getInt2() throws Exception
	{
		buf.getInt(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getLong1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5, 6, 7, 8, -1, -2, -3, -4, -5, -6, -7, -8 } );
		assertEquals( 0x0102030405060708L, buf.getLong() );
		assertEquals( 0xfffefdfcfbfaf9f8L, buf.getLong() );
		checkBuf( 16, 16, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getLong2() throws Exception
	{
		buf.getLong(); // fails with EOFException
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getFloat1() throws Exception
	{
		buf = new FlexBuffer( new byte[] {  } );
		assertEquals( 123456e-10f, buf.getFloat() );
		assertEquals( 123456e10f, buf.getFloat() );
		checkBuf( 0, 0, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getFloat2() throws Exception
	{
		buf.getFloat(); // fails with EOFException
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getDouble1() throws Exception
	{
		buf = new FlexBuffer( new byte[] {  } );
		assertEquals( 123456789123456789e-110, buf.getDouble() );
		assertEquals( 123456789123456789e110, buf.getDouble() );
		checkBuf( 0, 0, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getDouble2() throws Exception
	{
		buf.getDouble(); // fails with EOFException
	}

	/** @throws Exception */
	@Test
	public void getFully1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5, 6 } );
		byte[] b = new byte[3];
		buf.getFully( b );
		assertEquals( (byte) 1, b[0] );
		assertEquals( (byte) 2, b[1] );
		assertEquals( (byte) 3, b[2] );
		buf.getFully( b );
		assertEquals( (byte) 4, b[0] );
		assertEquals( (byte) 5, b[1] );
		assertEquals( (byte) 6, b[2] );
		checkBuf( 6, 6, 0 );
	}

	/** @throws Exception */
	@Test( expected = EOFException.class )
	public void getFully2() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2 } );
		byte[] b = new byte[3];
		buf.getFully( b ); // fails with EOFException
	}
	
	// ------------------------------------------------------
	// cases for FlexBuffer put( FlexBuffer ):
	// put, put FlexBuffer with valid buffer
    // put, put Flexbuffer to a Flexbuffer approaching max length, fails with IOException.
	@Test
	public void putFlexBuffer1() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		buf = new FlexBuffer( new byte[] { } );
		buf.put( buf0 );
		checkBuf( buf0.length(), buf0.length(), 0 );
	}

	@Test ( expected = IOException.class )
	public void putFlexBuffer2() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		int max = 4*1024*1024;
		buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.put( buf0 );
		checkBuf( max+buf0.length(), max+buf0.length(), 0 );
	}
		
	// ------------------------------------------------------
	// cases for FlexBuffer put( FlexBuffer, int ):
	// put, put FlexBuffer with length larger than available bytes, fails with IllegalArgumentException
	// put, put FlexBuffer with valid length value
    // put, put Flexbuffer to a Flexbuffer approaching max length, fails with IOException.
	@Test ( expected = IllegalArgumentException.class )
	public void putFlexBuffer3() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		buf = new FlexBuffer( new byte[] { } );
		buf.setIndex(0);
		buf.put( buf0, 3);
		checkBuf( 3, 3, 0 );
	}
	
	@Test
	public void putFlexBuffer4() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		buf = new FlexBuffer( new byte[] { } );
		buf.put( buf0, 1 );
		checkBuf( 1, 1, 0 );
	}

	@Test ( expected = IOException.class )
	public void putFlexBuffer5() throws Exception
	{
		FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		int max = 4*1024*1024;
		buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.put( buf0, 1);
		checkBuf( max+1, max+1, 0 );
	}
	
	// ------------------------------------------------------
	// cases for void putByte( byte ):
	// putByte, put byte value and expect getByte to return matched value.
    // putByte to a Flexbuffer approaching max length, fails with IOException.
	@Test
	public void putByte1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { } );
		buf.putByte( (byte) 0x01 );
		checkBuf( 1, 1, 0 );
		buf.setIndex(0);
		assertEquals( (byte) 0x01, buf.getByte() );
		checkBuf( 1, 1, 0 );
	}
	
	@Test ( expected = IOException.class )
	public void putByte2() throws Exception
	{
		int max = 4*1024*1024;
		buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putByte( (byte) 0x01 );
		checkBuf( max+1, max+1, 0 );
	}
	
	// ------------------------------------------------------
	// cases for void putDouble( double ):
	// putDouble, put Double and expect getDouble to return matched value.
    // putDouble to a Flexbuffer approaching max length, fails with IOException.
	@Test
	public void putDouble1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { } );
		buf.putDouble( Double.MAX_VALUE );
		checkBuf( 8, 8, 0 );
		buf.setIndex(0);
		assertEquals( Double.MAX_VALUE, buf.getDouble() );
		checkBuf( 8, 8, 0 );
	}
	
	@Test ( expected = IOException.class )
	public void putDouble2() throws Exception
	{
		int max = 4*1024*1024;
		buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putDouble( Double.MIN_VALUE );
		checkBuf( max+8, max+8, 0 );
	}
		
	// ------------------------------------------------------
	// cases for void putFloat( float ):
	// putFloat, put Float and expect getFloat to return matched value.
    // putFloat to a Flexbuffer approaching max length, fails with IOException.
	@Test
	public void putFloat1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { } );
		buf.putFloat( Float.MAX_VALUE );
		checkBuf( 4, 4, 0 );
		buf.setIndex(0);
		assertEquals( Float.MAX_VALUE, buf.getFloat() );
		checkBuf( 4, 4, 0 );
	}
	
	@Test ( expected = IOException.class )
	public void putFloat2() throws Exception
	{
		int max = 4*1024*1024;
		buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putFloat( Float.MIN_VALUE );
		checkBuf( max+4, max+4, 0 );
	}
	
	// ------------------------------------------------------
	// cases for void putInt( int ):
	// putInt, put Int and expect getInt to return matched value.
    // putInt to a Flexbuffer approaching max length, fails with IOException.
	@Test
	public void putInt1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { } );
		buf.putInt( Integer.MAX_VALUE );
		checkBuf( 4, 4, 0 );
		buf.setIndex(0);
		assertEquals( Integer.MAX_VALUE, buf.getInt() );
		checkBuf( 4, 4, 0 );
	}
	
	@Test ( expected = IOException.class )
	public void putInt2() throws Exception
	{
		int max = 4*1024*1024;
		buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putInt( Integer.MIN_VALUE );
		checkBuf( max+4, max+4, 0 );
	}
		
	// ------------------------------------------------------
	// cases for void putLong( long ):
	// putLong, put Long and expect getLong to return matched value.
    // putShort to a Flexbuffer approaching max length, fails with IOException.
	@Test
	public void putLong1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { } );
		buf.putLong( Long.MAX_VALUE );
		checkBuf( 8, 8, 0 );
		buf.setIndex(0);
		assertEquals( Long.MAX_VALUE, buf.getLong() );
		checkBuf( 8, 8, 0 );
	}
	
	@Test ( expected = IOException.class )
	public void putLong2() throws Exception
	{
		int max = 4*1024*1024;
		buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putLong( Long.MIN_VALUE );
		checkBuf( max+8, max+8, 0 );
	}
		
	// ------------------------------------------------------
	// cases for void putShort( short ):
	// putShort, put short and expect getShort to return matched value.
    // putShort to a Flexbuffer approaching max length, fails with IOException.
	@Test
	public void putShort1() throws Exception
	{
		buf = new FlexBuffer( new byte[] { } );
		buf.putShort( Short.MAX_VALUE );
		checkBuf( 2, 2, 0 );
		buf.setIndex(0);
		assertEquals( Short.MAX_VALUE, buf.getShort() );
		checkBuf( 2, 2, 0 );
	}
	
	@Test ( expected = IOException.class )
	public void putShort2() throws Exception
	{
		int max = 4*1024*1024;
		buf = new FlexBuffer( new byte[max] );
		buf.setIndex(max);
		buf.putShort( (short) 1 );
		checkBuf( max+2, max+2, 0 );
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
		buf = new FlexBuffer( new byte[] { } );
		buf.skip(-1, false);		
	}
	
	@Test( expected = EOFException.class )
	public void skip2() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(3, false);
	}
	
	@Test
	public void skip3() throws Exception
	{
		buf = new FlexBuffer( new byte[] { } );
		buf.skip(0, false);
		checkBuf( 0, 0, 0 );
	}
	
	@Test
	public void skip4() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(2, false);
		checkBuf( 2, 2, 0 );
	}
	
	@Test
	public void skip5() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(1, false);
		checkBuf( 2, 1, 1 );
	}
	
	@Test
	public void skip6() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(1, true);
		checkBuf( 2, 1, 1 );
	}

	@Test
	public void skip7() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2 } );
		buf.skip(5, true);
		checkBuf( 5, 5, 0 );
	}
	
	@Test ( expected = IOException.class )
	public void skip8() throws Exception
	{
		buf = new FlexBuffer( new byte[] { 1, 2 } );
		
		int max = 4*1024*1024;
		buf.skip(max+1, true);
		checkBuf(max+1, max+1, 0 );
	}
}
