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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    [TestFixture]
    public class TestUrlSerializer
    {
        private readonly ValueFactory vf = new DummyValueFactory();

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
    		
		    StructValue sv = helper.ExportValue( vf, url );
    		
		    Assert.AreEqual( sv.GetXType, type );
    		
		    URL url2 = (URL) helper.ImportValue( sv );
    		
		    Assert.AreEqual( url.ToString(), url2.ToString() );
	    }
    }
}
