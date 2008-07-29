import org.omg.CORBA.ORB;
import org.omg.CosNaming.NameComponent;
import org.omg.CosNaming.NamingContextExt;
import org.omg.CosNaming.NamingContextExtHelper;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAHelper;

import HelloApp.Hello;
import HelloApp.HelloHelper;
import HelloApp.HelloPOA;

@SuppressWarnings("all")
class HelloImpl extends HelloPOA
{
	private ORB orb;

	public void setORB( ORB orb_val )
	{
		orb = orb_val;
	}

	public int add( int x, int y )
	{
		return x + y;
	}

	public int sum( int[] values )
	{
		int sum = 0;
		for (int v: values)
			sum += v;
		return sum;
	}
	
	public void report( int code, String msg )
	{
		// ignore.
	}

	public void shutdown()
	{
		orb.shutdown( false );
	}
}

@SuppressWarnings("all")
public class HelloServer
{
	public static void main( String args[] ) throws Exception
	{
		// create and initialize the ORB
		ORB orb = ORB.init( args, null );

		// get reference to rootpoa & activate the POAManager
		POA rootpoa =
			POAHelper.narrow( orb.resolve_initial_references( "RootPOA" ) );
		rootpoa.the_POAManager().activate();

		// create servant and register it with the ORB
		HelloImpl helloImpl = new HelloImpl();
		helloImpl.setORB( orb );

		// get object reference from the servant
		org.omg.CORBA.Object ref = rootpoa.servant_to_reference( helloImpl );
		Hello href = HelloHelper.narrow( ref );

		// get the root naming context
		org.omg.CORBA.Object objRef =
			orb.resolve_initial_references( "NameService" );
		// Use NamingContextExt which is part of the Interoperable
		// Naming Service (INS) specification.
		NamingContextExt ncRef = NamingContextExtHelper.narrow( objRef );

		// bind the Object Reference in Naming
		String name = "Hello";
		NameComponent path[] = ncRef.to_name( name );
		ncRef.rebind( path, href );

		System.out.println( "HelloServer ready and waiting ..." );

		// wait for invocations from clients
		orb.run();
	}
}
