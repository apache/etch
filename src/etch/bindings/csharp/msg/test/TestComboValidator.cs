using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

namespace Etch.Msg.Test
{
    [TestFixture]
    public class TestComboValidator
    {
        [Test]
        public void Test()
        {
            Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE, true, ( sbyte ) 1 );
            Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.BOOLEAN_FALSE, false, (short) 2 );
		    Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT1, 0, (long) 3 );
		    Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT1, 1, (float) 4 );
		    Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT1, -1, (double) 5 );
		    Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT1, 127, "abc" );
		    Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT1, -128, new Object() );
		    Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT2, 32767, new int[] {} );
		    Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT2, -32768, new Boolean[] {} );
		    Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT4, 1000000000, new DateTime() );
            Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT4, -1000000000, typeof( int ) );
		    Test( 1, "(int[1] OR boolean[1])", Etch.Transport.Fmt.TypeCode.ARRAY, new int[] {}, 0 );
		    Test( 1, "(int[1] OR boolean[1])", Etch.Transport.Fmt.TypeCode.ARRAY, new Boolean[] {}, true );
        }

        [Test]
        private void Test( int n, String s, sbyte tc, Object good, Object bad )
        {
            Validator_int v1 = Validator_int.Get( n );
            Validator_boolean v2 = Validator_boolean.Get( n );
            ComboValidator v = new ComboValidator( v1, v2 );

            Assert.AreEqual( s, v.ToString() );
            Assert.IsTrue( v.Validate( good ) );
            Assert.IsFalse( v.Validate( bad ) );
            Assert.AreEqual( tc, v.CheckValue( good ) );
            Assert.IsNull( v.CheckValue( bad ) );
            if ( n > 0 )
            {
                v.ElementValidator();
            }
        }

        [Test]
	    public void Test2()  
	    {
		    Validator v1 = Validator_int.Get( 1 );
		    Validator v2 = Validator_boolean.Get( 0 );
		    Validator v = new ComboValidator( v1, v2 );

		    Assert.IsTrue( v.Validate( new int[] {} ) );
		    Assert.IsTrue( v.Validate( true ) );
		    Assert.IsTrue( v.Validate( false ) );
		    Assert.IsFalse( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();

		    Assert.IsFalse( v.Validate( new int[] {} ) );
		    Assert.IsFalse( v.Validate( true ) );
		    Assert.IsFalse( v.Validate( false ) );
		    Assert.IsTrue( v.Validate( 0 ) );
	    }
    	
	    [Test]
	    public void Test3()  
	    {
		    Validator v1 = Validator_boolean.Get( 0 );
		    Validator v2 = Validator_int.Get( 1 );
		    Validator v = new ComboValidator( v1, v2 );

		    Assert.IsTrue( v.Validate( new int[] {} ) );
		    Assert.IsTrue( v.Validate( true ) );
		    Assert.IsTrue( v.Validate( false ) );
		    Assert.IsFalse( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();

		    Assert.IsFalse( v.Validate( new int[] {} ) );
		    Assert.IsFalse( v.Validate( true ) );
		    Assert.IsFalse( v.Validate( false ) );
		    Assert.IsTrue( v.Validate( 0 ) );
	    }


        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void Test4()  
	    {
		    Validator v1 = Validator_boolean.Get( 0 );
		    Validator v2 = Validator_int.Get( 1 );
		    Validator v = new ComboValidator( v1, v2 );

		    Assert.IsTrue( v.Validate( new int[] {} ) );
		    Assert.IsTrue( v.Validate( true ) );
		    Assert.IsTrue( v.Validate( false ) );
		    Assert.IsFalse( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();

		    Assert.IsFalse( v.Validate( new int[] {} ) );
		    Assert.IsFalse( v.Validate( true ) );
		    Assert.IsFalse( v.Validate( false ) );
		    Assert.IsTrue( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();
	    }
    	
	    [Test]
        [ExpectedException(typeof(ArgumentException))]
	    public void Test5()  
	    {
		    Validator v1 = Validator_boolean.Get( 1 );
		    Validator v2 = Validator_int.Get( 1 );
		    Validator v = new ComboValidator( v1, v2 );

		    Assert.IsTrue( v.Validate( new Boolean[] {} ) );
		    Assert.IsTrue( v.Validate( new int[] {} ) );
    		
		    v = v.ElementValidator();

		    Assert.IsTrue( v.Validate( true ) );
		    Assert.IsTrue( v.Validate( false ) );
		    Assert.IsTrue( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();
	    }
    }
}
