/* TestValidator_int.java
 *
 * Created by wayetman on Oct 11, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertNull;

import org.junit.Test;

import etch.bindings.java.msg.Validator_int;
import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Test Validator_int.
 */
public class TestValidator_int
{
    /** */
	@Test
	public void test2string() throws Exception
	{
		Validator_int vInt = Validator_int.get( 0 );
		assertEquals( "int[0]", vInt.toString() );
	}
	
    /** */
	@Test
	public void elementValidatorGet0()
	{
		Validator_int vInt = Validator_int.get( 0 );
		// the scalar int validator should not be null
		assertNotNull( vInt );
	}	

    /** */
	@Test
	public void elementValidatorGetNeg1()
	{		
		Validator_int vInt1 = Validator_int.get( -1 );
		// the scalar int validator should be null
		assertNull( vInt1 );				
	}	

    /** */
	@Test
	public void elementValidatorGetPos1()
	{	
		Validator_int vInt2 = Validator_int.get( 1 );
		// the scalar int validator should not be null
		assertNotNull( vInt2);		
	}

    /** */
	@Test
	public void elementValidatorGetObj()
	{
		Validator_int vInt = Validator_int.get( 0 );
		// the element validator should be null because vInt is scalar
		assertNull( vInt.elementValidator() );
	}

    /** */
	@Test
	public void checkValueInts()
	{ 
		// test scalar validator
		Validator_int vInt = Validator_int.get( 0 );		
		// the boolean validator should not be null
		assertNotNull( vInt );
		
		// true should return TypeCode.TypeCode.INT1 -128 -> 127
		assertEquals( TypeCode.INT1, vInt.checkValue( 127 ) );
		// true should return TypeCode.TypeCode.INT1 -128 -> 127
		assertEquals( TypeCode.INT1, vInt.checkValue( -128 ) );
        // true should return TypeCode.TypeCode.INT2 -32768 -> 32767
		assertNotSame( TypeCode.INT1, vInt.checkValue( 128 ) );
		
        // true should return TypeCode.TypeCode.INT2 -32768 -> 32767
		assertEquals( TypeCode.INT2, vInt.checkValue( -32768 ) );	
		assertEquals( TypeCode.INT2, vInt.checkValue( 32767 ) );

        // true should return TypeCode.TypeCode.INT4
		assertEquals( TypeCode.INT4, vInt.checkValue( 32768 ) );	
	}

    /** */
	@Test	
	public void checkValueStr()
	{	
		Validator_int vInt = Validator_int.get( 0 );
		//Test no integers and return null
		assertNull( vInt.checkValue( "string" ) );
	}	

    /** */
	@Test	
	public void checkValueAsuite()
	{	
		Validator_int vInt = Validator_int.get( 0 );
		assertNull( vInt.checkValue( this ) );			
		assertNull( vInt.checkValue( 0.001 ) );
		// test one-dimentional integer array validator
		vInt = Validator_int.get( 1 );
		// the one-dimentional integer validator should not be null
		assertNotNull( vInt );
		
		int[] myIints = new int[1];
		// since vInt Dim = 1, it should return TypeCode.ARRAY for myIints
		assertEquals( TypeCode.ARRAY, vInt.checkValue( myIints ) );

		int[][] myIints2 = new int[1][1];
		// since vInt Dim = 1, myIints2 is two-dimentional, so checkValue should return null 
		assertNull( vInt.checkValue( myIints2 ) );		
		
		// test two-dimentional integer array validator
		vInt = Validator_int.get( 2 );
		// the two-dimentional integer validator should not be null
		assertNotNull( vInt );		
		// since vInt Dim = 2, myIints is one-dimentional, so checkValue should return null 
		assertNull( vInt.checkValue( myIints ) );				
		// since vInt Dim = 2, myIints2 is two-dimentional, so checkValue should return TypeCode.ARRAY 
		assertEquals( TypeCode.ARRAY, vInt.checkValue( myIints2 ) );		
		// since vInt Dim = 2, intMulti is three-dimentional, so checkValue should return null 
		int[][][] intMulti = new int[2][3][4];
		assertNull( vInt.checkValue( intMulti ) );				
	}

    /** */
	@Test
	public void testGet()
	{
		// negative test with invalid input
		Validator_int vInt = Validator_int.get( -1 );
		assertNull( vInt );
		
		// positive tests with different dimentions
		vInt = Validator_int.get( 0 );
		assertNotNull( vInt );
				
		vInt = Validator_int.get( 4 );
		assertNotNull( vInt );
		
		vInt = Validator_int.get( 10 );
		assertNotNull( vInt );
	}
}
