package etch.bindings.java.support.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.fail;

import org.junit.Before;
import org.junit.Test;

import etch.bindings.java.msg.IdName;
import etch.bindings.java.support.IdNameMap;

public class TestIdNameMap
{
	@Before
	public void init()
	{
		map = new IdNameMap<IdName>( 3 )
		{
			@Override
			public IdName makeNew( Integer id, String name )
			{
				return new IdName( id, name );
			}
		};
		
		a = map.add( new IdName( 1, "a" ) );
		b = map.add( new IdName( 2, "b" ) );
	}
	
	private IdNameMap<IdName> map;
	
	private IdName a;
	
	private IdName b;
	
	@Test
	public void get_1() throws Exception
	{
		IdName x = map.get( 1 );
		assertNotNull( x );
		assertSame( a, x );
		assertEquals( 1, x.getId() );
		assertEquals( "a", x.getName() );
	}
	
	@Test
	public void get_a() throws Exception
	{
		IdName x = map.get( "a" );
		assertNotNull( x );
		assertSame( a, x );
		assertEquals( 1, x.getId() );
		assertEquals( "a", x.getName() );
	}
	
	@Test
	public void get_2() throws Exception
	{
		IdName x = map.get( 2 );
		assertNotNull( x );
		assertSame( b, x );
		assertEquals( 2, x.getId() );
		assertEquals( "b", x.getName() );
	}
	
	@Test
	public void get_b() throws Exception
	{
		IdName x = map.get( "b" );
		assertNotNull( x );
		assertSame( b, x );
		assertEquals( 2, x.getId() );
		assertEquals( "b", x.getName() );
	}
	
	@Test
	public void get_3() throws Exception
	{
		IdName x = map.get( 3 );
		assertNotNull( x );
		assertNotSame( a, x );
		assertNotSame( b, x );
		assertEquals( 3, x.getId() );
		assertEquals( "3", x.getName() );
		
		IdName y = map.get( 3 );
		assertSame( x, y );
	}
	
	@Test
	public void get_c() throws Exception
	{
		IdName x = map.get( "c" );
		assertNotNull( x );
		assertNotSame( a, x );
		assertNotSame( b, x );
		assertEquals( 352988318, x.getId() );
		assertEquals( "c", x.getName() );
		
		IdName y = map.get( "c" );
		assertSame( x, y );
	}
	
	@Test
	public void get_just_enough() throws Exception
	{
		IdName c = map.get( "c" );
		assertNotNull( c );
		assertNotSame( a, c );
		assertNotSame( b, c );
		
		IdName d = map.get( "d" );
		assertNotNull( d );
		assertNotSame( a, d );
		assertNotSame( b, d );
		assertNotSame( c, d );
		
		IdName e = map.get( "e" );
		assertNotNull( e );
		assertNotSame( a, e );
		assertNotSame( b, e );
		assertNotSame( c, e );
		assertNotSame( d, e );
	}
	
	@Test( expected=IllegalArgumentException.class )
	public void get_too_many1() throws Exception
	{
		IdName c = map.get( "c" );
		assertNotNull( c );
		assertNotSame( a, c );
		assertNotSame( b, c );
		
		IdName d = map.get( "d" );
		assertNotNull( d );
		assertNotSame( a, d );
		assertNotSame( b, d );
		assertNotSame( c, d );
		
		IdName e = map.get( "e" );
		assertNotNull( e );
		assertNotSame( a, e );
		assertNotSame( b, e );
		assertNotSame( c, e );
		assertNotSame( d, e );
		
		map.get( "f" );
		fail( "maxAutoCount should have been exceeded" );
	}
	
	@Test( expected=IllegalArgumentException.class )
	public void get_too_many2() throws Exception
	{
		IdName c = map.get( 3 );
		assertNotNull( c );
		assertNotSame( a, c );
		assertNotSame( b, c );
		
		IdName d = map.get( 4 );
		assertNotNull( d );
		assertNotSame( a, d );
		assertNotSame( b, d );
		assertNotSame( c, d );
		
		IdName e = map.get( 5 );
		assertNotNull( e );
		assertNotSame( a, e );
		assertNotSame( b, e );
		assertNotSame( c, e );
		assertNotSame( d, e );
		
		map.get( 6 );
		fail( "maxAutoCount should have been exceeded" );
	}
	
	@Test( expected=IllegalArgumentException.class )
	public void get_too_many3() throws Exception
	{
		IdName c = map.get( 3 );
		assertNotNull( c );
		assertNotSame( a, c );
		assertNotSame( b, c );
		
		IdName d = map.get( "d" );
		assertNotNull( d );
		assertNotSame( a, d );
		assertNotSame( b, d );
		assertNotSame( c, d );
		
		IdName e = map.get( 5 );
		assertNotNull( e );
		assertNotSame( a, e );
		assertNotSame( b, e );
		assertNotSame( c, e );
		assertNotSame( d, e );
		
		map.get( "f" );
		fail( "maxAutoCount should have been exceeded" );
	}
	
	@Test
	public void add() throws Exception
	{
		assertSame( a, map.add( a ) );
		assertSame( b, map.add( b ) );
	}
	
	@Test( expected=IllegalArgumentException.class )
	public void add_id_coll() throws Exception
	{
		map.add( new IdName( 1, "c" ) );
	}
	
	@Test( expected=IllegalArgumentException.class )
	public void add_name_coll() throws Exception
	{
		map.add( new IdName( 3, "a" ) );
	}
	
	@Test( expected=IllegalArgumentException.class )
	public void add_id_name_coll() throws Exception
	{
		map.add( new IdName( 2, "a" ) );
	}
}
