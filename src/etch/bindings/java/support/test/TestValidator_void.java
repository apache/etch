
package etch.bindings.java.support.test;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

import etch.bindings.java.msg.Validator;
import etch.bindings.java.support.Validator_void;

/**
 * Test Validator_void.
 */
public class TestValidator_void
{
    /** */
	@Test
	public void test()
	{
		test( 0, "void[0]", null, (int) 1 );
		test( 0, "void[0]", null, (float) 2 );
		test( 0, "void[0]", null, (double) 3 );
		test( 0, "void[0]", null, (byte) 4 );
		test( 0, "void[0]", null, (long) 5 );
		test( 0, "void[0]", null, (short) 6 );
		
	}
	
	private void test( int n, String s, Object good, Object bad )
	{
		Validator_void v = Validator_void.get( n );
		assertTrue( v.validate( good ) );
		assertFalse( v.validate( bad ) );
	}

    /** */
	@Test( expected = IllegalArgumentException.class )
	public void testGetNeg1()
	{		
		Validator_void.get( -1 );
	}	

    /** */
	@Test
	public void testGetNeg2()
	{		
		assertNull( Validator_void.get( 0 ).elementValidator() );
	}		

    /** */
	@Test( expected = IllegalArgumentException.class )
	public void testGetMaxPlusOne()
	{		
		Validator_void.get( Validator.MAX_NDIMS+1 );			
	}
}
