// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;


using NUnit.Framework;
using etch.examples.types.ComplexData;

///<summary>Your custom implementation of BaseComplexDataServer. Add methods here to provide
///implementation of messages from the client. </summary>
namespace etch.examples
{
	///<summary>Implementation for ImplComplexDataServer</summary>
	public class ImplComplexDataServer : BaseComplexDataServer
	{
        private int receiveObjectArrayCounter;
        private int receiveBaseArrayCounter;
        private int receiveDerivedArrayCounter;
        private int receiveListCounter;

        private int sendStringCounter;
        private int sendBooleanCounter;
        private int sendByteCounter;
        private int sendIntCounter;
        private int sendLongCounter;
        private int sendDoubleCounter;
        private int sendFloatCounter;
        private int sendDatetimeCounter;
        private int sendShortCounter;
        private int sendObjectCounter;
        private int sendStringArrayCounter;
        private int sendObjectArrayCounter;
        private int sendBaseArrayCounter;
        private int sendDerivedArrayCounter;
        private int sendListCounter;
        private int sendExceptionCounter;

		/// <summary>Constructs the ImplComplexDataServer.</summary>
 		/// <param name="client">a connection to the client session. Use this to
 		/// send a message to the client.</param>
		public ImplComplexDataServer(RemoteComplexDataClient client)
		{
			this.client = client;

            this.receiveObjectArrayCounter = 0;
            this.receiveBaseArrayCounter = 0;
            this.receiveDerivedArrayCounter = 0;
            this.receiveListCounter = 0;

            this.sendStringCounter = 0;
            this.sendBooleanCounter = 0;
            this.sendByteCounter = 0;
            this.sendIntCounter = 0;
            this.sendLongCounter = 0;
            this.sendDoubleCounter = 0;
            this.sendFloatCounter = 0;
            this.sendDatetimeCounter = 0;
            this.sendShortCounter = 0;
            this.sendStringArrayCounter = 0;
            this.sendObjectCounter = 0;
            this.sendObjectArrayCounter = 0;
            this.sendBaseArrayCounter = 0;
            this.sendDerivedArrayCounter = 0;
            this.sendListCounter = 0;
            this.sendExceptionCounter = 0;
		}
		
		/// <summary>A connection to the client session. Use this to
 		/// send a message to the client.</summary>
		private readonly RemoteComplexDataClient client;

        public override bool? receiveBoolean()
        {
            return true;
        }

        public override sbyte? receiveByte()
        {
            return (sbyte)55;
        }

        public override double? receiveDouble()
        {
            return Double.MaxValue;
        }

        public override float? receiveFloat()
        {
            return float.MaxValue;
        }

        public override int? receiveInt()
        {
            return int.MaxValue;
        }

        public override long? receiveLong()
        {
            return long.MaxValue;
        }

        public override object receiveObject()
        {
//            return new BaseC[] { new BaseC( 1, 2 ) };
            return new BaseC( 1, 2 ) ;
        }

        public override short? receiveShort()
        {
            return short.MaxValue;
        }

        public override string receiveString()
        {
            return "test";
        }

        public override string[] receiveStringArray()
        {
            return new string[] { "one", "two", "three" };
        }

        public override object[] receiveObjectArray()
        {
            receiveObjectArrayCounter++;

            switch( receiveObjectArrayCounter )
            {
                case 1:
                    return null;

                case 2:
                    return new object[] { null } ;

                case 3:
                    return new object[] { 2  };

                case 4:
                    return new object[] { 1 };

                case 5:
                    return new object[] { 1, true, "hi" };

                case 6:
                    return new object[] { 1, false, new BaseA( 1 ), new BaseB( 2, 3 ), new BaseC( 4, 5 ) };

                case 7:
                    return new object[] { 1, new string[] { "one", "two", "three" }, new bool[] { true, false, true } };

                case 8:
                    return new object[0];

            }

            Assert.Fail( "No case for: " + receiveObjectArrayCounter );
            return null;            
        }

        public override BaseA[] receiveBaseArray()
        {
            receiveBaseArrayCounter++;

            switch( receiveBaseArrayCounter )
            {
                case 1:

                    return null;

                case 2:
                    return new BaseA[0];

                case 3:
                    return new BaseA[] { new BaseA( 0 ), new BaseA( 1 ), new BaseA( null ) };

                case 4:
                    return new BaseA[] { new BaseA( 0 ), new BaseB( 1, 2 ), new BaseC( 3, 4 ) };
            }

            Assert.Fail( "No case for: " + receiveBaseArrayCounter );
            return null;
        }

        public override BaseB[] receiveDerivedArray()
        {
            receiveDerivedArrayCounter++;

            switch( receiveDerivedArrayCounter )
            {
                case 1:

                    return null;

                case 2:
                    return new BaseB[0];

                case 3:
                    return new BaseB[] { new BaseB( 0, 1 ), new BaseB( 2, 3), new BaseB( null, null) };
            }

            Assert.Fail( "No case for: " + receiveDerivedArrayCounter );
            return null;
        }

        public override System.Collections.IList receiveList()
        {
            receiveListCounter++;

            switch( receiveListCounter )
            {
                case 1:
                    return null;

                case 2:
                    return new object[] { null };

                case 3:
                    return new object[] { 2 };

                case 4:
                    return new object[] { 1 };

                case 5:
                    return new object[] { 1, true, "hi" };

                case 6:
                    return new object[] { 1, false, new BaseA( 1 ), new BaseB( 2, 3 ), new BaseC( 4, 5 ) };

                case 7:
                    return new object[] { 1, new string[] { "one", "two", "three" }, new bool[] { true, false, true } };

                case 8:
                    return new object[0];

            }

            Assert.Fail( "No case for: " + receiveListCounter );
            return null;            
        }

        public override DateTime? receiveDatetime()
        {
            return DateTime.MaxValue;
        }

        public override System.Collections.IDictionary receiveMap()
        {
            System.Collections.Hashtable hash = new System.Collections.Hashtable();
            hash["1"] = "2";
            hash[1] = 2;

            return hash;
        }

        public override void sendBoolean( bool? value )
        {
            sendBooleanCounter++;

            switch( sendBooleanCounter )
            {
                case 1:
                    Assert.IsTrue( (bool) value );
                    break;

                case 2:
                    Assert.IsFalse( (bool) value );
                    break;

                case 3:
                    Assert.IsNull( value );
                    break;
            }
        }

        public override void sendByte( sbyte? value )
        {
            sendByteCounter++;

            switch( sendByteCounter )
            {
                case 1:
                    Assert.AreEqual( 0, value );
                    break;

                case 2:
                    Assert.AreEqual( 1, value );
                    break;

                case 3:
                    Assert.AreEqual( -1, value );
                    break;

                case 4:
                    Assert.AreEqual( sbyte.MaxValue, value );
                    break;

                case 5:
                    Assert.AreEqual( sbyte.MinValue, value );
                    break;

                case 6:
                    Assert.IsNull( value );
                    break;
            }
        }

        public override void sendInt( int? value )
        {
            sendIntCounter++;

            switch( sendIntCounter )
            {
                case 1:
                    Assert.AreEqual( 0, value );
                    break;

                case 2:
                    Assert.AreEqual( 1, value );
                    break;

                case 3:
                    Assert.AreEqual( -1, value );
                    break;

                case 4:
                    Assert.AreEqual( int.MaxValue, value );
                    break;

                case 5:
                    Assert.AreEqual( int.MinValue, value );
                    break;

                case 6:
                    Assert.IsNull( value );
                    break;
            }
        }

        public override void sendLong( long? value )
        {
            sendLongCounter++;

            switch( sendLongCounter )
            {
                case 1:
                    Assert.AreEqual( 0, value );
                    break;

                case 2:
                    Assert.AreEqual( 1, value );
                    break;

                case 3:
                    Assert.AreEqual( -1, value );
                    break;

                case 4:
                    Assert.AreEqual( 100000000000000, value );
                    break;

                case 5:
                    Assert.AreEqual( -100000000000000, value );
                    break;

                case 6:
                    Assert.IsNull( value );
                    break;
            }
        }


        public override void sendDouble( double? value )
        {
            sendDoubleCounter++;

            switch( sendDoubleCounter )
            {
                case 1:
                    Assert.AreEqual( 0.0, value );
                    break;

                case 2:
                    Assert.AreEqual( 1.1, value );
                    break;

                case 3:
                    Assert.AreEqual( -1.1, value );
                    break;

                case 4:
                    Assert.AreEqual( Double.MaxValue, value );
                    break;

                case 5:
                    Assert.AreEqual( Double.MinValue, value );
                    break;

                case 6:
                    Assert.IsNull( value );
                    break;

                case 7:
                    Assert.AreEqual( Double.PositiveInfinity, value );
                    break;

                case 8:
                    Assert.AreEqual( Double.NegativeInfinity, value );
                    break;
            }
        }

        public override void sendFloat( float? value )
        {
            sendFloatCounter++;

            switch( sendFloatCounter )
            {
                case 1:
                    Assert.AreEqual( 0.0, value );
                    break;

                case 2:
                    Assert.AreEqual( 1.1, value );
                    break;

                case 3:
                    Assert.AreEqual( -1.1, value );
                    break;

                case 4:
                    Assert.AreEqual( float.MaxValue, value );
                    break;

                case 5:
                    Assert.AreEqual( float.MinValue, value );
                    break;

                case 6:
                    Assert.IsNull( value );
                    break;

                case 7:
                    Assert.AreEqual( float.PositiveInfinity, value );
                    break;

                case 8:
                    Assert.AreEqual( float.NegativeInfinity, value );
                    break;
            }
        }

        public override void sendString( string value )
        {
            sendStringCounter++;

            switch( sendStringCounter )
            {
                case 1:
                    Assert.AreEqual( String.Empty, value );
                    break;

                case 2:
                    Assert.AreEqual( String.Empty, value );
                    break;

                case 3:
                    Assert.AreEqual( "a", value );
                    break;

                case 4:
                    Assert.AreEqual( "\u0009", value );
                    break;

                case 5:
                    Assert.AreEqual( "\u00A9", value );
                    break;

                case 6:
                    Assert.IsNull( value );
                    break;
            }
        }

        public override void sendStringArray( string[] values )
        {
            sendStringArrayCounter++;

            switch( sendStringArrayCounter )
            {
                case 1:
                    Assert.AreEqual(1, values.Length);
                    Assert.AreEqual("one", values[0]);
                    break;

                case 2:
                    Assert.AreEqual(2, values.Length);
                    Assert.AreEqual( "one", values[0] );
                    Assert.AreEqual( "two", values[1] );
                    break;

                case 3:
                    Assert.AreEqual(3, values.Length);
                    Assert.AreEqual( "one", values[0] );
                    Assert.AreEqual( "two", values[1] );
                    Assert.AreEqual( "three", values[2] );
                    break;

                case 4:
                    Assert.IsNull( values );
                    break;

                case 5:
                    Assert.AreEqual( 0, values.Length );
                    break;
            }
        }

        public override void sendObjectArray( object[] values )
        {
            sendObjectArrayCounter++;

            switch( sendObjectArrayCounter )
            {
                case 1:
                    Assert.AreEqual(3, values.Length);
                    Assert.AreEqual( 1.1, values[0] );
                    Assert.AreEqual( "one", values[1] );
                    Assert.AreEqual( true, values[2] );
                    break;

                case 2:
                    Assert.AreEqual( 3, values.Length );

                    // the first argument should be a string[]
                    Assert.AreEqual( typeof( string[] ), values[0].GetType() );
                    string[] value1 = values[0] as string[];
                    Assert.AreEqual( 3, value1.Length );
                    Assert.AreEqual( "one", value1[0] );
                    Assert.AreEqual( "two", value1[1] );
                    Assert.AreEqual( "three", value1[2] );

                    Assert.AreEqual( "one", values[1] );
                    Assert.AreEqual( true, values[2] );
                    break;

                case 3:
                    Assert.AreEqual( 3, values.Length );
                    Assert.AreEqual( typeof( object[] ), values.GetType() );
                    Assert.AreEqual( "one", values[0] );
                    Assert.AreEqual( "two", values[1] );
                    Assert.AreEqual( "three", values[2] );
                    break;

                case 4:
                    Assert.AreEqual( 3, values.Length );
                    Assert.AreEqual( typeof( object[] ), values.GetType() );
                    Assert.AreEqual( "one", values[0] );
                    Assert.IsNull(values[1]);
                    Assert.AreEqual( "three", values[2] );
                    break;

                case 5:
                    Assert.AreEqual( 3, values.Length );
                    Assert.AreEqual( typeof( object[] ), values.GetType() );
                    Assert.AreEqual( 1.1, values[0] );
                    Assert.AreEqual( 1.2, values[1] );
                    Assert.AreEqual( 1.3, values[2] );
                    break;

                case 6:
                    Assert.AreEqual( 1, values.Length );
                    Assert.AreEqual( typeof( object[] ), values.GetType() );
                    Assert.IsNull( values[0] );
                    break;

                case 7:
                    Assert.IsNull( values );
                    break;

                case 8:
                    Assert.AreEqual( 3, values.Length );

                    // the first argument should be a string[]
                    Assert.AreEqual( typeof( string[] ), values[0].GetType() );
                    value1 = values[0] as string[];
                    Assert.AreEqual( 3, value1.Length );
                    Assert.AreEqual( "one", value1[0] );
                    Assert.IsNull( value1[1] );
                    Assert.AreEqual( "three", value1[2] );

                    Assert.AreEqual( "one", values[1] );
                    Assert.AreEqual( true, values[2] );
                    break;

                case 9:
                    Assert.AreEqual( 1, values.Length );
                    Assert.AreEqual( typeof( object[] ), values.GetType() );
                    A a = values[0] as A;
                    Assert.AreEqual( "Hello", a.value );
                    Assert.AreEqual( 0, a.value2 );
                    break;

                case 10:
                    Assert.AreEqual( 3, values.Length );

                    // the first argument should be a string[]
                    Assert.AreEqual( typeof( object[] ), values[0].GetType() );
                    object[] value1Obj = values[0] as object[];

                    Assert.AreEqual( 1, value1Obj.Length );
                    a = value1Obj[0] as A;
                    Assert.AreEqual( "Hello", a.value );
                    Assert.AreEqual( 0, a.value2 );

                    Assert.AreEqual( "one", values[1] );
                    Assert.AreEqual( true, values[2] );
                    break;
            }
        }

        public override void sendBaseArray( BaseA[] values )
        {
            sendBaseArrayCounter++;

            switch( sendBaseArrayCounter )
            {
                case 1:
                    Assert.AreEqual( 0, values.Length );
                    break;

                case 2:
                    Assert.AreEqual( 1, values.Length );
                    BaseA baseA = values[0];
                    Assert.AreEqual( typeof( BaseA ), baseA.GetType() );
                    Assert.AreEqual( 1000, baseA.a);
                    break;

                case 3:
                    Assert.AreEqual( 1, values.Length );
                    baseA = values[0];
                    Assert.AreEqual( typeof( BaseB ), baseA.GetType() );
                    Assert.AreEqual( 1001, baseA.a);
                    BaseB baseB = baseA as BaseB;
                    Assert.AreEqual( 1002, baseB.b  );
                    break;

                case 4:
                    Assert.AreEqual( 1, values.Length );
                    baseA = values[0];
                    Assert.AreEqual(typeof(BaseC), baseA.GetType());
                    Assert.AreEqual(1003, baseA.a);
                    BaseC baseC = baseA as BaseC;
                    Assert.AreEqual(1004, baseC.c);
                    break;

                case 5:
                    Assert.IsNull(values);
                    break;

                case 6:
                    Assert.AreEqual(1, values.Length);
                    Assert.IsNull(values[0]);
                    break;

                case 7:
                    Assert.AreEqual(4, values.Length);
                    baseA = values[0] as BaseA;
                    Assert.AreEqual(typeof(BaseA), baseA.GetType());
                    Assert.AreEqual(baseA.a, 1005);
                    baseB = values[1] as BaseB;
                    Assert.AreEqual(typeof(BaseB), baseB.GetType());
                    Assert.AreEqual(baseB.a, 1006);
                    Assert.AreEqual(baseB.b, 1007);
                    baseC = values[2] as BaseC;
                    Assert.AreEqual(typeof(BaseC), baseC.GetType());
                    Assert.AreEqual(baseC.a, 1008);
                    Assert.AreEqual(baseC.c, 1009);
                    Assert.IsNull(values[3]);
                    break;
                    

            }
        }

        public override void sendDerivedArray( BaseB[] values )
        {
            sendDerivedArrayCounter++;

            switch( sendDerivedArrayCounter )
            {
                case 1:
                    Assert.AreEqual( 0, values.Length );
                    break;

                case 2:
                    Assert.AreEqual( 1, values.Length );
                    BaseB baseB = values[0];
                    Assert.AreEqual( typeof( BaseB ), baseB.GetType() );
                    Assert.AreEqual( 1000, baseB.a );
                    Assert.AreEqual( 1001, baseB.b );
                    break;

                case 3:
                    Assert.IsNull( values );
                    break;

                case 4:
                    Assert.AreEqual( 1, values.Length );
                    Assert.IsNull( values[0] );
                    break;

                case 5:
                    Assert.AreEqual( 3, values.Length );
                    baseB = values[0] as BaseB;
                    Assert.AreEqual( typeof( BaseB ), baseB.GetType() );
                    Assert.AreEqual( baseB.a, 1002 );
                    Assert.AreEqual( baseB.b, 1003 );
                    baseB = values[1] as BaseB;
                    Assert.AreEqual( typeof( BaseB ), baseB.GetType() );
                    Assert.AreEqual( baseB.a, 1004 );
                    Assert.AreEqual( baseB.b, 1005 );
                    Assert.IsNull( values[2] );
                    break;


            }
        }

        public override void sendList( System.Collections.IList values )
        {
            sendListCounter++;

            switch( sendListCounter )
            {
                case 1:
                    Assert.AreEqual( 0, values.Count );
                    break;

                case 2:
                    Assert.IsNull( values );
                    break;

                case 3:
                    Assert.AreEqual( 1, values.Count );
                    Assert.IsNull( values[0] );
                    break;

                case 4:
                    Assert.AreEqual( 1, values.Count );
                    Assert.AreEqual( 1, values[0] );
                    break;

                case 5:
                    Assert.AreEqual( 5, values.Count );
                    Assert.AreEqual( 1, values[0] );
                    Assert.AreEqual( false, values[1] );
                    Assert.AreEqual( typeof( string[] ), values[2].GetType() );
                    string[] stringArrays = values[2] as string[];
                    Assert.AreEqual( 3, stringArrays.Length );
                    Assert.AreEqual( "one", stringArrays[0] );
                    Assert.AreEqual( "two", stringArrays[1] );
                    Assert.AreEqual( "three", stringArrays[2] );
                    Assert.AreEqual( typeof( A ), values[3].GetType() );
                    A a = values[3] as A;
                    Assert.AreEqual( "test", a.value );
                    Assert.AreEqual( -1, a.value2 );
                    Assert.AreEqual( typeof( object[] ), values[4].GetType() ); // ideally would like to test BaseA[], but need type hinting
                    object[] baseArray = values[4] as object[];
                    Assert.AreEqual( typeof( BaseA ), baseArray[0].GetType() );
                    BaseA baseA = baseArray[0] as BaseA;
                    Assert.AreEqual( 1000, baseA.a );
                    Assert.AreEqual(typeof(BaseB), baseArray[1].GetType());
                    BaseB baseB = baseArray[1] as BaseB;
                    Assert.AreEqual(1001, baseB.a);
                    Assert.IsNull(baseB.b);

                    Assert.AreEqual(typeof(BaseC), baseArray[2].GetType());
                    BaseC baseC = baseArray[2] as BaseC;
                    Assert.AreEqual(1002, baseC.a);
                    Assert.AreEqual(-1002, baseC.c);
                    break;
            }
        }

        public override void sendException()
        {
            sendExceptionCounter++;

            switch( sendExceptionCounter )
            {
                case 1:
                    System.Threading.Thread.Sleep( 6000 ); // wait long enough to get a timeout excetuion
                    break;

                case 2:
                    throw new ApplicationException( "testing runtime exception" );
  

                case 3:
                    throw new etch.examples.types.ComplexData.CustomException( 10 );
            }
        }

        public override void sendDatetime( DateTime? value )
        {
            sendDatetimeCounter++;

            switch( sendDatetimeCounter ) 
            {
                case 1:
                    Assert.AreEqual( DateTime.MaxValue, value );
                    break;

                case 2:
                    Assert.AreEqual( DateTime.MinValue, value );
                    break;

                case 3:
                    Assert.IsNull( value );
                    break;
            }
        }

        public override void sendShort( short? value )
        {
            sendShortCounter++;

            switch( sendShortCounter )
            {
                case 1:
                    Assert.AreEqual( 0, value );
                    break;

                case 2:
                    Assert.AreEqual( 1, value );
                    break;

                case 3:
                    Assert.AreEqual( -1, value );
                    break;

                case 4:
                    Assert.AreEqual( short.MaxValue, value );
                    break;

                case 5:
                    Assert.AreEqual( short.MinValue, value );
                    break;

                case 6:
                    Assert.IsNull( value );
                    break;
            }
        }
        // this method expects to get multiple send object invokations
        public override void sendObject( object value )
        {
            sendObjectCounter++;

            switch(sendObjectCounter)
            {
                case 1:
                    A a = (A) value;
                    Console.WriteLine( a );
                    break;

                case 2:
                    B b = (B) value;
                    Console.WriteLine( b );
                    break;

                case 3:
                    BaseA baseA = (BaseA) value;
                    Console.WriteLine( baseA );
                    break;

                case 4:
                    BaseB baseB = (BaseB) value;
                    Console.WriteLine( baseB );
                    break;

                case 5:
                    BaseC baseC = (BaseC) value;
                    Console.WriteLine( baseC );
                    break;

                case 6:
                    a = (A) value;
                    Console.WriteLine( a );
                    break;

                case 7:
                    b = (B) value;
                    Console.WriteLine( b );
                    break;

                case 8:
                    baseA = (BaseA) value;
                    Console.WriteLine( baseA );
                    break;

                case 9:
                    baseB = (BaseB) value;
                    Console.WriteLine( baseB );
                    break;

                case 10:
                    baseC = (BaseC) value;
                    Console.WriteLine( baseC );
                    break;
            }

        }
	}
}