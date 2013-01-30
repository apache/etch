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

import java.util.Date;

import org.apache.etch.bindings.java.msg.ImportExportHelper;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DummyValueFactory;
import org.junit.Test;


/** Tests DateSerializer */
public class TestDateSerializer
{
	/** @throws Exception */
	@Test
	public void testImport() throws Exception
	{
		Date date = new Date();
		testDate( date );
	}
	
	private void testDate( Date date )
	{
//		System.out.println( "---------------------------------------------" );
//		System.out.println( "date = "+date );
		
		ValueFactory vf = new DummyValueFactory();
		Type type = new Type( "date" );
		Class2TypeMap class2type = new Class2TypeMap();
		DateSerializer.init( type, class2type );
		ImportExportHelper helper = type.getImportExportHelper();
		
		StructValue sv = helper.exportValue( vf, date );
//		System.out.println( "sv = "+sv );
		
		assertEquals( sv.type(), type );
		
		Date date2 = (Date) helper.importValue( sv );
//		System.out.println( "date2 = "+date2 );
		
		assertEquals( date, date2 );
	}
}
