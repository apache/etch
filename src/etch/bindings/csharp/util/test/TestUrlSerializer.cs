using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Msg;
using Etch.Support;

namespace Etch.Util.Test
{
    [TestFixture]
    public class TestUrlSerializer
    {

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestUrlSerializer" );
        }


        [Test]
	    public void TestImport()
	    {
		    URL url = new URL( "scheme://user:pw@host:23/resource;s=1;t=2?p=3&q=4#frag" );
		    Test( url );
	    }
    	
	    private void Test( URL url )
	    {
    		
		    XType type = new XType( "url" );
            Class2TypeMap class2type = new Class2TypeMap();
		    URLSerializer.Init( type, class2type );
		    ImportExportHelper helper = type.GetImportExportHelper();
    		
		    StructValue sv = helper.ExportValue( url );
    		
		    Assert.AreEqual( sv.GetXType, type );
    		
		    URL url2 = (URL) helper.ImportValue( sv );
    		
		    Assert.AreEqual( url.ToString(), url2.ToString() );
	    }
    }
}
