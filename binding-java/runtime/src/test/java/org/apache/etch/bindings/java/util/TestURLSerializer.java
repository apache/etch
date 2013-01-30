/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.bindings.java.util;

import static org.junit.Assert.assertEquals;

import org.apache.etch.bindings.java.msg.ImportExportHelper;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DummyValueFactory;
import org.apache.etch.util.URL;
import org.junit.Test;


/** Tests URLSerializer */
public class TestURLSerializer
{
	/** @throws Exception */
	@Test
	public void testImport() throws Exception
	{
		URL url = new URL( "scheme://user:pw@host:23/resource;s=1;t=2?p=3&q=4#frag" );
		test( url );
	}
	
	private void test( URL url )
	{
//		System.out.println( "---------------------------------------------" );
//		System.out.println( "url = "+url );

		ValueFactory vf = new DummyValueFactory();
		Type type = new Type( "url" );
		Class2TypeMap class2type = new Class2TypeMap();
		URLSerializer.init( type, class2type );
		ImportExportHelper helper = type.getImportExportHelper();
		
		StructValue sv = helper.exportValue( vf, url );
//		System.out.println( "sv = "+sv );
		
		assertEquals( sv.type(), type );
		
		URL url2 = (URL) helper.importValue( sv );
//		System.out.println( "url2 = "+url2 );
		
		assertEquals( url, url2 );
	}
}
