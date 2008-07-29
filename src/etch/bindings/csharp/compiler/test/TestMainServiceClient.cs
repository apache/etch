using System;
using System.Collections.Generic;
using System.Text;
using Etch.Util;
using etch.bindings.csharp.compiler.test;
using Etch.Transport;
using NUnit.Framework;
using System.Threading;

namespace Etch.Compiler.Test
{
    public class TestMainServiceClient
    {
        // Settings
        //--------------------------------------------------------------------------

        // Instance of the class.
        private ImplTestClient myMainTestClient;

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
        private ConstsTest.E1? myE1;
        private ConstsTest.S1 myS1;
        private ConstsTest.S2 myS2;
        private ConstsTest.S3 myS3;
        private ConstsTest.S4 myS4;
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
        private ConstsTest.E1[] myE1s;
        private ConstsTest.S1[] myS1s;
        private ConstsTest.S2[] myS2s;

        // Maintenance
	    // -------------------------------------------------------------------------

	    [SetUp]
	    public void SetUp()
	    {
		    this.myMainTestClient = new ImplTestClient( null );
	    }


	    // Tests
	    //--------------------------------------------------------------------------

	    [Test]
	    public void NothingMethod()
	    {
		    this.myMainTestClient.nothing();
	    }

	    [Test]
	    [ExpectedException(typeof(Exception))]
	    public void IncrMethod()
	    {
		    this.myInt = this.myMainTestClient.incr(this.myInt);
	    }

	    [Test]
        [ExpectedException( typeof( Exception ) )]
	    public void addMethod()
	    {
		    this.myInt = this.myMainTestClient.add(this.myInt, this.myInt);
	    }

        [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void subMethod()
	    {
		    this.myInt = this.myMainTestClient.sub(this.myInt, this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void sumMethod()
	    {
		    this.myInt = this.myMainTestClient.sum(this.myInts);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void transMethod()
	    {
		    this.myInt = this.myMainTestClient.trans(this.myE1, this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void distMethod()
	    {
		    this.myDouble = this.myMainTestClient.dist(this.myS1, this.myS1);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void fillMethod()
	    {
		    this.myInts = this.myMainTestClient.fill(this.myInt, this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void fillObjectMethod()
	    {
		    this.myObjs = this.myMainTestClient.fillObject(this.myInt, this.myObj);
	    }

	    [Test]
	    public void blowMethod()
	    {
		    this.myMainTestClient.blow(this.myString, this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void beetsMethod()
	    {
		    this.myInt = this.myMainTestClient.beets(this.myE1);
	    }

	    [Test]
	    public void throwExcp5Method()
	    {
		    this.myMainTestClient.throwExcp5(this.myString, this.myInt, this.myObj);
	    }

	    [Test]
	    public void throwExcp6Method()
	    {
		    this.myMainTestClient.throwExcp6(this.myString, this.myInt, this.myObjs);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_booleanMethod()
	    {
		    this.myBool = this.myMainTestClient.p_boolean(this.myBool);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_boolean_arrayMethod()
	    {
		    this.myBools = this.myMainTestClient.p_boolean_array(this.myBools);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_byteMethod()
	    {
		    this.myByte = this.myMainTestClient.p_byte(this.myByte);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_byte_arrayMethod()
	    {
		    this.myBytes = this.myMainTestClient.p_byte_array(this.myBytes);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_shortMethod()
	    {
		    this.myShort = this.myMainTestClient.p_short(this.myShort);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_short_arrayMethod()
	    {
		    this.myShorts = this.myMainTestClient.p_short_array(this.myShorts);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_intMethod()
	    {
		    this.myInt = this.myMainTestClient.p_int(this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_int_arrayMethod()
	    {
		    this.myInts = this.myMainTestClient.p_int_array(this.myInts);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_longMethod()
	    {
		    this.myLong = this.myMainTestClient.p_long(this.myLong);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_long_arrayMethod()
	    {
		    this.myLongs = this.myMainTestClient.p_long_array(this.myLongs);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_floatMethod()
	    {
		    this.myFloat = this.myMainTestClient.p_float(this.myFloat);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_float_arrayMethod()
	    {
		    this.myFloats = this.myMainTestClient.p_float_array(this.myFloats);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_doubleMethod()
	    {
		    this.myDouble = this.myMainTestClient.p_double(this.myDouble);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_double_arrayMethod()
	    {
		    this.myDoubles = this.myMainTestClient.p_double_array(this.myDoubles);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_stringMethod()
	    {
		    this.myString = this.myMainTestClient.p_string(this.myString);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_string_arrayMethod()
	    {
		    this.myStrings = this.myMainTestClient.p_string_array(this.myStrings);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_E1Method()
	    {
		    this.myE1 = this.myMainTestClient.p_E1(this.myE1);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_E1_arrayMethod()
	    {
		    this.myE1s = this.myMainTestClient.p_E1_array(this.myE1s);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_S1Method()
	    {
		     this.myS1 = this.myMainTestClient.p_S1(this.myS1);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_S1_arrayMethod()
	    {
		     this.myS1s = this.myMainTestClient.p_S1_array(this.myS1s);

	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_S2Method()
	    {
		    this.myS2 = this.myMainTestClient.p_S2(this.myS2);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_S2_arrayMethod()
	    {
		    this.myS2s = this.myMainTestClient.p_S2_array(this.myS2s);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_BlobMethod()
	    {
		    this.myMap = this.myMainTestClient.p_Blob(this.myMap);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_Blob_ArrayMethod()
	    {
		    this.myMaps = this.myMainTestClient.p_Blob_array(this.myMaps);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_objectMethod()
	    {
		    this.myObj = this.myMainTestClient.p_object(this.myObj);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_object_arrayMethod()
	    {
		    this.myMainTestClient.p_object_array(null);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_object_structMethod()
	    {
		    this.myS3 = this.myMainTestClient.p_object_struct(this.myS3);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void p_object_struct_arrayMethod()
	    {
		    this.myS4 = this.myMainTestClient.p_object_struct_array(this.myS4);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void alwaysWordsMethod()
	    {
		    this.myInt = this.myMainTestClient.alwaysWorks();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void isTrueMethod()
	    {
		    this.myBool = this.myMainTestClient.isTrue();
	    }

	    [Test]
	    public void alwaysFailsMethod()
	    {
		    this.myMainTestClient.alwaysFails();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void isFalseMethod()
	    {
		    this.myBool = this.myMainTestClient.isFalse();
	    }

	    [Test]
	    public void m1Method()
	    {
		    this.myMainTestClient.m1();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m1Method()
	    {
		    this.myBool = this.myMainTestClient.can_m1();
	    }

	    [Test]
	    public void m2Method()
	    {
		    this.myMainTestClient.m2();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m2Method()
	    {
		    this.myBool = this.myMainTestClient.can_m2(null);
	    }

	    [Test]
	    public void m3Method()
	    {
		    this.myMainTestClient.m3();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m3Method()
	    {
		    this.myBool = this.myMainTestClient.can_m3(null);
	    }

	    [Test]
	    public void m4Method()
	    {
		    this.myMainTestClient.m4();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m4Method()
	    {
		    this.myBool = this.myMainTestClient.can_m4(this.myBool);
	    }

	    [Test]
	    public void m5Method()
	    {
		    this.myMainTestClient.m5();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m5Method()
	    {
		    this.myBool = this.myMainTestClient.can_m5(this.myByte);
	    }

	    [Test]
	    public void m7Method()
	    {
		    this.myMainTestClient.m7();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m7Method()
	    {
		    this.myBool = this.myMainTestClient.can_m7(this.myInt);
	    }

	    [Test]
	    public void m8Method()
	    {
		    this.myMainTestClient.m8();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m8Method()
	    {
		    this.myBool = this.myMainTestClient.can_m8(this.myLong);
	    }

	    [Test]
	    public void m9Method()
	    {
		    this.myMainTestClient.m9();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m9Method()
	    {
		    this.myBool = this.myMainTestClient.can_m9(this.myFloat);
	    }

	    [Test]
	    public void m10Method()
	    {
		    this.myMainTestClient.m10();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m10Method()
	    {
		    this.myBool = this.myMainTestClient.can_m10(this.myDouble);
	    }

	    [Test]
	    public void m11Method()
	    {
		    this.myMainTestClient.m11();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m11Method()
	    {
		    this.myBool = this.myMainTestClient.can_m11(this.myString);
	    }

	    [Test]
	    public void m12Method()
	    {
		    this.myMainTestClient.m12();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m12Method()
	    {
		    this.myBool = this.myMainTestClient.can_m12(this.myInt);
	    }

	    [Test]
	    public void m13Method()
	    {
		    this.myMainTestClient.m13(this.myInt);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m13Method()
	    {
		    this.myBool = this.myMainTestClient.can_m13(this.myInt);
	    }

	    [Test]
	    public void m14Method()
	    {
		    this.myMainTestClient.m14(this.myS1);
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m14Method()
	    {
		    this.myBool = this.myMainTestClient.can_m14(this.myInt);
	    }

	    [Test]
	    public void m15Method()
	    {
		    this.myMainTestClient.m15();
	    }

	    [Test]
	    [ExpectedException( typeof( Exception ) )]
	    public void can_m15Method()
	    {
		    this.myBool = this.myMainTestClient.can_m15(this.myE1);
	    }

        [Test]
        public void MainServiceListenerAndClient()
        {
            ThreadClient client = new ThreadClient();
            ThreadListener listener = new ThreadListener();

            // start the listener & wait until it has started
            ( new Thread( new ThreadStart( listener.Run ) ) ).Start();
            commonLock.WaitUntilEq( LISTENER_STARTED );

            // start the client
            ( new Thread( new ThreadStart( client.Run ) ) ).Start();
            commonLock.WaitUntilEq( CLIENT_STARTED );

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
                    MainTestClient.Main2( null );
                }
                catch ( Exception e )
                {
                    Console.WriteLine( e.StackTrace );
                    myHasErrors = true;
                }
                TestMainServiceClient.commonLock.Set( TestMainServiceClient.CLIENT_STARTED );
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
                    MainTestListener.Main1( null );
                }
                catch ( Exception e )
                {
                    Console.WriteLine( e.StackTrace );
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
                    ( new Thread( new ThreadStart( listener.Run ) ) ).Start();
                    TestMainServiceClient.commonLock.Set( TestMainServiceClient.LISTENER_STARTED );
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

        public readonly static string LISTENER_STARTED = "Listener_Started";
        public readonly static string CLIENT_STARTED = "Client_Started";
    }
}
