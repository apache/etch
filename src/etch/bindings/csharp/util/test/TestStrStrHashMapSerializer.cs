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
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestStrStrHashMapSerializer" );
        }


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
            XType type = new XType( "map" );
		   
            Class2TypeMap class2type = new Class2TypeMap();
		    StrStrHashMapSerializer.Init( type, class2type );
		    ImportExportHelper helper = type.GetImportExportHelper();
    		
		    StructValue sv = helper.ExportValue( map );
    		
		    Assert.AreEqual( sv.GetXType, type );
    		
		    StrStrHashMap map2 = (StrStrHashMap) helper.ImportValue( sv );
    		
            Assert.AreEqual( map, map2 );
        }


    }
}
