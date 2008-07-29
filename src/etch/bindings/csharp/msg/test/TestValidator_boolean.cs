using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Msg;

namespace Etch.Msg.Test
{
    [TestFixture]
    public class TestValidator_boolean
    {
        [Test]
	    public void Test() 
	    {
		    Test( 0, "boolean[0]", Etch.Transport.Fmt.TypeCode.BOOLEAN_FALSE, typeof(Boolean), false, (sbyte) 1 );
		    Test( 0, "boolean[0]", Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE, typeof(Boolean), true, (short) 2 );
		    Test( 0, "boolean[0]", Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE, typeof(Boolean), true, (int) 3 );
		    Test( 0, "boolean[0]", Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE, typeof(Boolean), true, (long) 4 );
		    Test( 0, "boolean[0]", Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE, typeof(Boolean), true, (float) 5 );
		    Test( 0, "boolean[0]", Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE, typeof(Boolean), true, (double) 6 );
		    Test( 0, "boolean[0]", Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE, typeof(Boolean), true, "abc" );
		    Test( 0, "boolean[0]", Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE, typeof(Boolean), true, new Object() );
		    Test( 1, "boolean[1]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(Boolean[]), new Boolean[] {}, true );
		    Test( 2, "boolean[2]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(Boolean[][]), new Boolean[][] {}, new Boolean[] {} );
		    Test( 3, "boolean[3]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(Boolean[][][]), new Boolean[][][] {}, new Boolean[][][][] {} );
		    Test( 4, "boolean[4]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(Boolean[][][][]), new Boolean[][][][] {}, true );
		    Test( 5, "boolean[5]", Etch.Transport.Fmt.TypeCode.ARRAY, typeof(Boolean[][][][][]), new Boolean[][][][][] {}, true );
	    }
    	
	    private void Test( int n, String s, sbyte tc, Type clss, Object good, Object bad )
	    {
		    Validator_boolean v = Validator_boolean.Get( n );
		    Assert.AreEqual( n, v.GetNDims() );
		    Assert.AreEqual( clss, v.GetExpectedClass() );
		    Assert.AreEqual( s, v.ToString() );
		    Assert.IsTrue( v.Validate( good ) );
		    Assert.IsFalse( v.Validate( bad ) );
		    Assert.AreEqual( tc, v.CheckValue( good ) );
		    Assert.IsNull( v.CheckValue( bad ) );
		    if (n > 0)
			    Assert.AreEqual( n-1, ((Validator_boolean) v.ElementValidator()).GetNDims() );
	    }

        
	    [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
	    public void TestGetNeg1()
	    {		
		    Validator_boolean.Get( -1 );
	    }


        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void TestGetNeg2()
	    {		
		    Validator_boolean.Get( 0 ).ElementValidator();
	    }


        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void TestGetMaxPlusOne()
	    {		
		    Validator_boolean.Get( Validator.MAX_NDIMS + 1 );			
	    }
    }
}
