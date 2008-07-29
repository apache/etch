package etch.bindings.java.compiler.test;

import org.junit.Before;
import org.junit.Test;

/**
 * Simple tests to ensure that includes defined in TestIncludes.etch 
 * work properly and all included variables & messages can be accessed
 *
 * @author dattas
 */



public class TestImplIncludes
{
	
	private TestIncludesClient tc = null;
	private TestIncludesServer ts = null;
	private ImplTestIncludesServer testServer = null;
	private ImplTestIncludesClient testClient = null;
	
	/** */
	@Before public void setup() {
		testServer = new ImplTestIncludesServer(tc);
		testClient = new ImplTestIncludesClient();
		testClient._setServer( ts );
	}
	
	/** */
	@Test public void testConsts() {
		
		try
		{	
			testServer.testConst2();
			testServer.testConst3();
			Integer x = new Integer(5);
			Integer y = new Integer(50);
			testClient.testConst1( x, y );
			testClient.testConst2();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
		}		
	}
	
	/** */
	@Test public void testEnums() {
		try
		{
			testServer.testEnum2();
			testClient.testEnum1();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
		}	
	}
	
}
