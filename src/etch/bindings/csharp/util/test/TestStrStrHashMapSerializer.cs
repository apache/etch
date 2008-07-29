using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Support;
using Etch.Msg;

namespace Etch.Util.Test
{
    [TestFixture]
    public class TestStrStrHashMapSerializer
    {
        private readonly DefaultValueFactory vf = new MyValueFactory();

        [Test]
        public void TestImport0()
        {
            StrStrHashMap map = new StrStrHashMap();
            TestMap( map );
        }

        [Test]
        public void TestImport1()
        {
            StrStrHashMap map = new StrStrHashMap();
            map.Add( "a", "1" );
            TestMap( map );
        }

        [Test]
        public void TestImport2()
        {
            StrStrHashMap map = new StrStrHashMap();
            map.Add( "a", "1" );
            map.Add( "b", "2" );
            TestMap( map );
        }

        [Test]
        public void TestImport3()
        {
            StrStrHashMap map = new StrStrHashMap();
            map.Add( "a", "1" );
            map.Add( "b", "2" );
            map.Add( "c", "3" );
            TestMap( map );
        }

        // no tests for null keys as string? isn't allowed

        public void TestMap( StrStrHashMap map )
        {
            StructValue sv = new StructValue( MyValueFactory.blah );
            StrStrHashMapSerializer.ExportValue( vf, map, sv );
            StrStrHashMap map2 = StrStrHashMapSerializer.ImportValue( vf, sv );
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
