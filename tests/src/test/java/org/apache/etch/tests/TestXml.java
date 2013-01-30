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

package org.apache.etch.tests;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertNotNull;
import static junit.framework.Assert.assertNull;
import static junit.framework.Assert.assertTrue;

import java.io.File;
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.etch.compiler.EtchGrammarConstants;
import org.junit.After;
import org.junit.Before;
import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;


/** */
public class TestXml {

	Document document;

	/** @throws Exception */
	@Before
	public void setUp() throws Exception {
		// Load Test.xml into DOM
		DocumentBuilder builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
		//String userDir = System.getProperty("user.dir");
		try
		{
			File f = new File("tests/target/generated-sources/main/etch/xml/org/apache/etch/tests/Test1.xml");
			
			document = builder.parse(f);
		}
		catch(Exception e)
		{
			System.out.println("Unable to load Test.xml document: " + e);
			System.out.println("Current dir is "+new File("." ).getAbsolutePath() );
			assertTrue( false );
		}
	}

	/** @throws Exception */
	@After
	public void tearDown() throws Exception {
		// unload Test.xml 
		document = null;
	}


	/** @throws Exception */
	@org.junit.Test
	public void testModule() throws Exception
	{
		NodeList list = document.getElementsByTagName("module");
		
		assertEquals(list.getLength(), 1);
		
		String moduleName = list.item(0).getAttributes().getNamedItem("name").getNodeValue();
		assertEquals(moduleName, "org.apache.etch.tests");
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(list.item(0), new String[] {"name"}));
		
		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(list.item(0), new String[] {"description", "service"}));
		
		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testService() throws Exception
	{
		NodeList list = document.getElementsByTagName("service");
		
		assertEquals(list.getLength(), 1);
		
		String serviceName = list.item(0).getAttributes().getNamedItem("name").getNodeValue();
		assertEquals(serviceName, "Test1");
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(list.item(0), new String[] {"name"}));
			
		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(list.item(0), new String[] { "consts", "externs", "enums", "structs", "exceptions", "methods"}));
		
		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testConsts() throws Exception
	{
		// verify consts
		NodeList list = document.getElementsByTagName("const");
		
		assertEquals(list.getLength(), 42);
		
		int nodeCount = 0;
		
		checkConstProperties(list.item(nodeCount++), "BOOL1", getTypeName(EtchGrammarConstants.BOOLEAN), "false", null);
		checkConstProperties(list.item(nodeCount++), "BOOL2", getTypeName(EtchGrammarConstants.BOOLEAN), "true", null);
	
		checkConstProperties(list.item(nodeCount++), "BYTE1", getTypeName(EtchGrammarConstants.BYTE), "-128", null);
		checkConstProperties(list.item(nodeCount++), "BYTE2", getTypeName(EtchGrammarConstants.BYTE), "-1", null);
		checkConstProperties(list.item(nodeCount++), "BYTE3", getTypeName(EtchGrammarConstants.BYTE), "0", null);
		checkConstProperties(list.item(nodeCount++), "BYTE4", getTypeName(EtchGrammarConstants.BYTE), "1", null);
		checkConstProperties(list.item(nodeCount++), "BYTE5", getTypeName(EtchGrammarConstants.BYTE), "127", null);
		
		checkConstProperties(list.item(nodeCount++), "SHORT1", getTypeName(EtchGrammarConstants.SHORT), "-32768", null);
		checkConstProperties(list.item(nodeCount++), "SHORT2", getTypeName(EtchGrammarConstants.SHORT), "-1", null);
		checkConstProperties(list.item(nodeCount++), "SHORT3", getTypeName(EtchGrammarConstants.SHORT), "0", null);
		checkConstProperties(list.item(nodeCount++), "SHORT4", getTypeName(EtchGrammarConstants.SHORT), "1", null);
		checkConstProperties(list.item(nodeCount++), "SHORT5", getTypeName(EtchGrammarConstants.SHORT), "32767", null);
		
		checkConstProperties(list.item(nodeCount++), "INT1", getTypeName(EtchGrammarConstants.INT), "-2147483648", null);
		checkConstProperties(list.item(nodeCount++), "INT2", getTypeName(EtchGrammarConstants.INT), "-1", null);
		checkConstProperties(list.item(nodeCount++), "INT3", getTypeName(EtchGrammarConstants.INT), "0", null);
		checkConstProperties(list.item(nodeCount++), "INT4", getTypeName(EtchGrammarConstants.INT), "1", null);
		checkConstProperties(list.item(nodeCount++), "INT5", getTypeName(EtchGrammarConstants.INT), "2147483647", null);
		
		checkConstProperties(list.item(nodeCount++), "LONG1", getTypeName(EtchGrammarConstants.LONG), "-9223372036854775808", null);
		checkConstProperties(list.item(nodeCount++), "LONG2", getTypeName(EtchGrammarConstants.LONG), "-1", null);
		checkConstProperties(list.item(nodeCount++), "LONG3", getTypeName(EtchGrammarConstants.LONG), "0", null);
		checkConstProperties(list.item(nodeCount++), "LONG4", getTypeName(EtchGrammarConstants.LONG), "1", null);
		checkConstProperties(list.item(nodeCount++), "LONG5", getTypeName(EtchGrammarConstants.LONG), "9223372036854775807", null);
		
		checkConstProperties(list.item(nodeCount++), "FLOAT1", getTypeName(EtchGrammarConstants.FLOAT), "1.4e-45", null);
		checkConstProperties(list.item(nodeCount++), "FLOAT2", getTypeName(EtchGrammarConstants.FLOAT), "-1", null);
		checkConstProperties(list.item(nodeCount++), "FLOAT3", getTypeName(EtchGrammarConstants.FLOAT), "0", null);
		checkConstProperties(list.item(nodeCount++), "FLOAT4", getTypeName(EtchGrammarConstants.FLOAT), "1", null);
		checkConstProperties(list.item(nodeCount++), "FLOAT5", getTypeName(EtchGrammarConstants.FLOAT), "3.4028235e38", null);
		
		checkConstProperties(list.item(nodeCount++), "DOUBLE1", getTypeName(EtchGrammarConstants.DOUBLE), "4.9e-324", null);
		checkConstProperties(list.item(nodeCount++), "DOUBLE2", getTypeName(EtchGrammarConstants.DOUBLE), "-1", null);
		checkConstProperties(list.item(nodeCount++), "DOUBLE3", getTypeName(EtchGrammarConstants.DOUBLE), "0", null);
		checkConstProperties(list.item(nodeCount++), "DOUBLE4", getTypeName(EtchGrammarConstants.DOUBLE), "1", null);
		checkConstProperties(list.item(nodeCount++), "DOUBLE5", getTypeName(EtchGrammarConstants.DOUBLE), "1.7976931348623157e308", null);
		
		checkConstProperties(list.item(nodeCount++), "STRING1", getTypeName(EtchGrammarConstants.STRING), "", null);
		checkConstProperties(list.item(nodeCount++), "STRING2", getTypeName(EtchGrammarConstants.STRING), "a", null);
		checkConstProperties(list.item(nodeCount++), "STRING3", getTypeName(EtchGrammarConstants.STRING), "abc", null);
		checkConstProperties(list.item(nodeCount++), "STRING4", getTypeName(EtchGrammarConstants.STRING), "a\tb\tc\r\n", null);
		checkConstProperties(list.item(nodeCount++), "STRING5", getTypeName(EtchGrammarConstants.STRING), "x\u0020x", null);
		checkConstProperties(list.item(nodeCount++), "STRING6", getTypeName(EtchGrammarConstants.STRING), "x\u00bfx", null);
		checkConstProperties(list.item(nodeCount++), "STRING7", getTypeName(EtchGrammarConstants.STRING), "x\u03a9x", null);
		checkConstProperties(list.item(nodeCount++), "STRING8", getTypeName(EtchGrammarConstants.STRING), "x\\x", null);
		checkConstProperties(list.item(nodeCount++), "STRING9", getTypeName(EtchGrammarConstants.STRING), "x'x", null);
		checkConstProperties(list.item(nodeCount++), "STRING10", getTypeName(EtchGrammarConstants.STRING), "x\"x", null);
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testExterns() throws Exception
	{
		// verify consts
		NodeList list = document.getElementsByTagName("extern");
		
		assertEquals(list.getLength(), 1);
		
		int nodeCount = 0;
		
		
		
		Node externNode = list.item(nodeCount++);
		
		checkExternProperties(externNode, "Blob", "-1308039838", "org.apache.etch.tests.Test1.Blob");
		checkExternMapProperties(externNode, "java", "StrIntHashMap", "org.apache.etch.bindings.java.util.StrIntHashMap", "StrIntHashMapSerializer",  "org.apache.etch.bindings.java.util.StrIntHashMapSerializer");
		checkExternMapProperties(externNode, "csharp", "StrIntHashMap", "Org.Apache.Etch.Bindings.Csharp.Util", "StrIntHashMapSerializer",  "Org.Apache.Etch.Bindings.Csharp.Util");
	}
		
	/** @throws Exception */
	@org.junit.Test
	public void testEnums() throws Exception
	{
		// verify enums
		NodeList list = document.getElementsByTagName("enum");
		
		assertEquals(list.getLength(), 1);
		
		int nodeCount = 0;
		
		Node enumNode = list.item(nodeCount++);
		
		checkEnumProperties(enumNode, "E1", "874541489", "org.apache.etch.tests.Test1.E1", "blah");
		checkEnumValueProperties(enumNode, "A", "352988284");
		checkEnumValueProperties(enumNode, "B", "352988285");
		checkEnumValueProperties(enumNode, "C", "352988286");
		
		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testStructs() throws Exception
	{
		// verify structs
		NodeList list = document.getElementsByTagName("struct");
		
		assertEquals(list.getLength(), 4);
		
		int nodeCount = 0;
		
		Node structNode = list.item(nodeCount++);
		
		checkStructProperties(structNode, "S1", "875459875", "org.apache.etch.tests.Test1.S1", "null", "blah");
		checkStructValueProperties(structNode, "x", "352988339", "x", "int", "true", "false", null, "value");
		checkStructValueProperties(structNode, "y", "352988340", "y", "int", "true", "false", null, "value");
		checkStructValueProperties(structNode, "z", "352988341", "z", "int", "true", "false", null, "value");
		
	
	}
		
	/** @throws Exception */
	@org.junit.Test
	public void textExceptions() throws Exception
	{
		// verify structs
		NodeList exceptionsNodeList = document.getElementsByTagName("exceptions");
		
		assertEquals(exceptionsNodeList.getLength(), 1);
		
		Node exceptionsNode = exceptionsNodeList.item(0);
		
		ArrayList<Node>list = new ArrayList<Node>();
			
		NodeList exceptionsNodeChildren = exceptionsNode.getChildNodes();
		for(int i = 0; i < exceptionsNodeChildren.getLength(); i++)
		{
			if(exceptionsNodeChildren.item(i).getNodeName() == "exception")
			{
				list.add(exceptionsNodeChildren.item(i));
			}
		}
		
		
		assertEquals(list.size(), 6);
		
		int nodeCount = 0;
		
		Node exceptionNode = list.get(nodeCount++);
		
		checkExceptionProperties(exceptionNode, "Excp1", "false", "-1446075732", "org.apache.etch.tests.Test1.Excp1", "null", "blah");
		checkExceptionValueProperties(exceptionNode, "msg", "769750364", "msg", "string", "false", null, "value");
		checkExceptionValueProperties(exceptionNode, "code", "-325551150", "code", "int", "false", null, "value");
		
		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testMethods() throws Exception
	{
		// verify structs
		NodeList list = document.getElementsByTagName("method");
		
		/**
		 * There are 12 methods, and 11 of them have result/response
		 * messages => 23 total methods
		 */
		assertEquals(150, list.getLength());
		
		int nodeCount = 0;
		
		Node methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "nothing", "1830690024", "org.apache.etch.tests.Test1.nothing", "free", "false", "both", null, "do absolutely nothing", null);
		checkAuthorize(methodNode, null);
		checkResult(methodNode, "793503461", "org.apache.etch.tests.Test1._result_nothing", "void", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_nothing", "793503461", "org.apache.etch.tests.Test1._result_nothing", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "incr", "-914174311", "org.apache.etch.tests.Test1.incr", "queued", "false", "both", null, "Increments a value by one.", null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "x", "352988339", "x", "int", "true", "false", null, "a value");
		checkResult(methodNode, "-507533604", "org.apache.etch.tests.Test1._result_incr", "int", "true", "false", null, "a value one larger.");
		
	/*	methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_incr", "-1982489532", "etch.bindings.xml.compiler.test.Test._result_incr", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null);
				
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "sum", "874329470", "etch.bindings.xml.compiler.test.Test.sum", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "x", "352988339", "x", "int", "true", "true", "1", null);
		checkResult(methodNode, "98561147", "etch.bindings.xml.compiler.test.Test._result_sum", "int", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_sum", "98561147", "etch.bindings.xml.compiler.test.Test._result_sum", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null);
				
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "fill", "-1511270800", "etch.bindings.xml.compiler.test.Test.fill", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "n", "352988329", "n", "int", "true", "false", null, null);
		checkMethodFieldProperties(methodNode, "x", "352988339", "x", "int", "true", "false", null, null);
		checkResult(methodNode, "-70000973", "etch.bindings.xml.compiler.test.Test._result_fill", "int", "true", "true", "1", null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_fill", "-70000973", "etch.bindings.xml.compiler.test.Test._result_fill", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "alwaysWorks", "899313734", "etch.bindings.xml.compiler.test.Test.alwaysWorks", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, "isTrue");
		checkResult(methodNode, "-253629117", "etch.bindings.xml.compiler.test.Test._result_alwaysWorks", "int", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_alwaysWorks", "-253629117", "etch.bindings.xml.compiler.test.Test._result_alwaysWorks", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "isTrue", "-1995546491", "etch.bindings.xml.compiler.test.Test.isTrue", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, null);
		checkResult(methodNode, "2087745352", "etch.bindings.xml.compiler.test.Test._result_isTrue", "boolean", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_isTrue", "2087745352", "etch.bindings.xml.compiler.test.Test._result_isTrue", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "alwaysFails", "588983161", "etch.bindings.xml.compiler.test.Test.alwaysFails", "none", "true", "both", null, null, null);
		checkAuthorize(methodNode, "isFalse");
		checkResult(methodNode);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "isFalse", "1221405260", "etch.bindings.xml.compiler.test.Test.isFalse", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, null);
		checkResult(methodNode, "1152631881", "etch.bindings.xml.compiler.test.Test._result_isFalse", "boolean", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_isFalse", "1152631881", "etch.bindings.xml.compiler.test.Test._result_isFalse", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "m3", "-563514125", "etch.bindings.xml.compiler.test.Test.m3", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, "can_m1");
		checkResult(methodNode, "-1307038026", "etch.bindings.xml.compiler.test.Test._result_m3", "void", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_m3", "-1307038026", "etch.bindings.xml.compiler.test.Test._result_m3", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "can_m1", "-307006048", "etch.bindings.xml.compiler.test.Test.can_m1", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, null);
		checkResult(methodNode, "-518681501", "etch.bindings.xml.compiler.test.Test._result_can_m1", "boolean", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_can_m1", "-518681501", "etch.bindings.xml.compiler.test.Test._result_can_m1", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "m1", "-563514127", "etch.bindings.xml.compiler.test.Test.m1", "none", "false", "client", null, "Test\n1\n2", null);
		checkAuthorize(methodNode, null);
		checkException(methodNode, "Excp1", "etch.bindings.xml.compiler.test.Test.Excp1", "Exception test");
		checkMethodFieldProperties(methodNode, "x", "352988339", "x", "boolean", "true", "false", null, "hi");
		checkResult(methodNode, "-1307038028", "etch.bindings.xml.compiler.test.Test._result_m1", "void", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_m1", "-1307038028", "etch.bindings.xml.compiler.test.Test._result_m1", 
				"none", "true", "server", "4000", null, "result");
		checkAuthorize(methodNode, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "m2", "-563514126", "etch.bindings.xml.compiler.test.Test.m2", "none", "false", "both", null, "Test\na line is here", null);
		checkAuthorize(methodNode, null);
		checkException(methodNode, "Excp1", "etch.bindings.xml.compiler.test.Test.Excp1", "Exception test value again");
		checkMethodFieldProperties(methodNode, "a", "352988316", "a", "string", "true", "false", null, "test for A parameter");
		checkMethodFieldProperties(methodNode, "b", "352988317", "b", "string", "true", "true", "1", "test for B parameter");
		checkResult(methodNode, "-1307038027", "etch.bindings.xml.compiler.test.Test._result_m2", "boolean", "true", "false", null, "There is much to say about void!");
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_m2", "-1307038027", "etch.bindings.xml.compiler.test.Test._result_m2", 
				"none", "true", "both", "4000", null, "result");
		checkAuthorize(methodNode, null); */
	}
	
	
	private void checkConstProperties(Node node, String constName, String constType, String constValue, String constDescription)
	{
		NamedNodeMap attrs = node.getAttributes();
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(node, new String[] {"name", "type", "value"}));

		
		
		// verify that no new stuff has snuck into the XML binding
		assertEquals(attrs.getLength(), 3);
		
		String nameAttr = attrs.getNamedItem("name").getNodeValue();
		String typeAttr = attrs.getNamedItem("type").getNodeValue();
		String valueAttr = attrs.getNamedItem("value").getNodeValue();
		
		// verify name of constant
		assertEquals(nameAttr, constName);
		
		// verify type of constant
		assertEquals(typeAttr, constType);
		
		// verify value of constant
		assertEquals(constValue, valueAttr);
		
	
	}
	
	private void checkExternProperties(Node node, String externName, String typeId, String typeName) throws Exception
	{
		NamedNodeMap attrs = node.getAttributes();
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(node, new String[] {"name", "typeId", "typeName"}));

		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(node, new String[] {"externmap"}));

		// verify that no new stuff has snuck into the XML binding
		assertEquals(attrs.getLength(), 3);
		
		String nameAttr = attrs.getNamedItem("name").getNodeValue();
		String typeIdAttr = attrs.getNamedItem("typeId").getNodeValue();
		String typeNameAttr = attrs.getNamedItem("typeName").getNodeValue();
		
		assertEquals(nameAttr, externName);
		assertEquals(typeIdAttr, typeId);
		assertEquals(typeNameAttr, typeName);
	}
	
	private void checkExternMapProperties(Node node, String language, String name, String nImport, String serializer, String sImport)
	{
		Node languageNode = null;
		
		NodeList children = node.getChildNodes();
		for(int i = 0; i < children.getLength(); i++)
		{
			if(children.item(i).getNodeType() == Node.ELEMENT_NODE && children.item(i).getNodeName() == "externmap")
			{
				Node thisNode = children.item(i);
				NamedNodeMap attrs = thisNode.getAttributes();
				Node languageAttr = attrs.getNamedItem("language");
				String thisLanguage = languageAttr.getNodeValue();
				if(thisLanguage.compareTo(language) == 0)
				{
					languageNode = children.item(i);
					break;
				}
			}
		}
		
		// verify that the language node exists
		assertNotNull(languageNode);
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(languageNode, new String[] {"language", "name", "nImport", "serializer", "sImport"}));

		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(languageNode, new String[] {"description"}));

		NamedNodeMap attrs = languageNode.getAttributes();
		
		// verify that no new stuff has snuck into the XML binding
		assertEquals(attrs.getLength(), 5);
		
		String nameAttr = attrs.getNamedItem("name").getNodeValue();
		String nImportAttr = attrs.getNamedItem("nImport").getNodeValue();
		String serializerAttr = attrs.getNamedItem("serializer").getNodeValue();
		String sImportAttr = attrs.getNamedItem("sImport").getNodeValue();
		
		// verify the value of the externmap name
		assertEquals(nameAttr, name);
		
		// verify the value of the externmap nImport
		assertEquals(nImportAttr, nImport);
		
		// verify the value of the externmap serializer
		assertEquals(serializerAttr, serializer);
		
		// verify the value of the externmap sImport
		assertEquals(sImportAttr, sImport);
		
		// verify that no new children nodes have snuck in
		assertEquals(getChildElementCount(languageNode), 0);
	}
	
	private void checkEnumProperties(Node enumNode, String enumName, String enumTypeId, String enumTypeName, String description)
	{
		NamedNodeMap attrs = enumNode.getAttributes();
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(enumNode, new String[] {"name", "typeId", "typeName"}));

		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(enumNode, new String[] {"description", "entry"}));

		// verify that no new stuff has snuck into the XML binding
		assertEquals(attrs.getLength(), 3);
		
		String nameAttr = attrs.getNamedItem("name").getNodeValue();
		String typeAttr = attrs.getNamedItem("typeId").getNodeValue();
		String valueAttr = attrs.getNamedItem("typeName").getNodeValue();
		
		// verify name of constant
		assertEquals(nameAttr, enumName);
		
		// verify type of constant
		assertEquals(typeAttr, enumTypeId);
		
		// verify value of constant
		assertEquals(valueAttr, enumTypeName);
		
		Node child = getDescriptionElement(enumNode);
		
		if(description == null)
		{
			assertNull(child);
		}
		else
		{	
			String descriptionValue = child.getTextContent().trim();
			assertEquals(descriptionValue, description);
			
			// verify that no new children on description
			assertEquals(getChildElementCount(child), 0);
		}
	}

	private void checkEnumValueProperties(Node enumNode, String valueName, String valueId)
	{
		Node valueNode = null;
		
		NodeList children = enumNode.getChildNodes();
		for(int i = 0; i < children.getLength(); i++)
		{
			if(children.item(i).getNodeType() == Node.ELEMENT_NODE && children.item(i).getNodeName() == "entry")
			{
				Node thisNode = children.item(i);
				NamedNodeMap attrs = thisNode.getAttributes();
				Node valueAttr = attrs.getNamedItem("value");
				String thisValue = valueAttr.getNodeValue();
				if(thisValue.compareTo(valueName) == 0)
				{
					valueNode = children.item(i);
					break;
				}
			}
		}
		
		// verify that the value node exists
		assertNotNull(valueNode);
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(valueNode, new String[] {"value", "entryId"}));

		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(valueNode, new String[] {"description"}));

		NamedNodeMap attrs = valueNode.getAttributes();
		
		// verify that no new stuff has snuck into the XML binding
		assertEquals(attrs.getLength(), 2);
		
		String entryIdAttr = attrs.getNamedItem("entryId").getNodeValue();
		
		// verify the value of the externmap name
		assertEquals(entryIdAttr, valueId);
		
		
		// verify that no new children nodes have snuck in
		assertEquals(getChildElementCount(valueNode), 0);
	}
	
	private void checkStructProperties(Node structNode, String structName, String structTypeId, String structTypeName, String baseType, String description)
	{
		NamedNodeMap attrs = structNode.getAttributes();
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(structNode, new String[] {"name", "typeId", "typeName", "baseType"}));

		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(structNode, new String[] {"description", "field"}));

		// verify that no new stuff has snuck into the XML binding
		assertEquals(attrs.getLength(), 4);
		
		String nameAttr = attrs.getNamedItem("name").getNodeValue();
		String typeAttr = attrs.getNamedItem("typeId").getNodeValue();
		String valueAttr = attrs.getNamedItem("typeName").getNodeValue();
		String baseTypeAttr = attrs.getNamedItem("baseType").getNodeValue();
		
		// verify name of constant
		assertEquals(nameAttr, structName);
		
		// verify type of constant
		assertEquals(typeAttr, structTypeId);
		
		// verify value of constant
		assertEquals(valueAttr, structTypeName);
		
		assertEquals(baseTypeAttr, baseType );
		
		Node child = getDescriptionElement(structNode);
		
		if(description == null)
		{
			assertNull(child);
		}
		else
		{	
			String descriptionValue = child.getTextContent().trim();
			assertEquals(descriptionValue, description);
			
			// verify that no new children on description
			assertEquals(getChildElementCount(child), 0);
		}
	}
	
	private void checkStructValueProperties(Node structNode, String structName, String fieldId, String fieldName, 
		String type, String isPrimitiveType, String isArray, String dimension, String description)
	{
		Node fieldNode = null;
		
		NodeList children = structNode.getChildNodes();
		for(int i = 0; i < children.getLength(); i++)
		{
			if(children.item(i).getNodeType() == Node.ELEMENT_NODE && children.item(i).getNodeName() == "field")
			{
				Node thisNode = children.item(i);
				NamedNodeMap attrs = thisNode.getAttributes();
				Node valueAttr = attrs.getNamedItem("name");
				String thisValue = valueAttr.getNodeValue();
				if(thisValue.compareTo(structName) == 0)
				{
					fieldNode = children.item(i);
					break;
				}
			}
		}
		
		// verify that the value node exists
		assertNotNull(fieldNode);
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(fieldNode, new String[] {"name", "fieldId", "fieldName", "type", "isPrimitiveType", "isArray", "dimension"}));

		//verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(fieldNode, new String[] {"description"}));
		
		NamedNodeMap attrs = fieldNode.getAttributes();
		
		// verify that no new stuff has snuck into the XML binding
		// dimension attr is nullable, meaning it might just not be there if isArray = "false"
		assertEquals(attrs.getLength(), isArray == "false" ? 6 : 7);
		
		String fieldIdAttr = attrs.getNamedItem("fieldId").getNodeValue();
		String fieldNameAttr = attrs.getNamedItem("fieldName").getNodeValue();
		String typeAttr = attrs.getNamedItem("type").getNodeValue();
		String isArrayAttr = attrs.getNamedItem("isArray").getNodeValue();
		String isPrimitiveAttr = attrs.getNamedItem( "isPrimitiveType" ).getNodeValue();
		
		Node dimensionNode = attrs.getNamedItem("dimension");
		String dimensionAttr = dimensionNode == null ? dimension : dimensionNode.getNodeValue();
		
		
		// verify the value of the externmap name
		assertEquals(fieldIdAttr, fieldId);
		assertEquals(fieldNameAttr, fieldName);
		assertEquals(typeAttr, type);
		assertEquals(isArrayAttr, isArray);
		assertEquals(dimensionAttr, dimension);
		assertEquals( isPrimitiveAttr, isPrimitiveType );
		
		Node child = getDescriptionElement(fieldNode);
		
		if(description == null)
		{
			assertNull(child);
		}
		else
		{	
			String descriptionValue = child.getTextContent().trim();
			assertEquals(descriptionValue, description);
			
			// verify that no new children on description
			assertEquals(getChildElementCount(child), 0);
		}
	}
	
	private void checkExceptionProperties(Node exceptionNode, String name, String isUnchecked, String exceptionId, 
			String exceptionName, String baseTypeName, String description)
	{
		NamedNodeMap attrs = exceptionNode.getAttributes();
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(exceptionNode, 
				new String[] {"name", "isUnchecked", "typeId", "typeName", "baseType"}));

		//verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(exceptionNode, new String[] {"description", "field"}));
		
		// verify that no new stuff has snuck into the XML binding
		assertEquals(attrs.getLength(), 5);
		
		String nameAttr = attrs.getNamedItem("name").getNodeValue();
		String isUncheckedAttr = attrs.getNamedItem("isUnchecked").getNodeValue();
		String typeAttr = attrs.getNamedItem("typeId").getNodeValue();
		String typeName = attrs.getNamedItem("typeName").getNodeValue();
		String baseTypeAttr = attrs.getNamedItem("baseType").getNodeValue();
		
		// verify name of constant
		assertEquals(nameAttr, name);
		
		// verify type of constant
		assertEquals(isUncheckedAttr, isUnchecked);
		
		// verify type of constant
		assertEquals(typeAttr, exceptionId);
		
		// verify value of constant
		assertEquals(typeName, exceptionName);
		
		assertEquals( baseTypeAttr, baseTypeName );
		
		Node child = getDescriptionElement(exceptionNode);
		
		if(description == null)
		{
			assertNull(child);
		}
		else
		{	
			String descriptionValue = child.getTextContent().trim();
			assertEquals(descriptionValue, description);
			
			// verify that no new children on description
			assertEquals(getChildElementCount(child), 0);
		}
	}
	
	private void checkExceptionValueProperties(Node exceptionNode, String name, String fieldId, String fieldName, String type, String isArray, String dimension, String description)
	{
		Node fieldNode = null;
		
		NodeList children = exceptionNode.getChildNodes();
		for(int i = 0; i < children.getLength(); i++)
		{
			if(children.item(i).getNodeType() == Node.ELEMENT_NODE && children.item(i).getNodeName() == "field")
			{
				Node thisNode = children.item(i);
				NamedNodeMap attrs = thisNode.getAttributes();
				Node valueAttr = attrs.getNamedItem("name");
				String thisValue = valueAttr.getNodeValue();
				if(thisValue.compareTo(name) == 0)
				{
					fieldNode = children.item(i);
					break;
				}
			}
		}
			
		// verify that the value node exists
		assertNotNull(fieldNode);
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(fieldNode, new String[] {"name", "fieldId", "fieldName", "type", "isArray", "dimension"}));

//		verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(fieldNode, new String[] {"description", "field"}));
	
		NamedNodeMap attrs = fieldNode.getAttributes();
		
		// verify that no new stuff has snuck into the XML binding
		// dimension attr is nullable, meaning it might just not be there if isArray = "false"
		assertEquals(attrs.getLength(), isArray == "false" ? 5 : 6);
		
		String fieldIdAttr = attrs.getNamedItem("fieldId").getNodeValue();
		String fieldNameAttr = attrs.getNamedItem("fieldName").getNodeValue();
		String typeAttr = attrs.getNamedItem("type").getNodeValue();
		String isArrayAttr = attrs.getNamedItem("isArray").getNodeValue();
		
		Node dimensionNode = attrs.getNamedItem("dimension");
		String dimensionAttr = dimensionNode == null ? dimension : dimensionNode.getNodeValue();
		
		assertEquals(fieldIdAttr, fieldId);
		assertEquals(fieldNameAttr, fieldName);
		assertEquals(typeAttr, type);
		assertEquals(isArrayAttr, isArray);
		assertEquals(dimensionAttr, dimension);
		
		Node child = getDescriptionElement(fieldNode);
		
		if(description == null)
		{
			assertNull(child);
		}
		else
		{	
			String descriptionValue = child.getTextContent().trim();
			assertEquals(descriptionValue, description);
			
			// verify that no new children on description
			assertEquals(getChildElementCount(child), 0);
		}
	}
	

	private void checkMethodProperties(Node methodNode, String name, String methodId, String methodName, 
			String pool, String isOneway, String direction, String timeout, String description, String result)
	{
		NamedNodeMap attrs = methodNode.getAttributes();
		boolean isResultMessage = false;
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(methodNode, new String[] {"name", "typeId", "typeName", "asyncReceiverMode", "isOneway", "messageDirection", "timeout",
				"responseField"}));

		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(methodNode, new String[] {"description", "exception", "authorize", "field", "result"}));
	
		// verify that no new stuff has snuck into the XML binding
		isResultMessage = name.startsWith("_result");
		
		assertEquals(isResultMessage? 8 : 6, attrs.getLength());
		
		String nameAttr = attrs.getNamedItem("name").getNodeValue();
		String typeIdAttr = attrs.getNamedItem("typeId").getNodeValue();
		String typeNameAttr = attrs.getNamedItem("typeName").getNodeValue();
		String poolAttr = attrs.getNamedItem("asyncReceiverMode").getNodeValue();
		String isOnewayAttr = attrs.getNamedItem("isOneway").getNodeValue();
		String directionAttr = attrs.getNamedItem("messageDirection").getNodeValue();
		String timeoutAttr = "";
		String resultAttr = "";
		
		if ( isResultMessage )
		{
			timeoutAttr = attrs.getNamedItem("timeout").getNodeValue();
			resultAttr = attrs.getNamedItem("responseField").getNodeValue();
		}
		
		// verify name of method
		assertEquals(nameAttr, name);
		
		// verify type of method
		assertEquals(typeIdAttr, methodId);
		
		// verify value of method
		assertEquals(typeNameAttr, methodName);
		
		// verify value of pool
		assertEquals(poolAttr, pool);
		
		// verify value of isOneway
		assertEquals(isOnewayAttr, isOneway);
		
		// verify value of isOneway
		assertEquals(directionAttr, direction);
		
		// verify value of isOneway
		if ( isResultMessage )
		{
			assertEquals(timeoutAttr, timeout);
			assertEquals(resultAttr, result);
		}
		Node child = getDescriptionElement(methodNode);
		
		if(description == null)
		{
			assertNull(child);
		}
		else
		{	
			String descriptionValue = child.getTextContent().trim();
			assertEquals(descriptionValue, description);
			
			// verify that no new children on description
			assertEquals(getChildElementCount(child), 0);
		}
	}
	
	private void checkAuthorize(Node methodNode, String authorizeMethodName)
	{
		Node child = getAuthorizeElement(methodNode);
		
		if(authorizeMethodName == null)
		{
			assertNull(child);
		}
		else
		{
			// verify that no unknown attr exist
			assertTrue(onlyAttrsOfName(child, new String[] {"methodName"}));

			// verify that no unknown child nodes exist
			assertTrue(onlyChildNodesOfName(child, new String[] {}));
		
			String authorizeAttr = child.getAttributes().getNamedItem("methodName").getNodeValue();
			
			assertEquals(authorizeAttr, authorizeMethodName);
			
			// verify that no new children on description
			assertEquals(getChildElementCount(child), 0);
		}
	}
	
//	private void checkException(Node methodNode, String exceptionName, String exceptionType, String description)
//	{
//		Node exceptionNode = null;
//		
//		NodeList children = methodNode.getChildNodes();
//		for(int i = 0; i < children.getLength(); i++)
//		{
//			if(children.item(i).getNodeType() == Node.ELEMENT_NODE && children.item(i).getNodeName() == "exception")
//			{
//				Node thisNode = children.item(i);
//				NamedNodeMap attrs = thisNode.getAttributes();
//				Node valueAttr = attrs.getNamedItem("name");
//				String thisValue = valueAttr.getNodeValue();
//				if(thisValue.compareTo(exceptionName) == 0)
//				{
//					exceptionNode = children.item(i);
//					break;
//				}
//			}
//		}
//		
//		assertEquals( exceptionNode.getAttributes().getNamedItem("type").getNodeValue(), exceptionType );
//		
//		// verify that the value node exists
//		assertNotNull(exceptionNode);
//		
//		// verify that no unknown attr exist
//		assertTrue(onlyAttrsOfName(exceptionNode, new String[] {"name","type"}));
//
//		
//		// verify that no unknown child nodes exist
//		assertTrue(onlyChildNodesOfName(exceptionNode, new String[] {"description"}));
//	
//		Node child = getDescriptionElement(exceptionNode);
//		
//		if(description == null)
//		{
//			assertNull(child);
//		}
//		else
//		{	
//			String descriptionValue = child.getTextContent().trim();
//			assertEquals(descriptionValue, description);
//			
//			// verify that no new children on description
//			assertEquals(getChildElementCount(child), 0);
//		}
//		
//	}
	
	private void checkResult(Node methodNode, String fieldId, String fieldName, String type, String isPrimitiveType,
		String isArray, String dimension, String description)
	{
		Node resultNode = getResultElement(methodNode);
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(resultNode, new String[] {"fieldId", "fieldName", "type", "isPrimitiveType", "isArray", "dimension"}));

		// verify that the value node exists
		assertNotNull(resultNode);
		
		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(resultNode, new String[] {"description"}));
	
		NamedNodeMap attrs = resultNode.getAttributes();
		
		// verify that no new stuff has snuck into the XML binding
		
		// this is special case of a oneway message, so we should only have 'type=void' and 'isArray=true' specified.
		String fieldIdAttr = null;
		String fieldNameAttr = null;
		
		if(fieldId == null)
		{
			assertEquals(attrs.getLength(), 3);
			
		}
		else // dimension attr is nullable, meaning it might just not be there if isArray = "false"
		{
			fieldIdAttr = attrs.getNamedItem("fieldId").getNodeValue();
			fieldNameAttr = attrs.getNamedItem("fieldName").getNodeValue();
			
			assertEquals(attrs.getLength(), isArray == "false" ? 5 : 6);
		}
		
		String typeAttr = attrs.getNamedItem("type").getNodeValue();
		String isArrayAttr = attrs.getNamedItem("isArray").getNodeValue();

		Node dimensionNode = attrs.getNamedItem("dimension");
		String dimensionAttr = dimensionNode == null ? dimension : dimensionNode.getNodeValue();
		
		assertEquals(fieldIdAttr, fieldId);
		assertEquals(fieldNameAttr, fieldName);
		assertEquals(typeAttr, type);
		assertEquals(isArrayAttr, isArray);
		assertEquals(dimensionAttr, dimension);
		
		Node child = getDescriptionElement(resultNode);
		
		if(description == null)
		{
			assertNull(child);
		}
		else
		{	
			String descriptionValue = child.getTextContent().trim();
			assertEquals(descriptionValue, description);
			
			// verify that no new children on description
			assertEquals(getChildElementCount(child), 0);
		}
	}
	
	private void checkMethodFieldProperties(Node methodNode, String name, String fieldId, String fieldName, String type, 
			String isPrimitiveType, String isArray, String dimension, String description)
	{
		Node fieldNode = null;
		
		NodeList children = methodNode.getChildNodes();
		for(int i = 0; i < children.getLength(); i++)
		{
			if(children.item(i).getNodeType() == Node.ELEMENT_NODE && children.item(i).getNodeName() == "field")
			{
				Node thisNode = children.item(i);
				NamedNodeMap attrs = thisNode.getAttributes();
				Node valueAttr = attrs.getNamedItem("name");
				String thisValue = valueAttr.getNodeValue();
				if(thisValue.compareTo(name) == 0)
				{
					fieldNode = children.item(i);
					break;
				}
			}
		}
		
		// verify that the value node exists
		assertNotNull(fieldNode);
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(fieldNode, new String[] {"name", "fieldId", "fieldName", "type", 
				"isPrimitiveType", "isArray", "dimension" }));

		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(fieldNode, new String[] {"description"}));
	
		NamedNodeMap attrs = fieldNode.getAttributes();
		
		// verify that no new stuff has snuck into the XML binding
		// dimension attr is nullable, meaning it might just not be there if isArray = "false"
		assertEquals(attrs.getLength(), isArray == "false" ? 6 : 7);
		
		String fieldIdAttr = attrs.getNamedItem("fieldId").getNodeValue();
		String fieldNameAttr = attrs.getNamedItem("fieldName").getNodeValue();
		String typeAttr = attrs.getNamedItem("type").getNodeValue();
		String isArrayAttr = attrs.getNamedItem("isArray").getNodeValue();
		String isPrimitiveTypeAttr = attrs.getNamedItem("isPrimitiveType").getNodeValue();
		
		Node dimensionNode = attrs.getNamedItem("dimension");
		String dimensionAttr = dimensionNode == null ? dimension : dimensionNode.getNodeValue();
		
		assertEquals(fieldIdAttr, fieldId);
		assertEquals(fieldNameAttr, fieldName);
		assertEquals(typeAttr, type);
		assertEquals(isArrayAttr, isArray);
		assertEquals(dimensionAttr, dimension);
		assertEquals( isPrimitiveType, isPrimitiveTypeAttr );
		
		Node child = getDescriptionElement(fieldNode);
		
		if(description == null)
		{
			assertNull(child);
		}
		else
		{	
			String descriptionValue = child.getTextContent().trim();
			assertEquals(descriptionValue, description);
			
			// verify that no new children on description
			assertEquals(getChildElementCount(child), 0);
		}
	}
	
	private Node getDescriptionElement(Node node)
	{
		NodeList children = node.getChildNodes();
		for(int i = 0; i < children.getLength(); i++)
		{
			short nodeType = children.item(i).getNodeType();
			String nodeName = children.item(i).getNodeName();
			if(nodeType == Node.ELEMENT_NODE && nodeName == "description")
			{
				return children.item(i);
			}
		}
		return null;
	}
	
	private Node getAuthorizeElement(Node node)
	{
		NodeList children = node.getChildNodes();
		for(int i = 0; i < children.getLength(); i++)
		{
			short nodeType = children.item(i).getNodeType();
			String nodeName = children.item(i).getNodeName();
			if(nodeType == Node.ELEMENT_NODE && nodeName == "authorize")
			{
				return children.item(i);
			}
		}
		return null;
	}
	
	private Node getResultElement(Node node)
	{
		NodeList children = node.getChildNodes();
		for(int i = 0; i < children.getLength(); i++)
		{
			short nodeType = children.item(i).getNodeType();
			String nodeName = children.item(i).getNodeName();
			if(nodeType == Node.ELEMENT_NODE && nodeName == "result")
			{
				return children.item(i);
			}
		}
		return null;
	}
	
	private int getChildElementCount(Node node)
	{
		int count = 0;
		
		NodeList children = node.getChildNodes();
		for(int i = 0; i < children.getLength(); i++)
		{
			if(children.item(i).getNodeType() == Node.ELEMENT_NODE)
			{
				count++;
			}
		}
		
		return count;
	}
	
	private String getTypeName(int etchGrammarConstantsLookup)
	{
		switch (etchGrammarConstantsLookup)
		{
			case EtchGrammarConstants.VOID: return "void";
			case EtchGrammarConstants.BOOLEAN: return "boolean";
			case EtchGrammarConstants.BYTE: return "byte";
			case EtchGrammarConstants.SHORT: return "short";
			case EtchGrammarConstants.INT: return "int";
			case EtchGrammarConstants.LONG: return "long";
			case EtchGrammarConstants.FLOAT: return "float";
			case EtchGrammarConstants.DOUBLE: return "double";
			case EtchGrammarConstants.STRING: return "string";
			default: return null;
		}
	}
	
	private boolean onlyChildNodesOfName(Node node, String[] elementNamesToCheck)
	{
		NodeList children = node.getChildNodes();
		
		boolean hasValidChildren = true;
		
		for(int i = 0; i < children.getLength(); i++)
		{
		
			if(children.item(i).getNodeType() == Node.ELEMENT_NODE)
			{
				String elementName = children.item(i).getNodeName();
				
				boolean matches = false;
				for (String element : elementNamesToCheck)
				{
					if(0 == elementName.compareTo(element) )
					{
						matches = true;
						break;
					}
				}
				
				if(matches == false)
				{
					hasValidChildren = false;
					break;
				}
			}	
		}
		
		return hasValidChildren;
	}
	
	private boolean onlyAttrsOfName(Node node, String[] attrNamesToCheck)
	{
		NamedNodeMap attrs = node.getAttributes();
		
		boolean hasValidAttrs = true;
		
		for(int i = 0; i < attrs.getLength(); i++)
		{
			String attrName = attrs.item(i).getNodeName();
			
			boolean matches = false;
			for (String element : attrNamesToCheck)
			{
				if(0 == attrName.compareTo(element) )
				{
					matches = true;
					break;
				}
			}
			
			if(matches == false)
			{
				hasValidAttrs = false;
				break;
			}
			
		}
		
		return hasValidAttrs;
	}
}
