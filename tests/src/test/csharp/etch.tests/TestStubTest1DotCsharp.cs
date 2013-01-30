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
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Transport;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Test1;

namespace etch.tests
{
    [TestFixture]
    public class TestStubTest1DotCsharp
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestStub" );
        }

        [SetUp]
        public void Setup()
        {
            test = new FakeTest1();
            vf = new ValueFactoryTest1("none:");
	        pool = new FreePool();
            src = new MyMessageSource( this );
            stub = new StubTest1( src, test, pool, pool );
            xreplyMon.Set( null );
        }

        Monitor<Message> xreplyMon = new Monitor<Message>( "Monitor for xreply" );

        [Test]
	    public void method_nothing() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_nothing, vf );
            // msg.Add( ValueFactoryTest1._mf_x, 3 );
		    stub.SessionMessage( null, msg );
		    // check the result.
            xreplyMon.WaitUntilNotEq( null );
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_nothing );
		    Assert.AreEqual( 0, src.xreply.Count );
	    }

	    [Test]
	    public void method_incr1() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr, vf );
		    msg.Add( ValueFactoryTest1._mf_x, 3 );
		    stub.SessionMessage( null, msg );
		    // check the result.
            xreplyMon.WaitUntilNotEq( null );
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr );
            Assert.AreEqual(1, src.xreply.Count);
		    Assert.AreEqual( 4, src.xreply.Get( ValueFactoryTest1._mf_result ) );
	    }


	    [Test]
	    public void method_sub() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub, vf );
		    msg.Add( ValueFactoryTest1._mf_x, 7 );
		    msg.Add( ValueFactoryTest1._mf_y, 3 );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub );
            Assert.AreEqual(1, src.xreply.Count);
		    Assert.AreEqual( 4, src.xreply.Get( ValueFactoryTest1._mf_result ) );
	    }

	    [Test]
	    public void method_sum() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sum, vf );
            msg.Add( ValueFactoryTest1._mf_x, new int[] { 1, 2, 3, 7, 11 } );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sum );
            Assert.AreEqual(1, src.xreply.Count);
		    Assert.AreEqual( 24, src.xreply.Get( ValueFactoryTest1._mf_result ) );
	    }

	    [Test]
	    public void method_trans1() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans, vf );
		    msg.Add( ValueFactoryTest1._mf_e, E1.A );
		    msg.Add( ValueFactoryTest1._mf_x, 5 );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans );
            Assert.AreEqual(1, src.xreply.Count);
		    Assert.AreEqual( 2, src.xreply.Get( ValueFactoryTest1._mf_result ) );
	    }

	    [Test]
	    public void method_trans2() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans, vf );
		    msg.Add( ValueFactoryTest1._mf_e, E1.B );
		    msg.Add( ValueFactoryTest1._mf_x, 5 );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans );
            Assert.AreEqual(1, src.xreply.Count);
		    Assert.AreEqual( 10, src.xreply.Get( ValueFactoryTest1._mf_result ) );
	    }

	    [Test]
	    public void method_trans3() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans, vf );
		    msg.Add( ValueFactoryTest1._mf_e, E1.C );
		    msg.Add( ValueFactoryTest1._mf_x, 5 );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans );
            Assert.AreEqual(1, src.xreply.Count);
		    Assert.AreEqual( 12, src.xreply.Get( ValueFactoryTest1._mf_result ) );
	    }

	    [Test]
	    public void method_dist1() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist, vf );
		    msg.Add( ValueFactoryTest1._mf_a, new S1( 1, 1, 1 ) );
		    msg.Add( ValueFactoryTest1._mf_b, new S1( 0, 0, 0 ) );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( Math.Sqrt( 3 ), src.xreply.Get( ValueFactoryTest1._mf_result ) );
	    }

	    [Test]
	    public void method_dist2() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist, vf );
		    msg.Add( ValueFactoryTest1._mf_a, new S1( 1, 2, 3 ) );
		    msg.Add( ValueFactoryTest1._mf_b, new S1( 6, 5, 4 ) );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Assert.AreEqual( Math.Sqrt( 35 ), src.xreply.Get( ValueFactoryTest1._mf_result ) );
	    }

	    [Test]
	    public void method_fill() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill, vf );
		    msg.Add( ValueFactoryTest1._mf_n, 4 );
		    msg.Add( ValueFactoryTest1._mf_x, 3 );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill );
            Assert.AreEqual(1, src.xreply.Count);
		    int[] x = (int[]) src.xreply.Get( ValueFactoryTest1._mf_result );
            Assert.IsNotNull( x );
		    Assert.AreEqual( 4, x.Length );
		    foreach (int y in x)
			    Assert.AreEqual( 3, y );
	    }

	    [Test]
	    public void method_blow() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_blow, vf );
		    msg.Add( ValueFactoryTest1._mf_msg, "foo" );
		    msg.Add( ValueFactoryTest1._mf_code, 23 );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow );
            Assert.AreEqual(1, src.xreply.Count);
		    Excp1 e = (Excp1) src.xreply.Get( ValueFactoryTest1._mf_result );
		    Assert.IsNotNull( e );
		    Assert.AreEqual( "foo", e.msg );
		    Assert.AreEqual( 23, e.code );
	    }

	    [Test]
	    public void method_beets1() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets, vf );
		    msg.Add( ValueFactoryTest1._mf_e, E1.A );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest1._mf_result );
		    Assert.AreEqual( 5, o );
	    }

	    [Test]
	    public void method_beets2() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets, vf );
		    msg.Add( ValueFactoryTest1._mf_e, E1.B );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest1._mf_result );
		    Assert.AreEqual( typeof(Excp3), o.GetType() );
	    }

	    [Test]
	    public void method_beets3() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets, vf );
		    msg.Add( ValueFactoryTest1._mf_e, E1.C );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets );
		    Assert.AreEqual( 1, src.xreply.Count );
		    Object o = src.xreply.Get( ValueFactoryTest1._mf_result );
		    Assert.AreEqual( typeof(Excp4), o.GetType() );
	    }

	    //[Test]
	    //public void method_beets4() 
	    //{
		//    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets, vf );
		//    msg.Add( ValueFactoryTest1._mf_e, null );
		//    stub.Message( null, msg );
		   // check the result.
		//    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets );
		//    Assert.AreEqual( 0, src.xreply.Count );
		//    Object o = src.xreply.Get( ValueFactoryTest1._mf_result );
		//    Assert.IsNull( o );
	    //}
    	
	    [Test]
	    public void method_isTrue() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_isTrue, vf );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_isTrue );
            Assert.AreEqual(1, src.xreply.Count);
		    Object o = src.xreply.Get( ValueFactoryTest1._mf_result );
		    Assert.AreEqual( true, o );
	    }
    	
	    [Test]
	    public void method_isFalse() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_isFalse, vf );
		    stub.SessionMessage( null, msg );
		    // check the result.
		    src.xreply.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_isFalse );
            Assert.AreEqual(1, src.xreply.Count);
		    Object o = src.xreply.Get( ValueFactoryTest1._mf_result );
		    Assert.AreEqual( false, o );
	    }
    	
	    [Test]
	    public void method_alwaysWorks() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_alwaysWorks, vf );
		    stub.SessionMessage( null, msg );
		    // check the result.
            src.xreply.CheckType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_alwaysWorks);
            Assert.AreEqual(1, src.xreply.Count);
            Object o = src.xreply.Get(ValueFactoryTest1._mf_result);
		    Assert.AreEqual( 23, o );
	    }
    	
	    [Test]
	    public void method_alwaysFails() 
	    {
		    Message msg = new Message( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_alwaysFails, vf );
		    stub.SessionMessage( null, msg );
		    // check the result.
            src.xreply.CheckType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_alwaysFails);
            Assert.AreEqual(1, src.xreply.Count);
            Object o = src.xreply.Get(ValueFactoryTest1._mf_result);
		    Assert.IsTrue( o is _Etch_AuthException );
		    _Etch_AuthException e = (_Etch_AuthException) o;
		    Assert.AreEqual( "alwaysFails", e.msg );
	    }

        private FakeTest1 test;
        private ValueFactoryTest1 vf;
        private Pool pool;

        private StubTest1 stub;
        private MyMessageSource src;
    	
	    public class MyMessageSource : DeliveryService
	    {

            private TestStubTest1DotCsharp _baseObj;
	        private SessionMessage session;

            public MyMessageSource( TestStubTest1DotCsharp baseObj  )
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
                _baseObj.xreplyMon.Set(new Message(new XType("dummy"), new ValueFactoryTest1("none:")));
            }

            #endregion

            #region DeliveryService Members

            public Mailbox BeginCall(Message msg)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public object EndCall(Mailbox mb, XType responseType)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region TransportMessage Members

            public void TransportMessage(Who recipient, Message msg)
            {
                Assert.IsNull(recipient);
                Assert.IsNull(xreply);
                xreply = msg;
                _baseObj.xreplyMon.Set(new Message(new XType("dummy"), new ValueFactoryTest1("none:")));
            }

            #endregion

            #region Transport<SessionMessage> Members

            public object TransportQuery(object query)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void TransportControl(object control, object value)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void TransportNotify(object eventObj)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SetSession(SessionMessage session)
            {
                this.session = session;
            }

            #endregion

            #region SessionMessage Members

            public bool SessionMessage(Who sender, Message msg)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region Session Members

            public object SessionQuery(object query)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SessionControl(object control, object value)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SessionNotify(object eventObj)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region Transport<SessionMessage> Members


            public SessionMessage GetSession()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion
        }
    }
}
