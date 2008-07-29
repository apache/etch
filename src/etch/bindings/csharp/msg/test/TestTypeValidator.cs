using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

namespace Etch.Msg.Test
{
    [TestFixture]
    public class TestTypeValidator
    {

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void TestValidate()
        {
            new MyTypeValidator( typeof( int ), typeof( int ), -1, "bzzt" );
        }

        [Test]
        public void TestTypeValidator1()
        {
            MyTypeValidator v = new MyTypeValidator( typeof( int ), null, 0, "abc" );
            Assert.AreEqual( typeof( int ), v.GetExpectedClass() );
            Assert.AreEqual( 0, v.GetNDims() );
            Assert.AreEqual( "abc", v.ToString() );
        }

        [Test]
        public void TestTypeValidator2()
        {
            MyTypeValidator v = new MyTypeValidator( typeof(Boolean), null, 0, "def" );
            Assert.AreEqual( typeof( Boolean ), v.GetExpectedClass() );
		    Assert.AreEqual( 0, v.GetNDims() );
		    Assert.AreEqual( "def", v.ToString() );
        }

        [Test]
	    public void TestTypeValidator3()
	    {
		    MyTypeValidator v = new MyTypeValidator( null, typeof(int), 1, "ghi" );
		    Assert.AreEqual( typeof(int[]), v.GetExpectedClass() );
		    Assert.AreEqual( 1, v.GetNDims() );
		    Assert.AreEqual( "ghi", v.ToString() );
	    }

        [Test]
	    public void TestTypeValidator4()
	    {
		    MyTypeValidator v = new MyTypeValidator( null, typeof(Boolean), 1, "jkl" );
		    Assert.AreEqual( typeof(Boolean[]), v.GetExpectedClass() );
		    Assert.AreEqual( 1, v.GetNDims() );
		    Assert.AreEqual( "jkl", v.ToString() );
	    }

        [Test]
	    public void TestToString()
	    {
		    MyTypeValidator v = new MyTypeValidator( typeof(int), typeof(int), 0, "abc" );
		    Assert.AreEqual( "abc", v.ToString() );
    		
		    v = new MyTypeValidator( typeof(int), typeof(int), 0, "def" );
		    Assert.AreEqual( "def", v.ToString() );
    		
		    v = new MyTypeValidator( typeof(int), typeof(int), 0, "ghi" );
		    Assert.AreEqual( "ghi", v.ToString() );
	    }

        [Test]
	    public void TestSilly()
	    {
		    new MyTypeValidator( typeof(int), typeof(int), 0, "ghi" ).CheckValue( 0 );
		    new MyTypeValidator( typeof(int), typeof(int), 0, "ghi" ).ElementValidator();
	    }

        public class MyTypeValidator : TypeValidator
        {
            public MyTypeValidator( Type scalarClass, Type arrayClass, int nDims,
                String descr ) : base ( scalarClass, arrayClass, nDims, descr)
            { }

            public override sbyte? CheckValue( Object value )
            {
                // ignore
                return null;
            }

            public override Validator ElementValidator()
            {
                // ignore
                return null;
            }
        }
    }
}
