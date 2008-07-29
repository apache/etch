using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Msg;

namespace Etch.Msg.Test
{
    [TestFixture]
    public class TestValidator_int
    {
        [Test]
	    public void Test()
	    {
		    Test( 0, "int[0]", Etch.Transport.Fmt.TypeCode.INT1, typeof(int), 0, false );
		    Test( 0, "int[0]", Etch.Transport.Fmt.TypeCode.INT1, typeof(int), (int) SByte.MaxValue, (sbyte) 1 );
		    Test( 0, "int[0]", Etch.Transport.Fmt.TypeCode.INT1, typeof(int), (int) SByte.MinValue, (short) 2 );
		    Test( 0, "int[0]", Etch.Transport.Fmt.TypeCode.INT2, typeof(int), (int) short.MaxValue, (long) 3 );
		    Test( 0, "int[0]", Etch.Transport.Fmt.TypeCode.INT2, typeof(int), (int) short.MinValue, (float) 4 );
		    Test( 0, "int[0]", Etch.Transport.Fmt.TypeCode.INT4, typeof(int), int.MaxValue, (double) 5 );
		    Test( 0, "int[0]", Etch.Transport.Fmt.TypeCode.INT4, typeof(int), int.MinValue, "abc" );
		    Test( 1, "int[1]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(int[]), new int[] {}, 1 );
		    Test( 2, "int[2]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(int[][]), new int[][] {}, new int[] {} );
		    Test( 3, "int[3]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(int[][][]), new int[][][] {}, new int[][] {} );
		    Test( 4, "int[4]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(int[][][][]), new int[][][][] {}, new int[][][][][] {} );
		    Test( 5, "int[5]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(int[][][][][]), new int[][][][][] {}, new Object() );
	    }
    	
	    private void Test( int n, String s, sbyte tc, Type clss, Object good, Object bad )
	    {
		    Validator_int v = Validator_int.Get( n );
		    Assert.AreEqual( n, v.GetNDims() );
		    Assert.AreEqual( clss, v.GetExpectedClass() );
		    Assert.AreEqual( s, v.ToString() );
		    Assert.IsTrue( v.Validate( good ) );
		    Assert.IsFalse( v.Validate( bad ) );
		    Assert.AreEqual( tc, v.CheckValue( good ) );
		    Assert.IsNull( v.CheckValue( bad ) );
		    if (n > 0)
			    Assert.AreEqual( n-1, ((Validator_int) v.ElementValidator()).GetNDims() );
	    }


        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void TestGetNeg1()
	    {		
		    Validator_int.Get( -1 );
	    }


        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void TestGetNeg2()
	    {		
		    Validator_int.Get( 0 ).ElementValidator();
	    }


        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void TestGetMaxPlusOne()
	    {		
		    Validator_int.Get( Validator.MAX_NDIMS + 1 );			
	    }
    }
}
