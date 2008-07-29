using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Msg;
using Etch.Support;

namespace Etch.Util.Test
{
    [TestFixture]
    public class TestDateSerializer
    {

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestDateSerializer" );
        }


        [Test]
        public void TestImport()
        {
            DateTime date = new DateTime();
            TestDate( date );
        }

        private void TestDate( DateTime date )
        {
            XType type = new XType( "date" );
          
            Class2TypeMap class2type = 
                new Class2TypeMap();
            
            DateSerializer.Init( type, class2type );

            ImportExportHelper helper = type.GetImportExportHelper();
            StructValue sv = helper.ExportValue( date );

            Assert.AreEqual( sv.GetXType, type );

            DateTime date2 = ( DateTime ) helper.ImportValue( sv );

            Assert.AreEqual( date, date2 );
        }
    }
}
