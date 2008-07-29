/* $Id$
 *
 * Created by wert on Mar 14, 2007.
 *
 * Copyright (c) 2007 Cisco Systems. All rights reserved.
 */

package metreos.util.test;

import metreos.util.CircularQueue;
import metreos.util.Timer;

@SuppressWarnings("all")
public class testCircularQueue
{
	/**
	 * @param args
	 */
	public static void main( String[] args )
		throws Exception
	{
		log( "new CircularQueue" );
		CircularQueue<Integer> cq = new CircularQueue<Integer>( 2 );
		check( cq.size() == 2 );
		check( cq.count() == 0 );
		check( cq.isEmpty() );
		check( !cq.isFull() );
		check( cq.get( -1 ) == null );
		
		log( "cq.put( 1 )" );
		check( cq.put( 1 ) );
		check( cq.size() == 2 );
		check( cq.count() == 1 );
		check( !cq.isEmpty() );
		check( !cq.isFull() );

		log( "cq.get() == 1" );
		check( cq.get() == 1 );
		check( cq.size() == 2 );
		check( cq.count() == 0 );
		check( cq.isEmpty() );
		check( !cq.isFull() );
		check( cq.get( -1 ) == null );

		log( "cq.put( 2 )" );
		check( cq.put( 2 ) );
		check( cq.size() == 2 );
		check( cq.count() == 1 );
		check( !cq.isEmpty() );
		check( !cq.isFull() );

		log( "cq.put( 3 )" );
		check( cq.put( 3 ) );
		check( cq.size() == 2 );
		check( cq.count() == 2 );
		check( !cq.isEmpty() );
		check( cq.isFull() );
		
		long t0, t;

		log( "!cq.put( 4, -1 )" );
		t0 = Timer.getNanos();
		check( !cq.put( 4, -1 ) );
		t = Timer.getMillisSince( t0 );
		check( t == 0, "time %d > 0 millis", t );
		check( cq.size() == 2 );
		check( cq.count() == 2 );
		check( !cq.isEmpty() );
		check( cq.isFull() );

		log( "!cq.put( 4, 50 )" );
		t0 = Timer.getNanos();
		check( !cq.put( 4, 50 ) );
		t = Timer.getMillisSince( t0 );
		check( t >= 50, "time %d < 50 millis", t );
		check( cq.size() == 2 );
		check( cq.count() == 2 );
		check( !cq.isEmpty() );
		check( cq.isFull() );

		log( "cq.get() == 2" );
		check( cq.get() == 2 );
		check( cq.size() == 2 );
		check( cq.count() == 1 );
		check( !cq.isEmpty() );
		check( !cq.isFull() );

		log( "cq.get() == 3" );
		check( cq.get() == 3 );
		check( cq.size() == 2 );
		check( cq.count() == 0 );
		check( cq.isEmpty() );
		check( !cq.isFull() );
		
		log( "cq.get( -1 )" );
		t0 = Timer.getNanos();
		check( cq.get( -1 ) == null );
		t = Timer.getMillisSince( t0 );
		check( t == 0, "time %d == 0 millis", t );
		
		log( "cq.get( 50 )" );
		t0 = Timer.getNanos();
		check( cq.get( 50 ) == null );
		t = Timer.getMillisSince( t0 );
		check( t >= 50, "time %d >= 50 millis", t );
		
		log( "cq.put( 4 )" );
		check( cq.put( 4 ) );
		
		log( "cq.put( 5 )" );
		check( cq.put( 5 ) );
		
		log( "cq.get() == 4" );
		check( cq.get() == 4 );
		
		log( "cq.put( 6 )" );
		check( cq.put( 6 ) );
		
		log( "cq.get() == 5" );
		check( cq.get() == 5 );
		
		log( "cq.put( 7 )" );
		check( cq.put( 7 ) );
		
		log( "cq.get() == 6" );
		check( cq.get() == 6 );
		
		log( "cq.put( 8 )" );
		check( cq.put( 8 ) );
		
		log( "cq.get() == 7" );
		check( cq.get() == 7 );
		
		log( "cq.close()" );
		cq.close();
		
		log( "cq.get() == 8" );
		check( cq.get() == 8 );
		
		log( "cq.get() == null" );
		check( cq.get() == null );
		
		log( "done" );
	}
	
	public static void log( String msg )
	{
		System.out.println( msg );
	}
	
	public static void check( boolean ok )
	{
		if (!ok)
			throw new RuntimeException( "operation failed" );
	}
	
	public static void check( boolean ok, String fmt, Object ... params )
	{
		if (!ok)
			throw new RuntimeException( String.format( fmt, params ) );
	}
}
