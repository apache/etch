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

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestStrIntHashMapSerializer" );
        }


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
            map.Add( "a", 1 );
		    map.Add( "b", null );
            map.Add( "c", 2 );
		    TestMap( map );
	    }
    	
	    // no tests for null keys as string? isn't allowed

        public void TestMap( StrIntHashMap map )
        {
            XType type = new XType( "map" );
		  
            Class2TypeMap class2type = new Class2TypeMap();
		    StrIntHashMapSerializer.Init( type, class2type );
		    ImportExportHelper helper = type.GetImportExportHelper();
    		
		    StructValue sv = helper.ExportValue( map );
    		
		    Assert.AreEqual( sv.GetXType, type );
    		
		    StrIntHashMap map2 = (StrIntHashMap) helper.ImportValue( sv );
    		
		    Assert.AreEqual( map, map2 );
        }

    }
}
