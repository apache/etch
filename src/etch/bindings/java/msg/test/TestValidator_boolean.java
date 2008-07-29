/* TestValidator_boolean.java
 *
 * Created by jliau on Oct 8, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import etch.bindings.java.msg.Validator_boolean;
import etch.bindings.java.transport.fmt.TypeCode;

public class TestValidator_boolean
{
	@BeforeClass
	public static void setUpBeforeClass() throws Exception
	{
		// noop for one-time set up
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception
	{
		// noop for one-time tear down
	}

	@Before
	public void setUp() throws Exception
	{
		// noop before each tast case
	}

	@After
	public void tearDown() throws Exception
	{
		// noop after each test case
	}

	@Test
	public void testElementValidator()
	{
		Validator_boolean vb = Validator_boolean.get( 0 );
		// the scalar boolean validator should not be null
		assertNotNull( vb );
		// the element validator should be null becaus vb is scalar
		assertNull( vb.elementValidator() );
		
		vb = Validator_boolean.get( 1 );
		// the one-dimentional boolean validator should not be null
		assertNotNull( vb );
		Validator_boolean vb1 = (Validator_boolean) vb.elementValidator();
		// the element validator should be not null becaus vb is one-dimentional
		assertNotNull( vb1 );
		// the element validator should be null becaus vb1 is scalar
		assertNull( vb1.elementValidator() );
	}

	@Test
	public void testCheckValue()
	{
		// test scalar validator
		Validator_boolean vb = Validator_boolean.get( 0 );		
		// the boolean validator should not be null
		assertNotNull( vb );			
		// true should return TypeCode.BOOLEAN_TRUE
		assertEquals( TypeCode.BOOLEAN_TRUE, vb.checkValue( true ) );
		// true should return TypeCode.BOOLEAN_FALSE
		assertEquals( TypeCode.BOOLEAN_FALSE, vb.checkValue( false ) );
		// non-boolean object value should return null
		assertNull( vb.checkValue( this ) );		
		assertNull( vb.checkValue( 1 ) );
		assertNull( vb.checkValue( 0 ) );	
		assertNull( vb.checkValue( 0.001 ) );	
		assertNull( vb.checkValue( "string" ) );
		
		// test one-dimentional boolean array validator
		vb = Validator_boolean.get( 1 );
		// the one-dimentional boolean validator should not be null
		assertNotNull( vb );
		boolean[] ba1 = new boolean[1];
		// since vb Dim = 1, it should return TypeCode.ARRAY for ba1
		assertEquals( TypeCode.ARRAY, vb.checkValue( ba1 ) );
		boolean[] ba2 = new boolean[2];
		// since vb Dim = 1, it should return TypeCode.ARRAY for ba2
		assertEquals( TypeCode.ARRAY, vb.checkValue( ba2 ) );
		boolean[][] ba11 = new boolean[1][1];
		// since vb Dim = 1, ba11 is two-dimentional, so checkValue should return null 
		assertNull( vb.checkValue( ba11 ) );		
		
		// test two-dimentional boolean array validator
		vb = Validator_boolean.get( 2 );
		// the two-dimentional boolean validator should not be null
		assertNotNull( vb );		
		// since vb Dim = 2, ba1 is one-dimentional, so checkValue should return null 
		assertNull( vb.checkValue( ba1 ) );				
		// since vb Dim = 2, ba11 is two-dimentional, so checkValue should return TypeCode.ARRAY 
		assertEquals( TypeCode.ARRAY, vb.checkValue( ba11 ) );		
		// since vb Dim = 2, ba234 is three-dimentional, so checkValue should return null 
		boolean[][][] ba234 = new boolean[2][3][4];
		assertNull( vb.checkValue( ba234 ) );				
	}

	@Test
	public void testGet()
	{
		// negative test with invlid input
		Validator_boolean vb = Validator_boolean.get( -1 );
		assertNull( vb );
		
		// positive tests with different dimentions
		vb = Validator_boolean.get( 0 );
		assertNotNull( vb );
				
		vb = Validator_boolean.get( 4 );
		assertNotNull( vb );
		
		vb = Validator_boolean.get( 10 );
		assertNotNull( vb );
	}
}
