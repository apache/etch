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
using System.Threading;
using System.Globalization;
using NUnit.Framework;

// s is scheme:[//[user[:password]@]host[:port]/]uri[;params][?terms][#fragment]

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    [TestFixture]
    public class TestURL
    {

        private CultureInfo savedCulture;

        [SetUp]
        public void SetUp()
        {
            savedCulture = Thread.CurrentThread.CurrentCulture;
            Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;
        }

        [TearDown]
        public void TearDown()
        {
            Thread.CurrentThread.CurrentCulture = savedCulture;
        }

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestURL" );
        }

        [ExpectedException( typeof( ArgumentNullException ) )]
        public void TestNullURL()
        {
            string testUrl = "";
            URL url1 = new URL( testUrl );

        }
        
        /// <summary>
        /// Parse a simple URL
        /// </summary>
        [Test]
        public void TestParse1()
        {
            string testUrl = "tcp://user:password@127.0.0.1:4001";
            URL url1 = new URL( testUrl );
            Assert.AreEqual( "tcp", url1.Scheme );
            Assert.AreEqual( "user", url1.User );
            Assert.AreEqual( "password", url1.Password );
            Assert.AreEqual( "127.0.0.1", url1.Host );
            Assert.AreEqual( 4001, url1.Port );

            //scheme tests
            
            Assert.IsFalse( url1.HasTerms() );
            Assert.IsFalse( url1.HasParams() );

            Assert.AreEqual( "tcp://user:password@127.0.0.1:4001/", url1.ToString() );
        }

        [Test]
        public void TestSchemeFunctions()
        {
            string testUrl = "tcp://user:password@127.0.0.1:4001";
            URL url1 = new URL( testUrl );

            Assert.AreEqual( "tcp", url1.Scheme );
            Assert.IsTrue( url1.IsScheme( "tcp" ) );
            Assert.IsFalse( url1.IsScheme( "" ) );
            Assert.IsFalse( url1.IsScheme( "tcp-" ) );

            url1.Scheme = "udp";
            Assert.AreEqual( "udp", url1.Scheme );
            Assert.IsTrue( url1.IsScheme( "udp" ) );

            Assert.AreEqual( "udp://user:password@127.0.0.1:4001/", url1.ToString() );
        }

        [Test]
        public void TestUser()
        {
            string testUrl = "tcp://user:password@127.0.0.1:4001";
            URL url1 = new URL( testUrl );

            Assert.AreEqual( "user", url1.User );
            
            url1.User = "user2";
            Assert.AreEqual( "user2", url1.User );

            try
            {
                url1.User = "";
                Assert.Fail();
            }
            catch ( Exception )
            {
                Assert.IsTrue( true );
            }

            Assert.AreEqual( "tcp://user2:password@127.0.0.1:4001/", url1.ToString() );
        }

        [Test]
        public void TestPassword()
        {
            string testUrl = "tcp://user:password@127.0.0.1:4001";
            URL url1 = new URL( testUrl );

            Assert.AreEqual( "password", url1.Password );

            url1.Password = "password2";
            Assert.AreEqual( "password2", url1.Password );

            Assert.AreEqual( "tcp://user:password2@127.0.0.1:4001/", url1.ToString() );
        }

        [Test]
        public void TestHost()
        {
            string testUrl = "tcp://user:password@127.0.0.1:4001";
            URL url1 = new URL( testUrl );

            Assert.AreEqual( "127.0.0.1", url1.Host );

            url1.Host = "localhost";
            Assert.AreEqual( "localhost", url1.Host );

            Assert.AreEqual( "tcp://user:password@localhost:4001/", url1.ToString() );
        }

        [Test]
        public void TestPort()
        {
            string testUrl = "tcp://user:password@127.0.0.1:4001";
            URL url1 = new URL( testUrl );

            Assert.AreEqual( 4001, url1.Port );

            url1.Port = 8000;
            Assert.AreEqual( 8000, url1.Port );

            try
            {
                url1.Port = -1;
                Assert.Fail();
            }
            catch ( ArgumentException )
            {
                Assert.IsTrue( true );
                try
                {
                    url1.Port = 65536;
                    Assert.Fail();
                }
                catch ( ArgumentException )
                {
                    Assert.IsTrue( true );
                }
            }

            Assert.AreEqual( "tcp://user:password@127.0.0.1:8000/", url1.ToString() );
        }

        [Test]
        public void TestUri()
        {
            string testUrl = "tcp://user:password@127.0.0.1:4001/myUri";
            URL url1 = new URL( testUrl );

            Assert.AreEqual( "myUri", url1.Uri );

            url1.Uri = "";
            Assert.AreEqual( "", url1.Uri );

            Assert.AreEqual( "tcp://user:password@127.0.0.1:4001/", url1.ToString() );
        }

        [Test]
        public void TestParams()
        {
            string testUrl = "tcp://user:password@127.0.0.1:4001/myUri";
            URL url = new URL( testUrl );
            Assert.IsFalse( url.HasParams() );
            Assert.AreEqual(0, url.GetParams().Length);
            Assert.IsNull(url.GetParam("PARAM1="));

            testUrl = "tcp://user:password@127.0.0.1:4001/cuae;PARAM1=1;PARAM2=2";
            url = new URL( testUrl );
            Assert.IsTrue( url.HasParams() );
            Assert.AreEqual( "PARAM1=1", url.GetParam( "PARAM1" ) );
            Assert.AreEqual( "PARAM2=2", url.GetParam( "PARAM2" ) );
            
            // get iterator over params
            string[] p = url.GetParams();
            Assert.AreEqual(2, p.Length);
            Assert.IsTrue(Find("PARAM1=1", p));
            Assert.IsTrue(Find("PARAM2=2", p));

            // add a new param
            url.AddParam( "0123456789" );
            Assert.AreEqual( "0123456789", url.GetParam( "0123" ) );
            p = url.GetParams();
            Assert.AreEqual(3, p.Length);
            Assert.IsTrue(Find("PARAM1=1", p));
            Assert.IsTrue(Find("PARAM2=2", p));
            Assert.IsTrue(Find("0123456789", p));

            // remove a param
            Assert.AreEqual( "PARAM1=1", url.RemoveParam( "PARAM1" ) );
            Assert.IsNull(url.GetParam("PARAM1="));
            p = url.GetParams();
            Assert.AreEqual(2, p.Length);
            Assert.IsTrue(Find("PARAM2=2", p));
            Assert.IsTrue(Find("0123456789", p));

            // clear all params
            url.ClearParams();
            Assert.IsFalse( url.HasParams() );
        }

        private bool Find(string s, string[] a)
        {
            foreach (string x in a)
                if (x.Equals(s))
                    return true;
            return false;
        }

        [Test]
        public void TestQueryTerms()
        {
            String testUrl = "tcp://user:password@127.0.0.1:4001/cuae";
            URL url = new URL( testUrl );
            Assert.IsFalse( url.HasTerms() );

            testUrl = "tcp://user:password@127.0.0.1:4001/cuae?term1=500";
            url = new URL( testUrl );

            Assert.IsTrue( url.HasTerms() );
            Assert.IsTrue( url.HasTerm( "term1" ) );
            Assert.IsTrue( url.HasTerm( "term1", "500" ) );
            Assert.IsTrue(url.HasTerm("term1", 500));
            Assert.IsFalse(url.HasTerm("term1", "1000"));
            Assert.IsFalse(url.HasTerm("term1", 1000));
            Assert.IsFalse(url.HasMultipleValues("term1"));
            Assert.AreEqual("500", url.GetTerm("term1"));
            Assert.AreEqual(500, url.GetIntegerTerm("term1"));
            Assert.AreEqual("500", url.GetTerm("term1", "x"));
            Assert.AreEqual(500, url.GetIntegerTerm("term1", 2));

            Assert.IsFalse(url.HasTerm("term2"));
            Assert.IsFalse(url.HasTerm("term2", "500"));
            Assert.IsFalse(url.HasTerm("term2", 500));
            Assert.IsNull(url.GetTerm("term2"));
            Assert.IsNull(url.GetIntegerTerm("term2"));
            Assert.AreEqual("x", url.GetTerm("term2", "x"));
            Assert.AreEqual(2, url.GetIntegerTerm("term2", 2));

            // multiple values of the same term
            url.AddTerm( "term1", 500.5 );
            Assert.IsTrue(url.HasMultipleValues("term1"));
            Assert.IsTrue(url.HasTerm("term1"));
            Assert.IsTrue(url.HasTerm("term1", "500"));
            Assert.IsTrue(url.HasTerm("term1", 500));
            Assert.IsTrue(url.HasTerm("term1", "500.5"));
            Assert.IsTrue(url.HasTerm("term1", 500.5));
            try
            {
                url.GetTerm( "term1" );
                Assert.Fail( "Exception wasn't thrown..." );
            }
            catch ( Exception )
            {
                // ignore exception
            }
            string[] t = url.GetTerms( "term1" );
            Assert.AreEqual(2, t.Length);
            Assert.AreEqual( "500", t[0] );
            Assert.AreEqual( "500.5", t[1] );

            // add another term => multiple terms
            url.AddTerm( "term2", "value" );
            Assert.IsFalse( url.HasMultipleValues( "term2" ) );
            Assert.IsTrue( url.HasTerm( "term2" ) );
            Assert.IsTrue( url.HasTerm( "term2", "value" ) );
            Assert.AreEqual( "value", url.GetTerm( "term2" ) );

            t = url.GetTermNames();
            Assert.AreEqual(2, t.Length);
            Assert.AreEqual( "term1", t[0] );
            Assert.AreEqual( "term2", t[1] );

            // Console.WriteLine( url.ToString() );
            // remove term
            Assert.IsFalse( url.RemoveTerm( "term3" ) );
            Assert.IsTrue( url.RemoveTerm( "term2" ) );
            Assert.IsFalse( url.HasTerm( "term2" ) );
            t = url.GetTermNames();
            Assert.AreEqual(1, t.Length);
            Assert.AreEqual("term1", t[0]);

            // remove one value from a list of values for a term
            Assert.IsTrue( url.RemoveTerm( "term1", "500.5" ) );
            Assert.IsFalse( url.HasMultipleValues( "term1" ) );
            Assert.AreEqual( "500", url.GetTerm( "term1" ) );

            // clear terms
            url.ClearTerms();
            Assert.IsFalse( url.HasTerms() );
            Assert.AreEqual(0, url.GetTermNames().Length);
            Assert.AreEqual( "tcp://user:password@127.0.0.1:4001/cuae", url.ToString() );
        }

        [Test]
        public void TestFragment()
        {
            string testUrl = "tcp://user:password@127.0.0.1:4001/cuae;p1;p2?t1=1?t2=2";
            URL url = new URL( testUrl );
            Assert.IsNull( url.Fragment );

            url.Fragment = "fragments";
            Assert.AreEqual( "fragments", url.Fragment );
            
        }

        public void TestConstructor()
        {
            string testUrl2 = "tcp://user:password@127.0.0.1:4001/cuae;p1;p2?t1=1?t2=2#fragment";
            URL url2 = new URL( testUrl2 );
            URL url3 = new URL( url2 );

            Assert.AreEqual( url2.Scheme, url3.Scheme );
            Assert.AreEqual( url2.User, url3.User );
            Assert.AreEqual( url2.Password, url3.Password );
            Assert.AreEqual( url2.Host, url3.Host);
            Assert.AreEqual( url2.Port, url3.Port );
            Assert.AreEqual( url2.Uri, url3.Uri );
            //Assert.AreEqual( url2.GetParams(), url3.GetParams() );
            //Assert.AreEqual( url2., url3.GetTerms() );
            Assert.AreEqual( url2.Fragment, url3.Fragment );

        }
    }
}
