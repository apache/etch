using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Util;
using Etch.Support;
using Etch.Msg;
namespace Etch.Util.Test
{
    [TestFixture]
    public class TestStrIntHashMapSerializer
    {
        private DefaultValueFactory vf = new MyValueFactory();
        
        [Test]
        public void TestImport0() 
	    {
		    StrIntHashMap map = new StrIntHashMap();
		    TestMap( map );
	    }

        [Test]
	    public void TestImport1() 
	    {
		    StrIntHashMap map = new StrIntHashMap();
		    map.Add( "a", 1 );
		    TestMap( map );
	    }

        [Test]
	    public void TestImport2() 
	    {
		    StrIntHashMap map = new StrIntHashMap();
		    map.Add( "a", 1 );
		    map.Add( "b", 2 );
		    TestMap( map );
	    }

        [Test]
	    public void TestImport3() 
	    {
		    StrIntHashMap map = new StrIntHashMap();
		    map.Add( "a", 1 );
		    map.Add( "b", 2 );
		    map.Add( "c", 3 );
		    TestMap( map );
	    }
    	
        [Test]
	    public void TestImportNull1() 
	    {
		    StrIntHashMap map = new StrIntHashMap();
		    map.Add( "a", null );
		    TestMap( map );
	    }
    	
	    // no tests for null keys as string? isn't allowed

        public void TestMap( StrIntHashMap map )
        {
            StructValue sv = new StructValue( MyValueFactory.blah );
            StrIntHashMapSerializer.ExportValue( vf, map, sv );
            StrIntHashMap map2 = StrIntHashMapSerializer.ImportValue( vf, sv );
            Assert.AreEqual( map, map2 );
        }

        public class MyValueFactory : DefaultValueFactory
        {
            private readonly static TypeMap types = new TypeMap();

            private readonly static FieldMap fields = new FieldMap();

            public static XType blah = types.Get( "blah" );

            static MyValueFactory()
            {
                DefaultValueFactory.Init( types, fields );
                StrIntHashMapSerializer.Init( fields, blah );
            }

            public override XType GetType( int? id )
            {
                return types.Get( id );
            }

            public override XType GetType( string name )
            {
                return types.Get( name );
            }

            public override ICollection<XType> GetTypes()
            {
                return types.Values();
            }

            public override Field GetField( int? id )
            {
                return fields.Get( id );
            }

            public override Field GetField( string name )
            {
                return fields.Get( name );
            }

            public override ICollection<Field> GetFields()
            {
                return fields.Values();
            }
        }

    }
}
