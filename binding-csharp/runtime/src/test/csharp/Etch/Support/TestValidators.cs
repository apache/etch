// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    [TestFixture]
    public class TestValidators
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestValidator" );
        }

        [Test]
	    public void Test_boolean()
	    {
		    Test( Validator_boolean.Get( 2 ), 2,
			    new Boolean[][] { new Boolean[] { false, false }, new Boolean[] { false, false } } );
    		
		    Test( Validator_boolean.Get( 2 ), 2,
			    new Boolean[][] { new Boolean[] { true, true }, new Boolean[] { true, true } } );
	    }

	    
	    [Test]
	    public void Test_SByte()
	    {
		    Test( Validator_byte.Get( 2 ), 2,
			 
			    new SByte[][] { new SByte[] { 0, 0 }, new SByte[] { 0, 0 } } );

		    Test( Validator_byte.Get( 2 ), 2,
              
			    new SByte[][] { new SByte[] { 1, 1 }, new SByte[] { 1, 1 } } );

		    Test( Validator_byte.Get( 2 ), 2,
              
			    new SByte[][] { new SByte[] { -1, -1 }, new SByte[] { -1, -1 } } );
    		
		    Test( Validator_byte.Get( 2 ), 2,
               
			    new SByte[][] { new SByte[] { SByte.MaxValue, SByte.MinValue }, new SByte[] { SByte.MaxValue, SByte.MinValue } } );
    		
		    Test( Validator_byte.Get( 2 ), 2,
              
			    new SByte[][] { new SByte[] { SByte.MinValue, SByte.MinValue }, new SByte[] { SByte.MinValue, SByte.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_short()
	    {
		    Test( Validator_short.Get( 2 ), 2,
			    
			    new short[][] { new short[] { 0, 0 }, new short[] { 0, 0 } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			  
			    new short[][] { new short[] { 1, 1 }, new short[] { 1, 1 } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			   
			    new short[][] { new short[] { -1, -1 }, new short[] { -1, -1 } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			  
			    new short[][] { new short[] { SByte.MaxValue, SByte.MaxValue }, new short[] { SByte.MaxValue, SByte.MaxValue } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			  
			    new short[][] { new short[] { SByte.MinValue, SByte.MinValue }, new short[] { SByte.MinValue, SByte.MinValue } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			   
                new short[][] { new short[] { short.MaxValue, short.MaxValue }, new short[] { short.MaxValue, short.MaxValue } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			    
                new short[][] { new short[] { short.MinValue, short.MinValue }, new short[] { short.MinValue, short.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_int()
	    {
		    Test( Validator_int.Get( 2 ), 2,
			    
			    new int[][] { new int[] { 0, 0 }, new int[] { 0, 0 } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			   
			    new int[][] { new int[] { 1, 1 }, new int[] { 1, 1 } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			   
			    new int[][] { new int[] { -1, -1 }, new int[] { -1, -1 } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    
			    new int[][] { new int[] { SByte.MaxValue, SByte.MaxValue }, new int[] { SByte.MaxValue, SByte.MaxValue } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    
			    new int[][] { new int[] { SByte.MinValue, SByte.MinValue }, new int[] { SByte.MinValue, SByte.MinValue } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    
			    new int[][] { new int[] { short.MaxValue, short.MaxValue }, new int[] { short.MaxValue, short.MaxValue } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			   
			    new int[][] { new int[] { short.MinValue, short.MinValue }, new int[] { short.MinValue, short.MinValue } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			  
			    new int[][] { new int[] { int.MaxValue, int.MaxValue }, new int[] { int.MaxValue, int.MaxValue} } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			   
			    new int[][] { new int[] { int.MinValue, int.MinValue }, new int[] { int.MinValue, int.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_long()
	    {
		    Test( Validator_long.Get( 2 ), 2,
			   
			    new long[][] { new long[] { 0, 0 }, new long[] { 0, 0 } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			   
			    new long[][] { new long[] { 1, 1 }, new long[] { 1, 1 } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			   
			    new long[][] { new long[] { -1, -1 }, new long[] { -1, -1 } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			   
			    new long[][] { new long[] { SByte.MaxValue, SByte.MaxValue }, new long[] { SByte.MaxValue, SByte.MaxValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    
			    new long[][] { new long[] { SByte.MinValue, SByte.MinValue }, new long[] { SByte.MinValue, SByte.MinValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			  
			    new long[][] { new long[] { short.MaxValue, short.MaxValue }, new long[] { short.MaxValue, short.MaxValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			  
			    new long[][] { new long[] { short.MinValue, short.MinValue }, new long[] { short.MinValue, short.MinValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			
			    new long[][] { new long[] { int.MaxValue, int.MaxValue }, new long[] { int.MaxValue, int.MaxValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			  
			    new long[][] { new long[] { int.MinValue, int.MinValue }, new long[] { int.MinValue, int.MinValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			 
			    new long[][] { new long[] { long.MaxValue, long.MaxValue }, new long[] { long.MaxValue, long.MaxValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
		
			    new long[][] { new long[] { long.MinValue, long.MinValue }, new long[] { long.MinValue, long.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_float()
	    {
		    Test( Validator_float.Get( 2 ), 2,
			   
			    new float[][] { new float[] { 0, 0 }, new float[] { 0, 0 } } );
    		
		    Test( Validator_float.Get( 2 ), 2,
			   
			    new float[][] { new float[] { 1, 1 }, new float[] { 1, 1 } } );
    		
		    Test( Validator_float.Get( 2 ), 2,
			  
			    new float[][] { new float[] { -1, -1 }, new float[] { -1, -1 } } );
    		
		    Test( Validator_float.Get( 2 ), 2,
			 
			    new float[][] { new float[] { float.MaxValue, float.MaxValue }, new float[] { float.MaxValue, float.MaxValue } } );
    		
		    Test( Validator_float.Get( 2 ), 2,
			 
			    new float[][] { new float[] { float.MinValue, float.MinValue }, new float[] { float.MinValue, float.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_double()
	    {
		    Test( Validator_double.Get( 2 ), 2,
			   
			    new double[][] { new double[] { 0, 0 }, new double[] { 0, 0 } } );
    		
		    Test( Validator_double.Get( 2 ), 2,
			   
			    new double[][] { new double[] { 1, 1 }, new double[] { 1, 1 } } );
    		
		    Test( Validator_double.Get( 2 ), 2,
			   
			    new double[][] { new double[] { -1, -1 }, new double[] { -1, -1 } } );
    		
		    Test( Validator_double.Get( 2 ), 2,
			   
			    new double[][] { new double[] { Double.MaxValue, Double.MaxValue }, new double[] { Double.MaxValue, Double.MaxValue } } );
    		
		    Test( Validator_double.Get( 2 ), 2,
			   
			    new double[][] { new double[] { Double.MinValue, Double.MinValue }, new double[] { Double.MinValue, Double.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_string()
	    {
		    Test( Validator_string.Get( 2 ), 2,
			  
			    new String[][] { new String[] { "", "" }, new String[] { "", "" } } );
    		
		    Test( Validator_string.Get( 2 ), 2,
			  
			    new String[][] { new String[] { "abc", "abc" }, new String[] { "abc", "abc" } } );
	    }

	    
	    [Test]
	    public void Test_custom()
	    {
            Test( Validator_custom.Get( typeof( DateTime ), 2 ,false), 2,
               
                new DateTime[][] { new DateTime[] { new DateTime(), new DateTime() }, new DateTime[] { new DateTime(), new DateTime() } } );
	    }
    	
	    private void Test( Validator v, int n, Object o )
	    {
    //		System.out.printf( "Testing with %s\n", v );
    //		Assert.assertEquals( n, v.GetDim() );
    		
		    Assert.IsTrue( v.Validate( o ) );
		  
    		
		    if (n > 0)
		    {


                int k = ( ( Array ) o ).Length;
                for ( int i = 0; i < k; i++ )
                    Test( v.ElementValidator(), n-1, ( ( Array ) o ).GetValue( i ) );
		    }
	    }
    }
}
