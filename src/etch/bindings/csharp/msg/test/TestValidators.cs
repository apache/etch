using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

namespace Etch.Msg.Test
{
    [TestFixture]
    public class TestValidators
    {
        [Test]
	    public void Test_boolean()
	    {
		    Test( Validator_boolean.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.BOOLEAN_FALSE, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new Boolean[][] { new Boolean[] { false, false }, new Boolean[] { false, false } } );
    		
		    Test( Validator_boolean.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new Boolean[][] { new Boolean[] { true, true }, new Boolean[] { true, true } } );
	    }

	    
	    [Test]
	    public void Test_SByte()
	    {
		    Test( Validator_byte.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.BYTE1, Etch.Transport.Fmt.TypeCode.BYTES, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new SByte[][] { new SByte[] { 0, 0 }, new SByte[] { 0, 0 } } );

		    Test( Validator_byte.Get( 2 ), 2,
                new SByte[] { Etch.Transport.Fmt.TypeCode.BYTE1, Etch.Transport.Fmt.TypeCode.BYTES, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new SByte[][] { new SByte[] { 1, 1 }, new SByte[] { 1, 1 } } );

		    Test( Validator_byte.Get( 2 ), 2,
                new SByte[] { Etch.Transport.Fmt.TypeCode.BYTE1, Etch.Transport.Fmt.TypeCode.BYTES, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new SByte[][] { new SByte[] { -1, -1 }, new SByte[] { -1, -1 } } );
    		
		    Test( Validator_byte.Get( 2 ), 2,
                new SByte[] { Etch.Transport.Fmt.TypeCode.BYTE1, Etch.Transport.Fmt.TypeCode.BYTES, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new SByte[][] { new SByte[] { SByte.MaxValue, SByte.MinValue }, new SByte[] { SByte.MaxValue, SByte.MinValue } } );
    		
		    Test( Validator_byte.Get( 2 ), 2,
                new SByte[] { Etch.Transport.Fmt.TypeCode.BYTE1, Etch.Transport.Fmt.TypeCode.BYTES, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new SByte[][] { new SByte[] { SByte.MinValue, SByte.MinValue }, new SByte[] { SByte.MinValue, SByte.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_short()
	    {
		    Test( Validator_short.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.SHORT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new short[][] { new short[] { 0, 0 }, new short[] { 0, 0 } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.SHORT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new short[][] { new short[] { 1, 1 }, new short[] { 1, 1 } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.SHORT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new short[][] { new short[] { -1, -1 }, new short[] { -1, -1 } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.SHORT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new short[][] { new short[] { SByte.MaxValue, SByte.MaxValue }, new short[] { SByte.MaxValue, SByte.MaxValue } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.SHORT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new short[][] { new short[] { SByte.MinValue, SByte.MinValue }, new short[] { SByte.MinValue, SByte.MinValue } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.SHORT2, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
                new short[][] { new short[] { short.MaxValue, short.MaxValue }, new short[] { short.MaxValue, short.MaxValue } } );
    		
		    Test( Validator_short.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.SHORT2, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
                new short[][] { new short[] { short.MinValue, short.MinValue }, new short[] { short.MinValue, short.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_int()
	    {
		    Test( Validator_int.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.INT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new int[][] { new int[] { 0, 0 }, new int[] { 0, 0 } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.INT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new int[][] { new int[] { 1, 1 }, new int[] { 1, 1 } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.INT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new int[][] { new int[] { -1, -1 }, new int[] { -1, -1 } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.INT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new int[][] { new int[] { SByte.MaxValue, SByte.MaxValue }, new int[] { SByte.MaxValue, SByte.MaxValue } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.INT1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new int[][] { new int[] { SByte.MinValue, SByte.MinValue }, new int[] { SByte.MinValue, SByte.MinValue } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.INT2, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new int[][] { new int[] { short.MaxValue, short.MaxValue }, new int[] { short.MaxValue, short.MaxValue } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.INT2, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new int[][] { new int[] { short.MinValue, short.MinValue }, new int[] { short.MinValue, short.MinValue } } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.INT4, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new int[][] { new int[] { int.MaxValue, int.MaxValue }, new int[] { int.MaxValue, int.MaxValue} } );
    		
		    Test( Validator_int.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.INT4, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new int[][] { new int[] { int.MinValue, int.MinValue }, new int[] { int.MinValue, int.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_long()
	    {
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { 0, 0 }, new long[] { 0, 0 } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { 1, 1 }, new long[] { 1, 1 } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { -1, -1 }, new long[] { -1, -1 } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { SByte.MaxValue, SByte.MaxValue }, new long[] { SByte.MaxValue, SByte.MaxValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG1, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { SByte.MinValue, SByte.MinValue }, new long[] { SByte.MinValue, SByte.MinValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG2, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { short.MaxValue, short.MaxValue }, new long[] { short.MaxValue, short.MaxValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG2, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { short.MinValue, short.MinValue }, new long[] { short.MinValue, short.MinValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG4, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { int.MaxValue, int.MaxValue }, new long[] { int.MaxValue, int.MaxValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG4, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { int.MinValue, int.MinValue }, new long[] { int.MinValue, int.MinValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG8, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { long.MaxValue, long.MaxValue }, new long[] { long.MaxValue, long.MaxValue } } );
    		
		    Test( Validator_long.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.LONG8, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new long[][] { new long[] { long.MinValue, long.MinValue }, new long[] { long.MinValue, long.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_float()
	    {
		    Test( Validator_float.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT4, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new float[][] { new float[] { 0, 0 }, new float[] { 0, 0 } } );
    		
		    Test( Validator_float.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT4, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new float[][] { new float[] { 1, 1 }, new float[] { 1, 1 } } );
    		
		    Test( Validator_float.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT4, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new float[][] { new float[] { -1, -1 }, new float[] { -1, -1 } } );
    		
		    Test( Validator_float.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT4, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new float[][] { new float[] { float.MaxValue, float.MaxValue }, new float[] { float.MaxValue, float.MaxValue } } );
    		
		    Test( Validator_float.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT4, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new float[][] { new float[] { float.MinValue, float.MinValue }, new float[] { float.MinValue, float.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_double()
	    {
		    Test( Validator_double.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT8, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new double[][] { new double[] { 0, 0 }, new double[] { 0, 0 } } );
    		
		    Test( Validator_double.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT8, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new double[][] { new double[] { 1, 1 }, new double[] { 1, 1 } } );
    		
		    Test( Validator_double.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT8, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new double[][] { new double[] { -1, -1 }, new double[] { -1, -1 } } );
    		
		    Test( Validator_double.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT8, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new double[][] { new double[] { Double.MaxValue, Double.MaxValue }, new double[] { Double.MaxValue, Double.MaxValue } } );
    		
		    Test( Validator_double.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.FLOAT8, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new double[][] { new double[] { Double.MinValue, Double.MinValue }, new double[] { Double.MinValue, Double.MinValue } } );
	    }

	    
	    [Test]
	    public void Test_string()
	    {
		    Test( Validator_string.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.EMPTY_STRING, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new String[][] { new String[] { "", "" }, new String[] { "", "" } } );
    		
		    Test( Validator_string.Get( 2 ), 2,
			    new SByte[] { Etch.Transport.Fmt.TypeCode.STRING, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
			    new String[][] { new String[] { "abc", "abc" }, new String[] { "abc", "abc" } } );
	    }

	    
	    [Test]
	    public void Test_custom()
	    {
            Test( Validator_custom.Get( typeof( DateTime ), 2 ), 2,
                new SByte[] { Etch.Transport.Fmt.TypeCode.CUSTOM, Etch.Transport.Fmt.TypeCode.ARRAY, Etch.Transport.Fmt.TypeCode.ARRAY },
                new DateTime[][] { new DateTime[] { new DateTime(), new DateTime() }, new DateTime[] { new DateTime(), new DateTime() } } );
	    }
    	
	    private void Test( Validator v, int n, SByte[] tcs, Object o )
	    {
    //		System.out.printf( "Testing with %s\n", v );
    //		Assert.assertEquals( n, v.GetDim() );
    		
		    Assert.IsTrue( v.Validate( o ) );
		    Assert.AreEqual( tcs[n], v.CheckValue( o ) );
    		
		    if (n > 0)
		    {


                int k = ( ( Array ) o ).Length;
                for ( int i = 0; i < k; i++ )
                    Test( v.ElementValidator(), n-1, tcs, ( ( Array ) o ).GetValue( i ) );
		    }
	    }
    }
}
