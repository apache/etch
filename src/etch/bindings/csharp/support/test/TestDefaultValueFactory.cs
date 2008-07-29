///$Id$
///
///Created by Champakesan, Badri Narayanan on Jul 02, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using System.Collections.Generic;
using NUnit.Framework;
using Etch.Msg;
using System.Text;
using Etch.Support;

namespace Etch.Support.Test
{
    [TestFixture]
    public class TestDefaultValueFactory : List<Object>
    {

        MyValueFactory vf = new MyValueFactory();

        [Test]
	    public void testTypes()
	    {
		    List<XType> eTypes = MkTypeSet(
			    DefaultValueFactory._mt__Etch_RuntimeException,
			    DefaultValueFactory._mt__Etch_AuthException,
			    DefaultValueFactory._mt__exception,
			    MyValueFactory._mt_x,
			    MyValueFactory._mt_y
		    );
    		
		    Assert.IsTrue( CompareTypeLists( eTypes, vf.GetTypes() ) );

		    foreach (XType t in eTypes)
			    TestType( t );
	    }
    	
        /// <summary>
        /// Utility function to compare two lists of types
        /// </summary>
        /// <param name="list1"></param>
        /// <param name="list2"></param>
        /// <returns></returns>
        private bool CompareTypeLists(List<XType> list1, ICollection<XType> list2)
        {
            if ( ( list1.Count == 0 ) && (list2.Count == 0) )
                return true;

            // first compare length
            if ( list1.Count != list2.Count )
                return false;

            // verify that each element in list1 is 
            // present in list2
            foreach ( XType me in list1 )
                if ( ! list2.Contains( me ) )
                    return false;

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
    	
	    [Test]
	    public void TestFields()
	    {
		    List<Field> eFields = MkFieldSet(
			    DefaultValueFactory._mf_msg,
			    DefaultValueFactory._mf__messageId,
			    DefaultValueFactory._mf__inReplyTo,
			    DefaultValueFactory._mf_result,
			    MyValueFactory._mf_a,
			    MyValueFactory._mf_b
		    );

            Assert.IsTrue( CompareFieldLists( eFields, vf.GetFields() ) );
    		
		    foreach (Field f in eFields)
			    TestField( f );
	    }
    	
	    private List<Field> MkFieldSet( params Field[] fields )
	    {
		    List<Field> set = new List<Field>();

		    foreach (Field f in fields )
			    set.Add( f );

		    return set;
	    }
    	
	    private void TestField( Field field )
	    {
		    Assert.AreSame( field, vf.GetField( field.Id ) );
		    Assert.AreSame( field, vf.GetField( field.Name ) );
	    }
    	
	    
	    [Test]
	    public void Test_Etch_RuntimeException_fields()
	    {
		    TestTypeFields(
			    DefaultValueFactory._mt__Etch_RuntimeException,
			    DefaultValueFactory._mf_msg
		    );
    		
		    TestTypeField(
			    DefaultValueFactory._mt__Etch_RuntimeException,
			    DefaultValueFactory._mf_msg,
			    "abc"
		    );			
	    }
    	
	    
	    [Test]
        [ExpectedException (typeof(ArgumentException))]
	    public void Test_Etch_RuntimeException_msg_badValue()
	    {
		    TestTypeField(
			    DefaultValueFactory._mt__Etch_RuntimeException,
			    DefaultValueFactory._mf_msg,
			    123
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_Etch_RuntimeException_badField()
	    {
		    TestTypeField(
			    DefaultValueFactory._mt__Etch_RuntimeException,
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
			    DefaultValueFactory._mt__Etch_AuthException,
			    DefaultValueFactory._mf_msg
		    );
    		
		    TestTypeField(
			    DefaultValueFactory._mt__Etch_AuthException,
			    DefaultValueFactory._mf_msg,
			    "abc"
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_Etch_AuthException_msg_badValue()
	    {
		    TestTypeField(
			    DefaultValueFactory._mt__Etch_AuthException,
			    DefaultValueFactory._mf_msg,
			    123
		    );
	    }
    	
	    [Test]
	    public void Test_exception_fields()
	    {
		    TestTypeFields(
			    DefaultValueFactory._mt__exception,
			    DefaultValueFactory._mf_result,
			    DefaultValueFactory._mf__messageId,
			    DefaultValueFactory._mf__inReplyTo
		    );
    		
		    TestTypeField(
			    DefaultValueFactory._mt__exception,
			    DefaultValueFactory._mf_result,
			    new Exception()
		    );
    		
		    TestTypeField(
			    DefaultValueFactory._mt__exception,
			    DefaultValueFactory._mf__messageId,
			    123L
		    );
    		
		    TestTypeField(
			    DefaultValueFactory._mt__exception,
			    DefaultValueFactory._mf__inReplyTo,
			    123L
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_exception_result_badValue()
	    {
		    TestTypeField(
			    DefaultValueFactory._mt__exception,
			    DefaultValueFactory._mf_result,
			    123
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_exception_messageId_badValue()
	    {
		    TestTypeField(
			    DefaultValueFactory._mt__exception,
			    DefaultValueFactory._mf__messageId,
			    123
		    );
	    }
    	
	    
	    [Test]
        [ ExpectedException ( typeof( ArgumentException ) ) ]
	    public void Test_exception_inReplyTo_badValue()
	    {
		    TestTypeField(
			    DefaultValueFactory._mt__exception,
			    DefaultValueFactory._mf__inReplyTo,
			    123
		    );
	    }

	    
	    [Test]
	    public void GetType_locked()
	    {
		    Assert.IsNotNull( vf.GetType( "x" ) );
		    Assert.IsNotNull( vf.GetType( "y" ) );
		    Assert.IsNull( vf.GetType( "a" ) );
		    Assert.IsNull( vf.GetType( "b" ) );
	    }

	    
	    [Test]
	    public void GetField_locked()
	    {
		    Assert.IsNotNull( vf.GetField( "a" ) );
		    Assert.IsNotNull( vf.GetField( "b" ) );
		    Assert.IsNull( vf.GetField( "x" ) );
		    Assert.IsNull( vf.GetField( "y" ) );
	    }

        [Test]
        public void GetField_Id_MessageId()
        {
            Field x = vf.GetField( 1661383784 );
            Assert.IsNotNull(x);
            Assert.AreSame(DefaultValueFactory._mf__messageId, x);
        }

        [Test]
        public void GetField_Id_InReplyTo()
        {
            Field x = vf.GetField(-307705434);
            Assert.IsNotNull(x);
            Assert.AreSame(DefaultValueFactory._mf__inReplyTo, x);
        }

        [Test]
        public void GetField_Id_Msg()
        {
            Field x = vf.GetField(769750364);
            Assert.IsNotNull(x);
            Assert.AreSame(DefaultValueFactory._mf_msg, x);
        }

        [Test]
        public void GetField_Name_MessageId()
        {
            Field x = vf.GetField("_messageId");
            Assert.IsNotNull(x);
            Assert.AreSame(DefaultValueFactory._mf__messageId, x);
        }

        [Test]
        public void GetField_Name_InReplyTo()
        {
            Field x = vf.GetField("_inReplyTo");
            Assert.IsNotNull(x);
            Assert.AreSame(DefaultValueFactory._mf__inReplyTo, x);
        }

        [Test]
        public void GetField_Name_Msg()
        {
            Field x = vf.GetField("msg");
            Assert.IsNotNull(x);
            Assert.AreSame(DefaultValueFactory._mf_msg, x);
        }

        //[Test]
        //public void GetField_Auto_Just_Enough()
        //{
        //    for ( int i = 0; i < 5000; i++ )
        //        Assert.IsNotNull( vf.GetField( "a"+i ) );
        //}

        //[Test] 
        //[ExpectedException("System.ArgumentException")]
        //public void GetField_Auto_Too_Many()
        //{
        //    for ( int i = 0; i < 10000; i++ )
        //        Assert.IsNotNull( vf.GetField( "a"+i ) );
        //}

        //[Test]
        //public void GetFields()
        //{
        //    IList<Field> list = new List<Field>();
        //    list.Add(vf.GetField("a"));
        //    list.Add(vf.GetField("b"));
        //    list.Add(vf.GetField("c"));
        //    list.Add(DefaultValueFactory._mf_msg);
        //    list.Add(DefaultValueFactory._mf_ticks);
        //    list.Add(DefaultValueFactory._mf__messageId);
        //    list.Add(DefaultValueFactory._mf__inReplyTo);
        //    list.Add( DefaultValueFactory._mf_result );

        //    ICollection<Field> c = vf.GetFields();
        //    Assert.IsTrue(ContainsAll(c, list));
        //    // Assert.IsTrue(ContainsAll(list, c)); -- won't always be true because of execution order
        //}

        //public bool ContainsAll(ICollection<Field> x, ICollection<Field> y)
        //{
        //    foreach(Field z in y)
        //    {
        //        if(!x.Contains(z))
        //            return false;
        //    }
        //    return true;
        //}

        //////////////////////
        //// XType AND FIELD //
        //////////////////////

        //[Test]
        //public void GetTypeAndFieldAreDistinct()
        //{
        //    // verify that the XType and field maps are distinct.

        //    XType mta = vf.GetType("a");
        //    Assert.IsNotNull(mta);

        //    Field mfa = vf.GetField("a");
        //    Assert.IsNotNull(mfa);

        //    Assert.AreNotSame(mta, mfa);
        //}

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
            sv.CheckType(DefaultValueFactory._mt__Etch_RuntimeException);
            Assert.AreEqual(1, sv.Count);
            Assert.AreEqual("System.NullReferenceException: Object reference not set to an instance of an object.",
                sv.Get(DefaultValueFactory._mf_msg));
        }

        [Test]
        public void ExportCustomValue_RuntimeException_msg()
        {
            Exception value = new NullReferenceException("foo != null");
            StructValue sv = vf.ExportCustomValue(value);
            sv.CheckType(DefaultValueFactory._mt__Etch_RuntimeException);
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
            StructValue sv = new StructValue(DefaultValueFactory._mt__Etch_RuntimeException);
            _Etch_RuntimeException e = (_Etch_RuntimeException) vf.ImportCustomValue(sv);
            Assert.IsNotNull(e);
            Assert.IsNull(e.msg);
        }

        [Test]
        public void ImportCustomValue_Etch_RuntimeException_msg()
        {
            StructValue sv = new StructValue(DefaultValueFactory._mt__Etch_RuntimeException);
            sv[DefaultValueFactory._mf_msg] = "foo";
            _Etch_RuntimeException e = (_Etch_RuntimeException) vf.ImportCustomValue(sv);
            Assert.IsNotNull(e);
            Assert.AreEqual("foo", e.msg);
        }

        [Test]
        public void ImportCustomValue_Unknown()
        {
            StructValue sv = new StructValue( vf.GetType( "x" ) );
            Assert.IsNull( vf.ImportCustomValue( sv ) );
            // cannot using unsupported XType
        }

        /// <summary>
        /// A fake implementation of DefaultValueFactory for testing
        /// </summary>
        public class MyValueFactory : DefaultValueFactory
	    {
		    private readonly static TypeMap types = new TypeMap();

		    private readonly static FieldMap fields = new FieldMap();

		    static MyValueFactory()
		    {
			    DefaultValueFactory.Init( types, fields );

                types.Lock();
			    fields.Lock();
		    }
    		
		    // define our types and fields here:
    		
		    /** type x */
		    public readonly static XType _mt_x = types.Get( "x" );
    		
		    /** type y */
		    public readonly static XType _mt_y = types.Get( "y" );
    		
		    /** field a */
		    public readonly static Field _mf_a = fields.Get( "a" );
    		
		    /** field b */
		    public readonly static Field _mf_b = fields.Get( "b" );
    		
		    /**
		     * Constructs the ValueFactoryFake.
		     */
		    public MyValueFactory()
		    {
			    // nothing to do.
		    }

            public override Field GetField( int? id )
		    {
			    return fields.Get( id );

		    }

            public override Field GetField( String name )
		    {
			    return fields.Get( name );
		    }

            public override ICollection<Field> GetFields()
		    {
			    return new List<Field>( fields.Values() );
		    }

            public override XType GetType( String name )
		    {
			    return types.Get( name );
		    }

            public override ICollection<XType> GetTypes()
            {
                return new List<XType>( types.Values() );
            }

            public override XType GetType( int? id )
		    {
			    return types.Get( id );
		    }
	    }
        

        //[Test]
        //public void to_boolean_array()
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //    // try null array

        //    Assert.IsNull( DefaultValueFactory.to_boolean( typeof( bool[] ), null ) );
        //    Assert.IsNull( DefaultValueFactory.to_boolean( typeof( bool[] ), null ) );

        //    // our data values
        //     bool x = true;
        //     bool y = false;
        //     bool z = true;
	    	
        //    // try a 0-dimensional array

        //     Assert.AreEqual( x, DefaultValueFactory.to_boolean( typeof( bool ), x ) );
        //     Assert.AreEqual( y, DefaultValueFactory.to_boolean( typeof( bool ), y ) );
        //     Assert.AreEqual( z, DefaultValueFactory.to_boolean( typeof( bool ), z ) );
	    	
        //    // try a 1-dimensional array
	    	
        //    ArrayValue av = new ArrayValue();
        //    av.Add( x );
        //    av.Add( y );
        //    av.Add( z );

        //    Boolean[] b1x3 = ( Boolean[] ) DefaultValueFactory.to_boolean( typeof( bool[] ), av );
        //    Assert.IsNotNull( b1x3 );
        //    Assert.IsTrue( ArrayEquals( new Boolean[] { x, y, z }, b1x3 ) );
        //    b1x3 = null;
	    	
        //    // try a 2-dimensional array
	    	
        //    av.Clear();
        //    ArrayValue av0 = av.AddArrayValue();
        //    av0.Add( x );
        //    av0.Add( y );
        //    av0.Add( z );
        //    ArrayValue av1 = av.AddArrayValue();
        //    av1.Add( z );
        //    av1.Add( x );
        //    av1.Add( y );

        //    Boolean[][] b2x3 = ( Boolean[][] ) DefaultValueFactory.to_boolean( typeof( bool[][] ), av );
        //    Assert.IsNotNull( b2x3 );
        //    Assert.IsTrue(ArrayEquals(new Boolean[][] { 
        //                                                new Boolean[] { x, y, z }, 
        //                                                new Boolean[] { z, x, y } }, b2x3));
        //}

        //private Boolean ArrayEquals(Object a, Object b)
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //    if(a.GetType().IsArray != b.GetType().IsArray)
        //        return false;

        //    if(!a.GetType().IsArray)
        //        return ObjectEquals(a, b);

        //    int n = ((Array) a).Length;

        //    if(n != ((Array) b).Length)
        //                return false;

        //        for(int i = 0; i < n; i++)
        //            if(!ArrayEquals(((Array)a).GetValue(i), ((Array)b).GetValue(i)))
        //                return false;
        //    return true;
        //}

        //private Boolean ObjectEquals(Object a, Object b)
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //    if(a == null && b == null)
        //        return true;

        //    if(a == null || b == null)
        //        return false;

        //    return a.Equals(b);
        //}

        //[Test]
        //public void to_byte_array()
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //// try null array

        //    Assert.IsNull( DefaultValueFactory.to_byte( typeof( sbyte[] ), null ) );
        //    Assert.IsNull( DefaultValueFactory.to_byte( typeof( sbyte[] ), null ) );
		
        //// our data values
		
        // sbyte x = 0;
        // sbyte y = 1;
        // sbyte z = -1;
		
        //// try a 0-dimensional array

        // Assert.AreEqual( x, DefaultValueFactory.to_byte( typeof( sbyte ), x ) );
        // Assert.AreEqual( y, DefaultValueFactory.to_byte( typeof( sbyte ), y ) );
        // Assert.AreEqual( z, DefaultValueFactory.to_byte( typeof( sbyte ), z ) );
		
        //// try a 1-dimensional array
		
        //ArrayValue av = new ArrayValue();
        //av.Add( x );
        //av.Add( y );
        //av.Add( z );

        //sbyte[] b1x3 = ( sbyte[] ) DefaultValueFactory.to_byte( typeof( sbyte[] ), av );
        //Assert.IsNotNull( b1x3 );
        //Assert.IsTrue( ArrayEquals( new sbyte[] { x, y, z }, b1x3 ) );
        //b1x3 = null;
		
        //// try a 2-dimensional array
		
        //av.Clear();
        //ArrayValue av0 = av.AddArrayValue();
        //av0.Add( x );
        //av0.Add( y );
        //av0.Add( z );
        //ArrayValue av1 = av.AddArrayValue();
        //av1.Add( z );
        //av1.Add( x );
        //av1.Add( y );

        //sbyte[][] b2x3 = ( sbyte[][] ) DefaultValueFactory.to_byte( typeof( sbyte[][] ), av );
        //Assert.IsNotNull( b2x3 );
        //Assert.IsTrue(ArrayEquals(new sbyte[][] { 
        //                                          new sbyte[] { x, y, z }, 
        //                                          new sbyte[] { z, x, y } }, b2x3));
        //}

        //[Test]
        //public void to_short_array()
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //// try null array

        //    Assert.IsNull( DefaultValueFactory.to_short( typeof( short ), null ) );
        //    Assert.IsNull( DefaultValueFactory.to_short( typeof( short[] ), null ) );
		
        //// our data values
		
        // short x = 1234;
        // short y = 2345;
        // short z = -3456;
		
        //// try a 0-dimensional array

        // Assert.AreEqual( x, DefaultValueFactory.to_short( typeof( short ), x ) );
        // Assert.AreEqual( y, DefaultValueFactory.to_short( typeof( short ), y ) );
        // Assert.AreEqual( z, DefaultValueFactory.to_short( typeof( short ), z ) );
		
        //// try a 1-dimensional array
		
        //ArrayValue av = new ArrayValue();
        //av.Add( x );
        //av.Add( y );
        //av.Add( z );

        //short[] b1x3 = ( short[] ) DefaultValueFactory.to_short( typeof( short[] ), av );
        //Assert.IsNotNull( b1x3 );
        //Assert.IsTrue( ArrayEquals( new short[] { x, y, z }, b1x3 ) );
        //b1x3 = null;
		
        //// try a 2-dimensional array
		
        //av.Clear();
        //ArrayValue av0 = av.AddArrayValue();
        //av0.Add( x );
        //av0.Add( y );
        //av0.Add( z );
        //ArrayValue av1 = av.AddArrayValue();
        //av1.Add( z );
        //av1.Add( x );
        //av1.Add( y );

        //short[][] b2x3 = ( short[][] ) DefaultValueFactory.to_short( typeof( short[][] ), av );
        //Assert.IsNotNull( b2x3 );
        //Assert.IsTrue(ArrayEquals(new short[][] { 
        //                                          new short[] { x, y, z }, 
        //                                          new short[] { z, x, y } }, b2x3));
        //}

        //[Test]
        //public void to_integer_array()
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //// try null array

        //    Assert.IsNull( DefaultValueFactory.to_int( typeof( int ), null ) );
        //    Assert.IsNull( DefaultValueFactory.to_int( typeof( int[] ), null ) );
		
        //// our data values
		
        // int x = 123456;
        // int y = 234567;
        // int z = -345678;
		
        //// try a 0-dimensional array

        // Assert.AreEqual( x, DefaultValueFactory.to_int( typeof( int ), x ) );
        // Assert.AreEqual( y, DefaultValueFactory.to_int( typeof( int ), y ) );
        // Assert.AreEqual( z, DefaultValueFactory.to_int( typeof( int ), z ) );
		
        //// try a 1-dimensional array
		
        //ArrayValue av = new ArrayValue();
        //av.Add( x );
        //av.Add( y );
        //av.Add( z );

        //int[] b1x3 = ( int[] ) DefaultValueFactory.to_int( typeof( int[] ), av );
        //Assert.IsNotNull( b1x3 );
        //Assert.IsTrue( ArrayEquals( new int[] { x, y, z }, b1x3 ) );
        //b1x3 = null;
		
        //// try a 2-dimensional array
		
        //av.Clear();
        //ArrayValue av0 = av.AddArrayValue();
        //av0.Add( x );
        //av0.Add( y );
        //av0.Add( z );
        //ArrayValue av1 = av.AddArrayValue();
        //av1.Add( z );
        //av1.Add( x );
        //av1.Add( y );

        //int[][] b2x3 = ( int[][] ) DefaultValueFactory.to_int( typeof( int[][] ), av );
        //Assert.IsNotNull( b2x3 );
        //Assert.IsTrue(ArrayEquals(new int[][] { 
        //                                        new int[] { x, y, z }, 
        //                                        new int[] { z, x, y } }, b2x3));
        //}

        //[Test]
        //public void to_long_array()
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //// try null array

        //    Assert.IsNull( DefaultValueFactory.to_long( typeof( long ), null ) );
        //    Assert.IsNull( DefaultValueFactory.to_long( typeof( long[] ), null ) );
		
        //// our data values
		
        // long x = 123456789123L;
        // long y = 234567891234L;
        // long z = -345678912345L;
		
        //// try a 0-dimensional array

        // Assert.AreEqual( x, DefaultValueFactory.to_long( typeof( long ), x ) );
        // Assert.AreEqual( y, DefaultValueFactory.to_long( typeof( long ), y ) );
        // Assert.AreEqual( z, DefaultValueFactory.to_long( typeof( long ), z ) );
		
        //// try a 1-dimensional array
		
        //ArrayValue av = new ArrayValue();
        //av.Add( x );
        //av.Add( y );
        //av.Add( z );

        //long[] b1x3 = ( long[] ) DefaultValueFactory.to_long( typeof( long[] ), av );
        //Assert.IsNotNull( b1x3 );
        //Assert.IsTrue( ArrayEquals( new long[] { x, y, z }, b1x3 ) );
        //b1x3 = null;
		
        //// try a 2-dimensional array
		
        //av.Clear();
        //ArrayValue av0 = av.AddArrayValue();
        //av0.Add( x );
        //av0.Add( y );
        //av0.Add( z );
        //ArrayValue av1 = av.AddArrayValue();
        //av1.Add( z );
        //av1.Add( x );
        //av1.Add( y );

        //long[][] b2x3 = ( long[][] ) DefaultValueFactory.to_long( typeof( long[][] ), av );
        //Assert.IsNotNull( b2x3 );
        //Assert.IsTrue(ArrayEquals(new long[][] { 
        //                                         new long[] { x, y, z }, 
        //                                         new long[] { z, x, y } }, b2x3));
        //}

        //[Test]
        //public void to_float_array()
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //// try null array

        //    Assert.IsNull( DefaultValueFactory.to_float( typeof( float ), null ) );
        //    Assert.IsNull( DefaultValueFactory.to_float( typeof( float[] ), null ) );
		
        //// our data values
		
        // float x = 1.2f;
        // float y = 2.3f;
        // float z = -3.4f;
		
        //// try a 0-dimensional array

        // Assert.AreEqual( x, DefaultValueFactory.to_float( typeof( float ), x ) );
        // Assert.AreEqual( y, DefaultValueFactory.to_float( typeof( float ), y ) );
        // Assert.AreEqual( z, DefaultValueFactory.to_float( typeof( float ), z ) );
		
        //// try a 1-dimensional array
		
        //ArrayValue av = new ArrayValue();
        //av.Add( x );
        //av.Add( y );
        //av.Add( z );

        //float[] b1x3 = ( float[] ) DefaultValueFactory.to_float( typeof( float[] ), av );
        //Assert.IsNotNull( b1x3 );
        //Assert.IsTrue( ArrayEquals( new float[] { x, y, z }, b1x3 ) );
        //b1x3 = null;
		
        //// try a 2-dimensional array
		
        //av.Clear();
        //ArrayValue av0 = av.AddArrayValue();
        //av0.Add( x );
        //av0.Add( y );
        //av0.Add( z );
        //ArrayValue av1 = av.AddArrayValue();
        //av1.Add( z );
        //av1.Add( x );
        //av1.Add( y );

        //float[][] b2x3 = ( float[][] ) DefaultValueFactory.to_float( typeof( float[][] ), av );
        //Assert.IsNotNull( b2x3 );
        //Assert.IsTrue(ArrayEquals(new float[][] { 
        //                                          new float[] { x, y, z }, 
        //                                          new float[] { z, x, y } }, b2x3));
        //}

        //[Test]
        //public void to_double_array()
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //// try null array

        //    Assert.IsNull( DefaultValueFactory.to_double( typeof( double ), null ) );
        //    Assert.IsNull( DefaultValueFactory.to_double( typeof( double[] ), null ) );
		
        //// our data values
		
        // double x = 1.2e99;
        // double y = 2.3e99;
        // double z = -3.4e99;
		
        //// try a 0-dimensional array

        // Assert.AreEqual( x, DefaultValueFactory.to_double( typeof( double ), x ) );
        // Assert.AreEqual( y, DefaultValueFactory.to_double( typeof( double ), y ) );
        // Assert.AreEqual( z, DefaultValueFactory.to_double( typeof( double ), z ) );
		
        //// try a 1-dimensional array
		
        //ArrayValue av = new ArrayValue();
        //av.Add( x );
        //av.Add( y );
        //av.Add( z );

        //double[] b1x3 = ( double[] ) DefaultValueFactory.to_double( typeof( double[] ), av );
        //Assert.IsNotNull( b1x3 );
        //Assert.IsTrue( ArrayEquals( new double[] { x, y, z }, b1x3 ) );
        //b1x3 = null;
		
        //// try a 2-dimensional array
		
        //av.Clear();
        //ArrayValue av0 = av.AddArrayValue();
        //av0.Add( x );
        //av0.Add( y );
        //av0.Add( z );
        //ArrayValue av1 = av.AddArrayValue();
        //av1.Add( z );
        //av1.Add( x );
        //av1.Add( y );

        //double[][] b2x3 = ( double[][] ) DefaultValueFactory.to_double( typeof( double[][] ), av );
        //Assert.IsNotNull( b2x3 );
        //Assert.IsTrue(ArrayEquals(new double[][] { 
        //                                           new double[] { x, y, z }, 
        //                                           new double[] { z, x, y } }, b2x3));
        //}

        //[Test]
        //public void to_string_array()
        //{
        //    DefaultValueFactory vf = new DefaultValueFactory(3);
        //// try null array

        //    Assert.IsNull( DefaultValueFactory.to_string( typeof( string ), null ) );
        //    Assert.IsNull( DefaultValueFactory.to_string( typeof( string[] ), null ) );
		
        //// our data values
		
        // String x = "abc";
        // String y = "def";
        // String z = "ghi";
		
        //// try a 0-dimensional array

        // Assert.AreEqual( x, DefaultValueFactory.to_string( typeof( string ), x ) );
        // Assert.AreEqual( y, DefaultValueFactory.to_string( typeof( string ), y ) );
        // Assert.AreEqual( z, DefaultValueFactory.to_string( typeof( string ), z ) );
		
        //// try a 1-dimensional array
		
        //ArrayValue av = new ArrayValue();
        //av.Add( x );
        //av.Add( y );
        //av.Add( z );

        //String[] b1x3 = ( String[] ) DefaultValueFactory.to_string( typeof( string[] ), av );
        //Assert.IsNotNull( b1x3 );
        //Assert.IsTrue( ArrayEquals( new String[] { x, y, z }, b1x3 ) );
        //b1x3 = null;
		
        //// try a 2-dimensional array
		
        //av.Clear();
        //ArrayValue av0 = av.AddArrayValue();
        //av0.Add( x );
        //av0.Add( y );
        //av0.Add( z );
        //ArrayValue av1 = av.AddArrayValue();
        //av1.Add( z );
        //av1.Add( x );
        //av1.Add( y );

        //String[][] b2x3 = ( String[][] ) DefaultValueFactory.to_string( typeof( string[][] ), av );
        //Assert.IsNotNull( b2x3 );
        //Assert.IsTrue(ArrayEquals(new String[][] { 
        //                                           new String[] { x, y, z }, 
        //                                           new String[] { z, x, y } }, b2x3));
        //}   
    }
}