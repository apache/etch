import metreos.util.Assertion;
import metreos.util.Log;
import metreos.util.PerfTest;

import org.omg.CORBA.ORB;
import org.omg.CosNaming.NameComponent;
import org.omg.CosNaming.NamingContext;
import org.omg.CosNaming.NamingContextHelper;

import HelloApp.Hello;
import HelloApp.HelloHelper;

@SuppressWarnings("all")
public class HelloClient
{
	public static void main( String args[] ) throws Exception
	{
		Log.addSink( null );

		// create and initialize the ORB
		ORB orb = ORB.init( args, null );

		// get the root naming context
		org.omg.CORBA.Object objRef =
			orb.resolve_initial_references( "NameService" );
		NamingContext ncRef = NamingContextHelper.narrow( objRef );

		// resolve the Object Reference in Naming
		NameComponent nc = new NameComponent( "Hello", "" );
		NameComponent path[] = { nc };
		final Hello server = HelloHelper.narrow( ncRef.resolve( path ) );
		
		Assertion.check( server.add( 2, 3 ) == 5, "server.add( 2, 3 ) == 5" );

		Assertion.check( server.add( 3, 4 ) == 7, "server.add( 3, 4 ) == 7" );
		
		Assertion.check( server.sum( new int[] { 1, 2, 3 } ) == 6,
			"server.sum( new int[] { 1, 2, 3 } ) == 6" );
	
		Assertion.check( server.sum( new int[] { 2, 3, 4 } ) == 9,
			"server.sum( new int[] { 2, 3, 4 } ) == 9" );
		
		server.report( 23, "all ok" );
		
		server.report( 19, "stopping" );
		
		new PerfTest( "add", 10, 5 )
		{
			@Override
			public void run( long n )
			{
				while (n-- > 0)
					server.add( 1000000000, 2000000000 );
			}
		}.run();
	
		final int[] values = new int[1000];
		for (int i = 0; i < values.length; i++)
			values[i] = i;
		
		new PerfTest( "sum", 10, 5 )
		{
			@Override
			public void run( long n )
			{
				while (n-- > 0)
					server.sum( values );
			}
		}.run();
	
		new PerfTest( "report", 10, 5 )
		{
			@Override
			public void run( long n )
			{
				while (n-- > 0)
					server.report( 23,
						// 50 characters:
						"this message describes the specifics of a code 23." );
			}
		}.run();

		
		System.out.println( "done" );
	}
}
