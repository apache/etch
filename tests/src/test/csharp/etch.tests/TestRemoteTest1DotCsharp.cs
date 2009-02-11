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
    public class TestRemoteTest1DotCsharp
    {
        ValueFactory vf = new DummyValueFactory();

        public TestRemoteTest1DotCsharp()
        {
             svc = new MyDeliveryService();
             test = new RemoteTest1( svc, vf );
        }

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestRemote" );
        }


        [SetUp]
        public void SetUpAttribute()
        {
            svc = new MyDeliveryService();
            test = new RemoteTest1( svc, vf );
        }

        [Test]
        public void method_nothing()
        {
            test.nothing();
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_nothing,
                new Field[] { },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_nothing,
                4000 );
        }

        [Test]
        public void method_incr1()
        {
            svc.xresult = 2;
            Assert.AreEqual( 2, test.incr( 1 ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr,
                new Field[] { ValueFactoryTest1._mf_x },
                new Object[] { 1 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr,
                4000 );
        }

        [Test]
        public void method_incr2()
        {
            svc.xresult = 3;
            Assert.AreEqual( 3, test.incr( 2 ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr,
                new Field[] { ValueFactoryTest1._mf_x },
                new Object[] { 2 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr,
                4000 );
        }

        [Test]
        public void method_incr3()
        {
            svc.xresult = -1;
            Assert.AreEqual( -1, test.incr( -2 ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr,
                new Field[] { ValueFactoryTest1._mf_x },
                new Object[] { -2 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr,
                4000 );
        }

        [Test]
        public void method_sub1()
        {
            svc.xresult = 5;
            Assert.AreEqual( 5, test.sub( 7, 2 ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub,
                new Field[] { ValueFactoryTest1._mf_x, ValueFactoryTest1._mf_y },
                new Object[] { 7, 2 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub,
                4000 );
        }

        [Test]
        public void method_sub2()
        {
            svc.xresult = 8;
            Assert.AreEqual( 8, test.sub( 23, 15 ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub,
                new Field[] { ValueFactoryTest1._mf_x, ValueFactoryTest1._mf_y },
                new Object[] { 23, 15 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub,
                4000 );
        }

        [Test]
        public void method_sub3()
        {
            svc.xresult = -5;
            Assert.AreEqual( -5, test.sub( 2, 7 ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub,
                new Field[] { ValueFactoryTest1._mf_x, ValueFactoryTest1._mf_y },
                new Object[] { 2, 7 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub,
                4000 );
        }

        [Test]
        public void method_sum()
        {
            svc.xresult = 24;
            int[] arg = { 1, 2, 3, 7, 11 };
            Assert.AreEqual( 24, test.sum( arg ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sum,
                new Field[] { ValueFactoryTest1._mf_x },
                new Object[] { arg },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sum,
                4000 );
        }

        [Test]
        public void method_trans1()
        {
            svc.xresult = 2;
            Assert.AreEqual( 2, test.trans( E1.A, 5 ) ); // divide by 2
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
                new Field[] { ValueFactoryTest1._mf_e, ValueFactoryTest1._mf_x },
                new Object[] { E1.A, 5 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans,
                4000 );
        }

        [Test]
        public void method_trans2()
        {
            svc.xresult = 10;
            Assert.AreEqual( 10, test.trans( E1.B, 5 ) ); // mul by 2
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
                new Field[] { ValueFactoryTest1._mf_e, ValueFactoryTest1._mf_x },
                new Object[] { E1.B, 5 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans,
                4000 );
        }

        [Test]
        public void method_trans3()
        {
            svc.xresult = 12;
            Assert.AreEqual( 12, test.trans( E1.C, 5 ) ); // add 7
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
                new Field[] { ValueFactoryTest1._mf_e, ValueFactoryTest1._mf_x },
                new Object[] { E1.C, 5 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans,
                4000 );
        }

        [Test]
        public void method_dist1()
        {
            svc.xresult = Math.Sqrt( 3 );
            S1 arg1 =  new S1( 1, 1, 1 );
            S1 arg2 = new S1( 0, 0, 0 );
            Assert.AreEqual( Math.Sqrt( 3 ), test.dist( arg1, arg2 ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist,
                new Field[] { ValueFactoryTest1._mf_a, ValueFactoryTest1._mf_b },
                new Object[] { arg1, arg2 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist,
                4000 );
        }

        [Test]
        public void method_dist2()
        {
            svc.xresult = Math.Sqrt( 35 );
            S1 arg1 = new S1( 1, 2, 3 );
            S1 arg2 = new S1( 6, 5, 4 );
            Assert.AreEqual( Math.Sqrt( 35 ), test.dist( arg1, arg2 ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist,
                new Field[] { ValueFactoryTest1._mf_a, ValueFactoryTest1._mf_b },
                new Object[] { arg1, arg2 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist,
                4000 );
        }

        [Test]
        public void method_dist3()
        {
            svc.xresult = Math.Sqrt( 56 );
            S1 arg1 = new S1( 1, 2, 3 );
            S1 arg2 = new S1( -1, -2, -3 );
            Assert.AreEqual( Math.Sqrt( 56 ), test.dist( arg1, arg2 ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist,
                new Field[] { ValueFactoryTest1._mf_a, ValueFactoryTest1._mf_b },
                new Object[] { arg1, arg2 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist,
                4000 );
        }

        [Test]
        public void method_fill1()
        {
            svc.xresult = new int[] { };
            int[] x = test.fill( 0, 1 );
            Assert.AreEqual( 0, x.Length );
            foreach ( int i in x )
                Assert.AreEqual( 1, i );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill,
                new Field[] { ValueFactoryTest1._mf_n, ValueFactoryTest1._mf_x },
                new Object[] { 0, 1 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill,
                4000 );
        }

        [Test]
        public void method_fill2()
        {
            svc.xresult = new int[] { 2 };
            int[] x = test.fill( 1, 2 );
            Assert.AreEqual( 1, x.Length );
            foreach ( int i in x )
                Assert.AreEqual( 2, i );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill,
                new Field[] { ValueFactoryTest1._mf_n, ValueFactoryTest1._mf_x },
                new Object[] { 1, 2 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill,
                4000 );
        }

        [Test]
        public void method_fill3()
        {
            svc.xresult = new int[] { 3, 3 };
            int[] x = test.fill( 2, 3 );
            Assert.AreEqual( 2, x.Length );
            foreach ( int i in x )
                Assert.AreEqual( 3, i );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill,
                new Field[] { ValueFactoryTest1._mf_n, ValueFactoryTest1._mf_x },
                new Object[] { 2, 3 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill,
                4000 );
        }

        [Test]
        public void method_blow1()
        {
            try
            {
                svc.xresult = new Excp1( "foo", 2 );
                test.blow( "foo", 2 );

            }
            catch ( Excp1 e )
            {
                Assert.AreEqual( "foo", e.msg );
                Assert.AreEqual( 2, e.code );
            }
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_blow,
                new Field[] { ValueFactoryTest1._mf_msg, ValueFactoryTest1._mf_code },
                new Object[] { "foo", 2 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow,
                4000 );
        }

        [Test]
        public void method_blow2()
        {
            try
            {
                svc.xresult = new Excp1( "bar", 3 );
                test.blow( "bar", 3 );
                //fail( "blow did not throw" );
            }
            catch ( Excp1 e )
            {
                Assert.AreEqual( "bar", e.msg );
                Assert.AreEqual( 3, e.code );
            }
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_blow,
                new Field[] { ValueFactoryTest1._mf_msg, ValueFactoryTest1._mf_code },
                new Object[] { "bar", 3 },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow,
                4000 );
        }

        [Test]
        public void method_beets1()
        {
            svc.xresult = 5;
            Assert.AreEqual( 5, test.beets( E1.A ) );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
                new Field[] { ValueFactoryTest1._mf_e },
                new Object[] { E1.A },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets,
                4000 );
        }

        [Test]
        public void method_beets2()
        {
            bool exceptionFlag = false;
            try
            {
                svc.xresult = new Excp3();
                test.beets( E1.B );
            }
            catch ( Excp3 )
            {
                exceptionFlag = true;
            }
            Assert.IsTrue(exceptionFlag);
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
                new Field[] { ValueFactoryTest1._mf_e },
                new Object[] { E1.B },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets,
                4000 );
        }

        [Test]
        public void method_beets3()
        {
            bool exceptionFlag = false;
            try
            {
                svc.xresult = new Excp4();
                test.beets( E1.C );
            }
            catch ( Excp4  )
            {
                exceptionFlag = true;
            }
            Assert.IsTrue( exceptionFlag );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
                new Field[] { ValueFactoryTest1._mf_e },
                new Object[] { E1.C },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets,
                4000 );
        }

        //[Test]
        //public void method_beets4()
        //{
        //    svc.xresult = null;
        //    Assert.IsNull( test.beets( null ) );
        //    check( What.CALL,
        //        ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
        //        new Field[] { ValueFactoryTest1._mf_e },
        //        new Object[] { null },
        //        ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets,
        //        4000 );
        //}

        /////////////
        // bool //
        /////////////

        [Test]
        public void method_p_boolean_null()
        {
            svc.clear();
            bool? a = null;
            svc.xresult = a;
            bool? r = test.p_boolean( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean,
                new Field[] {},
                new Object[] {},
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean,
                4000 );
        }

        [Test]
        public void method_p_boolean()
        {
            bool? a = true;
            svc.xresult = a;
            bool? r = test.p_boolean( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean,
                4000 );
        }

        [Test]
        public void method_p_boolean_array_null()
        {
            bool[] a = null;
            svc.xresult = a;
            bool[] r = test.p_boolean_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean_array,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean_array,
                4000 );
        }

        [Test]
        public void method_p_boolean_array()
        {
            bool x = false;
            bool y = true;
            bool[] a = { x, y };
            svc.xresult = a;
            bool[] r = test.p_boolean_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean_array,
                4000 );
        }

        //////////
        // sbyte //
        //////////

        [Test]
        public void method_p_byte_null()
        {
            sbyte? a = null;
            svc.xresult = a;
            sbyte? r = test.p_byte( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte,
                new Field[] { },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte,
                4000 );
        }

        [Test]
        public void method_p_byte()
        {
            sbyte? a = 1;
            svc.xresult = a;
            sbyte? r = test.p_byte( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte,
                4000 );
        }

        [Test]
        public void method_p_byte_array_null()
        {
            sbyte[] a = null;
            svc.xresult = a;
            sbyte[] r = test.p_byte_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte_array,
                new Field[] {  },
                new Object[] {},
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte_array,
                4000 );
        }

        [Test]
        public void method_p_byte_array()
        {
            sbyte x = 12;
            sbyte y = 23;
            sbyte[] a = { x, y };
            svc.xresult = a;
            sbyte[] r = test.p_byte_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte_array,
                4000 );
        }

        ///////////
        // SHORT //
        ///////////

        [Test]
        public void method_p_short_null()
        {
            short? a = null;
            svc.xresult = a;
            short? r = test.p_short( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short,
                new Field[] {  },
                new Object[] {  },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short,
                4000 );
        }

        [Test]
        public void method_p_short()
        {
            short? a = 1234;
            svc.xresult = a;
            short? r = test.p_short( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short,
                4000 );
        }

        [Test]
        public void method_p_short_array_null()
        {
            short[] a = null;
            svc.xresult = a;
            short[] r = test.p_short_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short_array,
                new Field[] { },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short_array,
                4000 );
        }

        [Test]
        public void method_p_short_array()
        {
            short x = 1234;
            short y = 2345;
            short[] a = { x, y };
            svc.xresult = a;
            short[] r = test.p_short_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short_array,
                4000 );
        }

        /////////
        // INT //
        /////////

        [Test]
        public void method_p_int_null()
        {
            int? a = null;
            svc.xresult = a;
            int? r = test.p_int( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int,
                new Field[] { },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int,
                4000 );
        }

        [Test]
        public void method_p_int()
        {
            int? a = 12345678;
            svc.xresult = a;
            int? r = test.p_int( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int,
                4000 );
        }

        [Test]
        public void method_p_int_array_null()
        {
            int[] a = null;
            svc.xresult = a;
            int[] r = test.p_int_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int_array,
                new Field[] { },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int_array,
                4000 );
        }

        [Test]
        public void method_p_int_array()
        {
            int x = 12345678;
            int y = 23456789;
            int[] a = { x, y };
            svc.xresult = a;
            int[] r = test.p_int_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int_array,
                4000 );
        }

        //////////
        // LONG //
        //////////

        [Test]
        public void method_p_long_null()
        {
            long? a = null;
            svc.xresult = a;
            long? r = test.p_long( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long,
                4000 );
        }

        [Test]
        public void method_p_long()
        {
            long? a = 1234567812345678L;
            svc.xresult = a;
            long? r = test.p_long( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long,
                4000 );
        }

        [Test]
        public void method_p_long_array_null()
        {
            long[] a = null;
            svc.xresult = a;
            long[] r = test.p_long_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long_array,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long_array,
                4000 );
        }

        [Test]
        public void method_p_long_array()
        {
            long x = 1234567812345678L;
            long y = 2345678923456789L;
            long[] a = { x, y };
            svc.xresult = a;
            long[] r = test.p_long_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long_array,
                4000 );
        }

        ///////////
        // float //
        ///////////

        [Test]
        public void method_p_float_null()
        {
            float? a = null;
            svc.xresult = a;
            float? r = test.p_float( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float,
                4000 );
        }

        [Test]
        public void method_p_float()
        {
            float? a = 123.456f;
            svc.xresult = a;
            float? r = test.p_float( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float,
                4000 );
        }

        [Test]
        public void method_p_float_array_null()
        {
            float[] a = null;
            svc.xresult = a;
            float[] r = test.p_float_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float_array,
                new Field[] { },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float_array,
                4000 );
        }

        [Test]
        public void method_p_float_array()
        {
            float x = 123.456f;
            float y = 234.567f;
            float[] a = { x, y };
            svc.xresult = a;
            float[] r = test.p_float_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float_array,
                4000 );
        }

        ////////////
        // DOUBLE //
        ////////////

        [Test]
        public void method_p_double_null()
        {
            double? a = null;
            svc.xresult = a;
            double? r = test.p_double( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double,
                new Field[] { },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double,
                4000 );
        }

        [Test]
        public void method_p_double()
        {
            double? a = 12345678.12345678;
            svc.xresult = a;
            double? r = test.p_double( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double,
                4000 );
        }

        [Test]
        public void method_p_double_array_null()
        {
            double[] a = null;
            svc.xresult = a;
            double[] r = test.p_double_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double_array,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double_array,
                4000 );
        }

        [Test]
        public void method_p_double_array()
        {
            double x = 12345678.12345678;
            double y = 23456789.23456789;
            double[] a = { x, y };
            svc.xresult = a;
            double[] r = test.p_double_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double_array,
                4000 );
        }

        ////////////
        // STRING //
        ////////////

        [Test]
        public void method_p_string_null()
        {
            String a = null;
            svc.xresult = a;
            String r = test.p_string( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string,
                new Field[] { },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string,
                4000 );
        }

        [Test]
        public void method_p_string()
        {
            String a = "abc";
            svc.xresult = a;
            String r = test.p_string( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string,
                4000 );
        }

        [Test]
        public void method_p_string_array_null()
        {
            String[] a = null;
            svc.xresult = a;
            String[] r = test.p_string_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string_array,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array,
                4000 );
        }

        [Test]
        public void method_p_string_array()
        {
            String x = "abc";
            String y = "bcd";
            String[] a = { x, y };
            svc.xresult = a;
            String[] r = test.p_string_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array,
                4000 );
        }

        ////////
        // E1 //
        ////////

        [Test]
        public void method_p_E1_null()
        {
            E1? a = null;
            svc.xresult = a;
            E1? r = test.p_E1( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1,
                new Field[] { },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1,
                4000 );
        }

        [Test]
        public void method_p_E1()
        {
            E1 a = E1.A;
            svc.xresult = a;
            E1? r = test.p_E1( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1,
                4000 );
        }

        [Test]
        public void method_p_E1_array_null()
        {
            E1[] a = null;
            svc.xresult = a;
            E1[] r = test.p_E1_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1_array,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1_array,
                4000 );
        }

        [Test]
        public void method_p_E1_array()
        {
            E1 x = E1.A;
            E1 y = E1.B;
            E1[] a = { x, y };
            svc.xresult = a;
            E1[] r = test.p_E1_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1_array,
                4000 );
        }

        ////////
        // S1 //
        ////////

        [Test]
        public void method_p_S1_null()
        {
            S1 a = null;
            svc.xresult = a;
            S1 r = test.p_S1( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1,
                4000 );
        }

        [Test]
        public void method_p_S1()
        {
            S1 a = new S1( 1, 2, 3 );
            svc.xresult = a;
            S1 r = test.p_S1( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1,
                4000 );
        }

        [Test]
        public void method_p_S1_array_null()
        {
            S1[] a = null;
            svc.xresult = a;
            S1[] r = test.p_S1_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1_array,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1_array,
                4000 );
        }

        [Test]
        public void method_p_S1_array()
        {
            S1 x = new S1( 1, 2, 3 );
            S1 y = new S1( 2, 3, 4 );
            S1[] a = { x, y };
            svc.xresult = a;
            S1[] r = test.p_S1_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1_array,
                4000 );
        }

        ////////
        // S2 //
        ////////

        [Test]
        public void method_p_S2_null()
        {
            S2 a = null;
            svc.xresult = a;
            S2 r = test.p_S2( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2,
                4000 );
        }

        [Test]
        public void method_p_S2()
        {
            S2 a = new S2( new S1( 1, 2, 3 ), new S1( 2, 3, 4 ), E1.A );
            svc.xresult = a;
            S2 r = test.p_S2( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2,
                4000 );
        }

        [Test]
        public void method_p_S2_array_null()
        {
            S2[] a = null;
            svc.xresult = a;
            S2[] r = test.p_S2_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2_array,
                new Field[] {  },
                new Object[] { },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2_array,
                4000 );
        }

        [Test]
        public void method_p_S2_array()
        {
            S2 x = new S2( new S1( 1, 2, 3 ), new S1( 2, 3, 4 ), E1.A );
            S2 y = new S2( new S1( 2, 3, 4 ), new S1( 3, 4, 5 ), E1.B );
            S2[] a = { x, y };
            svc.xresult = a;
            S2[] r = test.p_S2_array( a );
            assertObjectEquals( a, r );
            check( What.CALL,
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2_array,
                new Field[] { ValueFactoryTest1._mf_a },
                new Object[] { a },
                ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2_array,
                4000 );
        }

        ///////////////////
        // UTILITY STUFF //
        ///////////////////

        /**
         * @param a
         * @param b
         */
        private void assertObjectEquals( Object a, Object b )
        {
            if ( a == b )
                return;

            if ( a == null || b == null )
            {
                //fail( "a == null || b == null" );
                Assert.IsTrue( false );
                return;
            }

            Assert.AreSame( a.GetType(), b.GetType() );

            if ( ( a is Array ) && ( b is Array ) )
            {
                assertArrayEquals( a, b );
                return;
            }

            Assert.AreEqual( a, b );
        }

        /**
         * @param a
         * @param b
         */
        private void assertArrayEquals( Object a, Object b )
        {
            int n = ( ( Array ) a ).Length;
            Assert.AreEqual( n, ( ( Array ) b ).Length );
            for ( int i = 0; i < n; i++ )
                assertObjectEquals( ( ( Array ) a ).GetValue( i ), ( ( Array ) b ).GetValue( i ) );
        }

        private void check( What what, XType type, Field[] fields, Object[] objects,
            XType resultType, int timeout )
        {
            Assert.AreEqual( what, svc.what );
            Assert.AreEqual( type, svc.xmsg.GetXType );

            int n = fields.Length;
            Assert.AreEqual( n, objects.Length );

            //Console.WriteLine( "size = " + svc.xmsg.Count );
            Assert.AreEqual( n, svc.xmsg.Count );

            for ( int i = 0; i < n; i++ )
            {
                Field f = fields[ i ];
                Assert.AreEqual( objects[ i ], svc.xmsg.Get( f ) );
            }

            Assert.AreEqual( resultType, svc.xresponseType );
            Assert.AreEqual( ValueFactoryTest1._mf_result, svc.xresponseField );
            Assert.AreEqual(  timeout, svc.xtimeout );
        }

        public enum What { SEND, CALL, SHUTDOWNOUTPUT, BEGINCALL }

        private MyDeliveryService svc;
        private org.apache.etch.tests.Test1 test;

        public  class MyDeliveryService : DeliveryService,MailboxManager
        {
            public Enum what;
            public Message xmsg;
            public XType xresponseType;
            public Field xresponseField;
            public int? xtimeout;
            public Object xresult;
            public Mailbox xmb;

            public void clear()
            {
                what = null;
                xmsg = null;
                xresponseType = null;
                xresponseField = null;
                xtimeout = null;
                xresult = null;
            }

            public void Send( Message msg )
            {
                Assert.IsNull( what );
                what = What.SEND;
                xmsg = msg;
            }

            public Mailbox BeginCall( Message msg )
            {
                Assert.IsNull( what );
                what = What.BEGINCALL;
                xmsg = msg;
                xmb = new PlainMailbox( this, 0L );
                return xmb;
            }

            public Object EndCall( Mailbox mb, XType responseType )
            {
                Assert.AreEqual( What.BEGINCALL, what );
                Assert.AreEqual( xmb, mb );
                what = What.CALL;
                xmb = null;
                xresponseType = responseType;
                xresponseField = responseType.ResponseField;
                xtimeout = responseType.Timeout;
                if ( xresult is Exception )
                    throw ( Exception ) xresult;
                return xresult;
            }

            #region Transport Members

            public object TransportQuery( object query )
            {
                // ignore
                return null;
            }

            public void TransportControl( object control, object value )
            {
                //ignore
            }

            public void TransportNotify( object eventObj )
            {
                //ignore
            }

            #endregion

            #region DeliveryService Members

        
            #endregion

            public void Redeliver( Who sender, Message msg ) 
		    {
			// ignore
		    }

		    public void Unregister( Mailbox mb )
		    {
			// ignore
		    }


            #region TransportMessage Members

            public void TransportMessage(Who recipient, Message msg)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region Transport<SessionMessage> Members


            public void SetSession(SessionMessage session)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public SessionMessage GetSession()
            {
                throw new Exception("The method or operation is not implemented.");
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

            #region MailboxManager Members


            public Mailbox TransportCall(Who recipient, Message msg)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion
        }
    }
}
