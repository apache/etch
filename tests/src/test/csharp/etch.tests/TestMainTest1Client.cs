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
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Test1;

namespace etch.tests
{
    [TestFixture]
    public class TestMainTest1Client
    {
        // Settings
        //--------------------------------------------------------------------------

        // Instance of the class.
        private ImplTest1Client myMainTestClient;

        // Instances of arguments types
        private bool? myBool;
        private sbyte? myByte;
        private short? myShort;
        private int? myInt;
        private long? myLong;
        private float? myFloat;
        private double? myDouble;
        private Object myObj;
        private String myString;
        private StrIntHashMap myMap;
        private E1? myE1;
        private S1 myS1;
        private S2 myS2;
        private S3 myS3;
        private S4 myS4;
        private Boolean[] myBools;
        private sbyte[] myBytes;
        private short[] myShorts;
        private int[] myInts;
        private long[] myLongs;
        private float[] myFloats;
        private double[] myDoubles;
        private Object[] myObjs;
        private String[] myStrings;
        private StrIntHashMap[] myMaps;
        private E1[] myE1s;
        private S1[] myS1s;
        private S2[] myS2s;

        // Maintenance
	    // -------------------------------------------------------------------------

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestMain" );
        }

	    [SetUp]
	    public void SetUp()
	    {
            this.myMainTestClient = new ImplTest1Client( null);
	    }


	    // Tests
	    //--------------------------------------------------------------------------

	    [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void NothingMethod()
	    {
		    this.myMainTestClient.nothing();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void IncrMethod()
	    {
		    this.myInt = this.myMainTestClient.incr(this.myInt);
	    }

	    [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void addMethod()
	    {
		    this.myInt = this.myMainTestClient.add(this.myInt, this.myInt);
	    }

        [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void subMethod()
	    {
		    this.myInt = this.myMainTestClient.sub(this.myInt, this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void sumMethod()
	    {
		    this.myInt = this.myMainTestClient.sum(this.myInts);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void transMethod()
	    {
		    this.myInt = this.myMainTestClient.trans(this.myE1, this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void distMethod()
	    {
		    this.myDouble = this.myMainTestClient.dist(this.myS1, this.myS1);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void fillMethod()
	    {
		    this.myInts = this.myMainTestClient.fill(this.myInt, this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void fillObjectMethod()
	    {
		    this.myObjs = this.myMainTestClient.fillObject(this.myInt, this.myObj);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void blowMethod()
	    {
		    this.myMainTestClient.blow(this.myString, this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void beetsMethod()
	    {
		    this.myInt = this.myMainTestClient.beets(this.myE1);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void throwExcp5Method()
	    {
		    this.myMainTestClient.throwExcp5(this.myString, this.myInt, this.myObj);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void throwExcp6Method()
	    {
		    this.myMainTestClient.throwExcp6(this.myString, this.myInt, this.myObjs);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_booleanMethod()
	    {
		    this.myBool = this.myMainTestClient.p_boolean(this.myBool);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_boolean_arrayMethod()
	    {
		    this.myBools = this.myMainTestClient.p_boolean_array(this.myBools);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_byteMethod()
	    {
		    this.myByte = this.myMainTestClient.p_byte(this.myByte);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_byte_arrayMethod()
	    {
		    this.myBytes = this.myMainTestClient.p_byte_array(this.myBytes);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_shortMethod()
	    {
		    this.myShort = this.myMainTestClient.p_short(this.myShort);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_short_arrayMethod()
	    {
		    this.myShorts = this.myMainTestClient.p_short_array(this.myShorts);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_intMethod()
	    {
		    this.myInt = this.myMainTestClient.p_int(this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_int_arrayMethod()
	    {
		    this.myInts = this.myMainTestClient.p_int_array(this.myInts);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_longMethod()
	    {
		    this.myLong = this.myMainTestClient.p_long(this.myLong);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_long_arrayMethod()
	    {
		    this.myLongs = this.myMainTestClient.p_long_array(this.myLongs);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_floatMethod()
	    {
		    this.myFloat = this.myMainTestClient.p_float(this.myFloat);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_float_arrayMethod()
	    {
		    this.myFloats = this.myMainTestClient.p_float_array(this.myFloats);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_doubleMethod()
	    {
		    this.myDouble = this.myMainTestClient.p_double(this.myDouble);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_double_arrayMethod()
	    {
		    this.myDoubles = this.myMainTestClient.p_double_array(this.myDoubles);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_stringMethod()
	    {
		    this.myString = this.myMainTestClient.p_string(this.myString);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_string_arrayMethod()
	    {
		    this.myStrings = this.myMainTestClient.p_string_array(this.myStrings);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_E1Method()
	    {
		    this.myE1 = this.myMainTestClient.p_E1(this.myE1);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_E1_arrayMethod()
	    {
		    this.myE1s = this.myMainTestClient.p_E1_array(this.myE1s);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_S1Method()
	    {
		     this.myS1 = this.myMainTestClient.p_S1(this.myS1);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_S1_arrayMethod()
	    {
		     this.myS1s = this.myMainTestClient.p_S1_array(this.myS1s);

	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_S2Method()
	    {
		    this.myS2 = this.myMainTestClient.p_S2(this.myS2);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_S2_arrayMethod()
	    {
		    this.myS2s = this.myMainTestClient.p_S2_array(this.myS2s);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_BlobMethod()
	    {
		    this.myMap = this.myMainTestClient.p_Blob(this.myMap);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_Blob_ArrayMethod()
	    {
		    this.myMaps = this.myMainTestClient.p_Blob_array(this.myMaps);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_objectMethod()
	    {
		    this.myObj = this.myMainTestClient.p_object(this.myObj);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_object_arrayMethod()
	    {
		    this.myMainTestClient.p_object_array(null);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_object_structMethod()
	    {
		    this.myS3 = this.myMainTestClient.p_object_struct(this.myS3);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void p_object_struct_arrayMethod()
	    {
		    this.myS4 = this.myMainTestClient.p_object_struct_array(this.myS4);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void alwaysWordsMethod()
	    {
		    this.myInt = this.myMainTestClient.alwaysWorks();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void isTrueMethod()
	    {
		    this.myBool = this.myMainTestClient.isTrue();
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void alwaysFailsMethod()
	    {
		    this.myMainTestClient.alwaysFails();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void isFalseMethod()
	    {
		    this.myBool = this.myMainTestClient.isFalse();
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m1Method()
	    {
		    this.myMainTestClient.m1();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m1Method()
	    {
		    this.myBool = this.myMainTestClient.can_m1();
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m2Method()
	    {
		    this.myMainTestClient.m2();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m2Method()
	    {
		    this.myBool = this.myMainTestClient.can_m2(null);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m3Method()
	    {
		    this.myMainTestClient.m3();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m3Method()
	    {
		    this.myBool = this.myMainTestClient.can_m3(null);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m4Method()
	    {
		    this.myMainTestClient.m4();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m4Method()
	    {
		    this.myBool = this.myMainTestClient.can_m4(this.myBool);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m5Method()
	    {
		    this.myMainTestClient.m5();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m5Method()
	    {
		    this.myBool = this.myMainTestClient.can_m5(this.myByte);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m7Method()
	    {
		    this.myMainTestClient.m7();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m7Method()
	    {
		    this.myBool = this.myMainTestClient.can_m7(this.myInt);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m8Method()
	    {
		    this.myMainTestClient.m8();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m8Method()
	    {
		    this.myBool = this.myMainTestClient.can_m8(this.myLong);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m9Method()
	    {
		    this.myMainTestClient.m9();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m9Method()
	    {
		    this.myBool = this.myMainTestClient.can_m9(this.myFloat);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m10Method()
	    {
		    this.myMainTestClient.m10();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m10Method()
	    {
		    this.myBool = this.myMainTestClient.can_m10(this.myDouble);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m11Method()
	    {
		    this.myMainTestClient.m11();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m11Method()
	    {
		    this.myBool = this.myMainTestClient.can_m11(this.myString);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m12Method()
	    {
		    this.myMainTestClient.m12();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m12Method()
	    {
		    this.myBool = this.myMainTestClient.can_m12(this.myInt);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m13Method()
	    {
		    this.myMainTestClient.m13(this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m13Method()
	    {
		    this.myBool = this.myMainTestClient.can_m13(this.myInt);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m14Method()
	    {
		    this.myMainTestClient.m14(this.myS1);
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m14Method()
	    {
		    this.myBool = this.myMainTestClient.can_m14(this.myInt);
	    }

        [Test]
        [ExpectedException( typeof( NotSupportedException ) )]
	    public void m15Method()
	    {
		    this.myMainTestClient.m15();
	    }

	    [Test]
	    [ExpectedException( typeof( NotSupportedException ) )]
	    public void can_m15Method()
	    {
		    this.myBool = this.myMainTestClient.can_m15(this.myE1);
	    }

        //[Test]
        public void MainServiceListenerAndClient()
        {
            ThreadClient client = new ThreadClient();
            ThreadListener listener = new ThreadListener();

            // start the listener & wait until it has started
            ( new Thread( new ThreadStart( listener.Run ) ) ).Start();
            commonLock.WaitUntilEq( LISTENER_STARTED, 4000 );

            // start the client
            ( new Thread( new ThreadStart( client.Run ) ) ).Start();
            commonLock.WaitUntilEq( CLIENT_STARTED, 4000 );

            Console.WriteLine( "am i coming here?" );

            if ( client.HasErrors() || listener.HasErrors() )
                Assert.Fail();
        }

        public static Monitor<string> commonLock = new Monitor<string>( "common lock", null );

        private class ThreadClient
        {
            /// <summary>
            /// Record when an error occurs with client
            /// </summary>
            private Boolean myHasErrors;

            public ThreadClient()
            {
                myHasErrors = false;
            }

            public void Run()
            {
                try
                {
                    MainTest1Client.Main( null );
                }
                catch ( Exception e )
                {
                    Console.WriteLine( e );
                    myHasErrors = true;
                }
                TestMainTest1Client.commonLock.Set( TestMainTest1Client.CLIENT_STARTED );

            }

            public Boolean HasErrors()
            {
                return this.myHasErrors;
            }
        } 

        private class SimpleThreadListener
        {
            private Boolean myHasErrors;

            public SimpleThreadListener()
            {
                myHasErrors = false;
            }

            public void Run()
            {
                try
                {
   //                 MainTest1Listener.Main( null );  
                }
                catch ( Exception e )
                {
                    Console.WriteLine( e );
                    myHasErrors = true;
                }
            }

            public Boolean HasErrors()
            {
                return this.myHasErrors;
            }
        }

        private class ThreadListener
        {
            private Boolean myHasErrors;

            public ThreadListener()
            {
                myHasErrors = false;
            }

            public void Run()
            {
                SimpleThreadListener listener = new SimpleThreadListener();

                try
                {
                    // start listener
                    Thread t = ( new Thread( new ThreadStart( listener.Run ) ) );
                    t.Start();
                    TestMainTest1Client.commonLock.Set( TestMainTest1Client.LISTENER_STARTED );
                }
                catch ( Exception )
                {
                    // do nothing
                }

            }


            public Boolean HasErrors()
            {
                return this.myHasErrors;
            }
        }

        public const string LISTENER_STARTED = "Listener_Started";
        public const string CLIENT_STARTED = "Client_Started";
    }
}
