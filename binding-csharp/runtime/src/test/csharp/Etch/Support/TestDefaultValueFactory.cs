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
using System.Collections.Generic;
using System.Text;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    [TestFixture]
    public class TestDefaultValueFactory : List<Object>
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.WriteLine("TestDefaultValueFactory");
        }

        private readonly MyValueFactory vf = new MyValueFactory();

        [Test]
	    public void testTypes()
	    {
            MyValueFactory vf = new MyValueFactory();

		    List<XType> eTypes = MkTypeSet(
                vf.GetType("_Etch_RuntimeException"),
                vf.GetType("_Etch_List"),
                vf.GetType("_Etch_Map"),
                vf.GetType("_Etch_Datetime"),
                vf.GetType("_Etch_AuthException"),
                vf.GetType("_exception"),
			    MyValueFactory._mt_x,
			    MyValueFactory._mt_y
		    );

            //Dump("eTypes", eTypes);
            //Dump("vf.GetTypes()", vf.GetTypes());

		    Assert.IsTrue( CompareTypeLists( eTypes, vf.GetTypes() ) );

		    foreach (XType t in eTypes)
			    TestType( t );
	    }

        private void Dump(string title, ICollection<XType> c)
        {
            Console.Write(title+"[");
            bool first = true;
            foreach (XType type in c)
            {
                if (!first)
                    Console.Write(", ");
                else
                    first = false;
                Console.Write(type);
            }
            Console.WriteLine("]");
        }
    	
        /// <summary>
        /// Utility function to compare two lists of types
        /// </summary>
        /// <param name="list1"></param>
        /// <param name="list2"></param>
        /// <returns></returns>
        private bool CompareTypeLists(List<XType> list1, ICollection<XType> list2)
        {
            if ((list1.Count == 0) && (list2.Count == 0))
                return true;

            // first compare length
            if (list1.Count != list2.Count)
            {
                Console.WriteLine("lists count differs: {0} v. {1}", list1.Count, list2.Count);
                return false;
            }

            // verify that each element in list1 is 
            // present in list2 and vice versa

            foreach (XType me in list1)
            {
                if (!list2.Contains(me))
                {
                    Console.WriteLine("!list2.Contains({0})", me);
                    return false;
                }
            }

            foreach (XType me in list2)
            {
                if (!list1.Contains(me))
                {
                    Console.WriteLine("!list1.Contains({0})", me);
                    return false;
                }
            }

            return true;
        }
        
        /// <summary>
        /// Utility function to compare two lists of fields
        /// </summary>
        /// <param name="list1"></param>
        /// <param name="list2"></param>
        /// <returns></returns>
        private bool CompareFieldLists( List<Field> list1, ICollection<Field> list2 )
        {
            if ( ( list1.Count == 0 ) && ( list2.Count == 0 ) )
                return true;

            // first compare length
            if ( list1.Count != list2.Count )
                return false;

            // verify that each element in list1 is 
            // present in list2
            foreach ( Field me in list1 )
                if ( !list2.Contains( me ) )
                    return false;

            return true;
        }

	    private List<XType> MkTypeSet( params XType[] types )
	    {
		    List<XType> set = new List<XType>();

		    foreach (XType t in types)
			    set.Add( t );

		    return set;
	    }
    	
	    private void TestType( XType type )
	    {
		    Assert.AreSame( type, vf.GetType( type.Id ) );
		    Assert.AreSame( type, vf.GetType( type.Name ) );
	    }
    	
	    
    	
	    private List<Field> MkFieldSet( params Field[] fields )
	    {
		    List<Field> set = new List<Field>();

		    foreach (Field f in fields )
			    set.Add( f );

		    return set;
	    }
    	
	    
	    
	    [Test]
	    public void Test_Etch_RuntimeException_fields()
	    {
		    TestTypeFields(
                vf.GetType("_Etch_RuntimeException"),
			    DefaultValueFactory._mf_msg
		    );
    		
		    TestTypeField(
                vf.GetType("_Etch_RuntimeException"),
			    DefaultValueFactory._mf_msg,
			    "abc"
		    );			
	    }
    	
	    
	    [Test]
        [ExpectedException (typeof(ArgumentException))]
	    public void Test_Etch_RuntimeException_msg_badValue()
	    {
		    TestTypeField(
                vf.GetType("_Etch_RuntimeException"),
			    DefaultValueFactory._mf_msg,
			    123
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_Etch_RuntimeException_badField()
	    {
		    TestTypeField(
                vf.GetType("_Etch_RuntimeException"),
			    MyValueFactory._mf_a,
			    "abc"
		    );	
	    }
    	
	    private void TestTypeField( XType type, Field field, Object value )
	    {
		    Message msg = new Message( type, vf );
		    msg.Add( field, value );
	    }
    	
	    private void TestTypeFields( XType type, params Field[] fields )
	    {
		    List<Field> aFields = type.GetFields();
		    List<Field> eFields = MkFieldSet( fields );
            Assert.IsTrue( CompareFieldLists( eFields, aFields ) );
	    }
    	
	    
	    [Test]
	    public void Test_Etch_AuthException_fields()
	    {
		    TestTypeFields(
                vf.GetType("_Etch_AuthException"),
			    DefaultValueFactory._mf_msg
		    );
    		
		    TestTypeField(
                vf.GetType("_Etch_AuthException"),
			    DefaultValueFactory._mf_msg,
			    "abc"
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_Etch_AuthException_msg_badValue()
	    {
		    TestTypeField(
                vf.GetType("_Etch_AuthException"),
			    DefaultValueFactory._mf_msg,
			    123
		    );
	    }
    	
	    [Test]
	    public void Test_exception_fields()
	    {
		    TestTypeFields(
                vf.GetType("_exception"),
			    DefaultValueFactory._mf_result,
			    DefaultValueFactory._mf__messageId,
			    DefaultValueFactory._mf__inReplyTo
		    );
    		
		    TestTypeField(
                vf.GetType("_exception"),
			    DefaultValueFactory._mf_result,
			    new Exception()
		    );
    		
		    TestTypeField(
                vf.GetType("_exception"),
			    DefaultValueFactory._mf__messageId,
			    123L
		    );
    		
		    TestTypeField(
                vf.GetType("_exception"),
			    DefaultValueFactory._mf__inReplyTo,
			    123L
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_exception_result_badValue()
	    {
		    TestTypeField(
                vf.GetType("_exception"),
			    DefaultValueFactory._mf_result,
			    123
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_exception_messageId_badValue()
	    {
		    TestTypeField(
                vf.GetType("_exception"),
			    DefaultValueFactory._mf__messageId,
			    true
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_exception_inReplyTo_badValue()
	    {
		    TestTypeField(
                vf.GetType("_exception"),
			    DefaultValueFactory._mf__inReplyTo,
			    "abc"
		    );
	    }

	    
	    [Test]
	    public void GetType_locked()
	    {
		    Assert.IsNotNull( vf.GetType( "x" ) );
		    Assert.IsNotNull( vf.GetType( "y" ) );
            Assert.IsNotNull(vf.GetType("a"));
            vf.LockDynamicTypes();
		    Assert.IsNull( vf.GetType( "b" ) );
	    }

	    
	    [Test]
	    public void GetField_locked()
	    {
		    XType type = new XType("blah");

            Assert.IsNotNull( type.GetField( "a" ) );
		    Assert.IsNotNull( type.GetField( "b" ) );
            type.Lock();
            Assert.IsNotNull(type.GetField("a"));
            Assert.IsNotNull(type.GetField("b"));
		    Assert.IsNull( type.GetField( "x" ) );
		    Assert.IsNull( type.GetField( "y" ) );
	    }

        [Test]
        public void GetField_Id_MessageId()
        {
            Assert.AreEqual("_messageId", DefaultValueFactory._mf__messageId.Name);
            Assert.AreEqual(1661383784, DefaultValueFactory._mf__messageId.Id);
        }

        [Test]
        public void GetField_Id_InReplyTo()
        {
            Assert.AreEqual("_inReplyTo", DefaultValueFactory._mf__inReplyTo.Name);
            Assert.AreEqual(-307705434,DefaultValueFactory._mf__inReplyTo.Id);
        }

        
        /////////////////////
        // STRING ENCODING //
        /////////////////////

        [Test]
        public void GetStringEncoding()
        {
            Encoding enc = vf.GetStringEncoding();
            String s = enc.GetString(enc.GetBytes("abc"));
            Assert.AreEqual("abc", s);
        }

        ////////////////
        // MESSAGE ID //
        ////////////////

        [Test]
        public void MessageId()
        {   
            XType mt_foo = new XType( "foo" );
            mt_foo.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );
            
            Message msg = new Message( mt_foo, vf );

            Assert.IsNull(vf.GetMessageId(msg));

            vf.SetMessageId(msg, 234L);

            long msgid = (long) vf.GetMessageId(msg);
            Assert.IsNotNull(msgid);
            Assert.AreEqual(234L, msgid);

            vf.SetMessageId(msg, null);

            Assert.IsNull(vf.GetMessageId(msg));
        }

        /////////////////
        // IN REPLY TO //
        /////////////////

        [Test]
        public void InReplyTo()
        {
            XType mt_foo = new XType( "foo" );
            mt_foo.PutValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.Get( 0 ) );

            Message msg = new Message( mt_foo, vf );

            Assert.IsNull( vf.GetInReplyTo( msg ) );

            vf.SetInReplyTo( msg, 234L );

            long? msgid = vf.GetInReplyTo( msg );
            Assert.IsNotNull( msgid );
            Assert.AreEqual( 234L, msgid );

            vf.SetInReplyTo( msg, null );

            Assert.IsNull( vf.GetInReplyTo( msg ) );
        }

        //////////////////////
        // VALUE CONVERSION //
        //////////////////////

        [Test]
        public void ExportCustomValue_RuntimeException()
        {
            Exception value = new NullReferenceException();
            StructValue sv = vf.ExportCustomValue(value);
            sv.CheckType(vf.GetType("_Etch_RuntimeException"));
            Assert.AreEqual(1, sv.Count);
            Assert.AreEqual("System.NullReferenceException: Object reference not set to an instance of an object.",
                sv.Get(DefaultValueFactory._mf_msg));
        }

        [Test]
        public void ExportCustomValue_RuntimeException_msg()
        {
            Exception value = new NullReferenceException("foo != null");
            StructValue sv = vf.ExportCustomValue(value);
            sv.CheckType(vf.GetType("_Etch_RuntimeException"));
            Assert.AreEqual(1, sv.Count);
            Assert.AreEqual("System.NullReferenceException: foo != null",
                sv.Get(DefaultValueFactory._mf_msg));
        }

        [Test]
        public void ExportCustomValue_Object()
        {
            Object value = new Object();
            Assert.IsNull( vf.ExportCustomValue( value ) );
            // cannot export unsupported XType
        }

        [Test]
        public void ImportCustomValue_Etch_RuntimeException()
        {
            StructValue sv = new StructValue(vf.GetType("_Etch_RuntimeException"), vf);
            _Etch_RuntimeException e = (_Etch_RuntimeException) vf.ImportCustomValue(sv);
            Assert.IsNotNull(e);
            Assert.IsNull(e.msg);
        }

        [Test]
        public void ImportCustomValue_Etch_RuntimeException_msg()
        {
            StructValue sv = new StructValue(vf.GetType("_Etch_RuntimeException"), vf);
            sv[DefaultValueFactory._mf_msg] = "foo";
            _Etch_RuntimeException e = (_Etch_RuntimeException) vf.ImportCustomValue(sv);
            Assert.IsNotNull(e);
            Assert.AreEqual("foo", e.msg);
        }

        [Test]
        public void ImportCustomValue_Unknown()
        {
            StructValue sv = new StructValue( vf.GetType( "x" ), vf );
            Assert.IsNull( vf.ImportCustomValue( sv ) );
            // cannot using unsupported XType
        }

        /// <summary>
        /// A fake implementation of DefaultValueFactory for testing
        /// </summary>
        public class MyValueFactory : DefaultValueFactory
	    {
		    private readonly static TypeMap types = new TypeMap();

		   

            private readonly static Class2TypeMap class2type = 
                new Class2TypeMap();

		    
    		
		    // define our types and fields here:
    		
		    /** type x */
		    public readonly static XType _mt_x = types.Get( "x" );
    		
		    /** type y */
		    public readonly static XType _mt_y = types.Get( "y" );
    		
		    /** field a */
		    public readonly static Field _mf_a = new Field( "a" );
    		
		    /** field b */
		    public readonly static Field _mf_b = new Field( "b" );

            static MyValueFactory()
            {
                DefaultValueFactory.Init(types, class2type);
                types.Lock();
                class2type.Lock();


            }
            
    		
		    /**
		     * Constructs the ValueFactoryFake.
		     */
		    public MyValueFactory(): base("none:",types,class2type)
		    {
			    // nothing to do.
		    }

           

	    }
        
    }
}