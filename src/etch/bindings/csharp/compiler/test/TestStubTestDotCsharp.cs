using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using etch.bindings.csharp.compiler.test;

namespace Etch.Compiler.Test
{
    [TestFixture]
    public class TestStubTestDotCsharp
    {
        [SetUp]
        public void Setup()
        {
            test = new FakeTest();
	        vf = new ValueFactoryTest();
	        pool = new FreePool();
            src = new MyMessageSource( this );
            stub = new StubTest( src, test, pool, pool );
            xreplyMon.Set( null );
        }

        Monitor<Message> xreplyMon = new Monitor<Message>( "Monitor for xreply" );

        [Test]
	    public void method_nothing() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_nothing, vf );
            // msg.Add( ValueFactoryTest._mf_x, 3 );
		    stub.Message( null, msg );
		    // check the result.
            xreplyMon.WaitUntilNotEq( null );
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_nothing );
		    Assert.AreEqual( 0, src.xreply.Count );
	    }

	    [Test]
	    public void method_incr1() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_incr, vf );
		    msg.Add( ValueFactoryTest._mf_x, 3 );
		    stub.Message( null, msg );
		    // check the result.
            xreplyMon.WaitUntilNotEq( null );
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_incr );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( 4, src.xreply.Get( ValueFactoryTest._mf_result ) );
	    }


	    [Test]
	    public void method_sub() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_sub, vf );
		    msg.Add( ValueFactoryTest._mf_x, 7 );
		    msg.Add( ValueFactoryTest._mf_y, 3 );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_sub );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( 4, src.xreply.Get( ValueFactoryTest._mf_result ) );
	    }

	    [Test]
	    public void method_sum() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_sum, vf );
            msg.Add( ValueFactoryTest._mf_x, new int[] { 1, 2, 3, 7, 11 } );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_sum );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( 24, src.xreply.Get( ValueFactoryTest._mf_result ) );
	    }

	    [Test]
	    public void method_trans1() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_trans, vf );
		    msg.Add( ValueFactoryTest._mf_e, etch.bindings.csharp.compiler.test.ConstsTest.E1.A );
		    msg.Add( ValueFactoryTest._mf_x, 5 );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_trans );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( 2, src.xreply.Get( ValueFactoryTest._mf_result ) );
	    }

	    [Test]
	    public void method_trans2() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_trans, vf );
		    msg.Add( ValueFactoryTest._mf_e, etch.bindings.csharp.compiler.test.ConstsTest.E1.B );
		    msg.Add( ValueFactoryTest._mf_x, 5 );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_trans );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( 10, src.xreply.Get( ValueFactoryTest._mf_result ) );
	    }

	    [Test]
	    public void method_trans3() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_trans, vf );
		    msg.Add( ValueFactoryTest._mf_e, etch.bindings.csharp.compiler.test.ConstsTest.E1.C );
		    msg.Add( ValueFactoryTest._mf_x, 5 );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_trans );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( 12, src.xreply.Get( ValueFactoryTest._mf_result ) );
	    }

	    [Test]
	    public void method_dist1() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_dist, vf );
		    msg.Add( ValueFactoryTest._mf_a, new etch.bindings.csharp.compiler.test.ConstsTest.S1( 1, 1, 1 ) );
		    msg.Add( ValueFactoryTest._mf_b, new etch.bindings.csharp.compiler.test.ConstsTest.S1( 0, 0, 0 ) );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_dist );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( Math.Sqrt( 3 ), src.xreply.Get( ValueFactoryTest._mf_result ) );
	    }

	    [Test]
	    public void method_dist2() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_dist, vf );
		    msg.Add( ValueFactoryTest._mf_a, new etch.bindings.csharp.compiler.test.ConstsTest.S1( 1, 2, 3 ) );
		    msg.Add( ValueFactoryTest._mf_b, new etch.bindings.csharp.compiler.test.ConstsTest.S1( 6, 5, 4 ) );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_dist );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( Math.Sqrt( 35 ), src.xreply.Get( ValueFactoryTest._mf_result ) );
	    }

	    [Test]
	    public void method_fill() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_fill, vf );
		    msg.Add( ValueFactoryTest._mf_n, 4 );
		    msg.Add( ValueFactoryTest._mf_x, 3 );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_fill );
		    Assert.AreEqual( 1, src.xreply.Count );
		    int[] x = (int[]) src.xreply.Get( ValueFactoryTest._mf_result );
            Assert.IsNotNull( x );
		    Assert.AreEqual( 4, x.Length );
		    foreach (int y in x)
			    Assert.AreEqual( 3, y );
	    }

	    [Test]
	    public void method_blow() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_blow, vf );
		    msg.Add( ValueFactoryTest._mf_msg, "foo" );
		    msg.Add( ValueFactoryTest._mf_code, 23 );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_blow );
		    Assert.AreEqual( 1, src.xreply.Count );
		    etch.bindings.csharp.compiler.test.ConstsTest.Excp1 e = (etch.bindings.csharp.compiler.test.ConstsTest.Excp1) src.xreply.Get( ValueFactoryTest._mf_result );
		    Assert.IsNotNull( e );
		    Assert.AreEqual( "foo", e.msg );
		    Assert.AreEqual( 23, e.code );
	    }

	    [Test]
	    public void method_beets1() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_beets, vf );
		    msg.Add( ValueFactoryTest._mf_e, etch.bindings.csharp.compiler.test.ConstsTest.E1.A );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_beets );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest._mf_result );
		    Assert.AreEqual( 5, o );
	    }

	    [Test]
	    public void method_beets2() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_beets, vf );
		    msg.Add( ValueFactoryTest._mf_e, etch.bindings.csharp.compiler.test.ConstsTest.E1.B );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_beets );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest._mf_result );
		    Assert.AreEqual( typeof(etch.bindings.csharp.compiler.test.ConstsTest.Excp3), o.GetType() );
	    }

	    [Test]
	    public void method_beets3() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_beets, vf );
		    msg.Add( ValueFactoryTest._mf_e, etch.bindings.csharp.compiler.test.ConstsTest.E1.C );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_beets );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest._mf_result );
		    Assert.AreEqual( typeof(etch.bindings.csharp.compiler.test.ConstsTest.Excp4), o.GetType() );
	    }

	    //[Test]
	    //public void method_beets4() 
	    //{
		//    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_beets, vf );
		//    msg.Add( ValueFactoryTest._mf_e, null );
		//    stub.Message( null, msg );
		   // check the result.
		//    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_beets );
		//    Assert.AreEqual( 0, src.xreply.Count );
		//    Object o = src.xreply.Get( ValueFactoryTest._mf_result );
		//    Assert.IsNull( o );
	    //}
    	
	    [Test]
	    public void method_isTrue() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_isTrue, vf );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_isTrue );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest._mf_result );
		    Assert.AreEqual( true, o );
	    }
    	
	    [Test]
	    public void method_isFalse() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_isFalse, vf );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_isFalse );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest._mf_result );
		    Assert.AreEqual( false, o );
	    }
    	
	    [Test]
	    public void method_alwaysWorks() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_alwaysWorks, vf );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_alwaysWorks );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest._mf_result );
		    Assert.AreEqual( 23, o );
	    }
    	
	    [Test]
	    public void method_alwaysFails() 
	    {
		    Message msg = new Message( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_alwaysFails, vf );
		    stub.Message( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_alwaysFails );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest._mf_result );
		    Assert.IsTrue( o is _Etch_AuthException );
		    _Etch_AuthException e = (_Etch_AuthException) o;
		    Assert.AreEqual( "alwaysFails", e.msg );
	    }

        private FakeTest test;
        private ValueFactoryTest vf;
        private Pool pool;

        private StubTest stub;
        private MyMessageSource src;
    	
	    public class MyMessageSource : MessageSource
	    {

            private TestStubTestDotCsharp _baseObj;

            public MyMessageSource( TestStubTestDotCsharp baseObj )
            {
                _baseObj = baseObj;
            }

            #region MessageSource Members
            
            public Message xreply;
            
            public void Messagex( Who recipient, Message msg )
            {
                Assert.IsNull( recipient );
                Assert.IsNull( xreply );
                xreply = msg;
                _baseObj.xreplyMon.Set( new Message( new XType( "dummy" ), new ValueFactoryTest() ) );
            }

            #endregion
        }
    }
}
