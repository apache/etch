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

import org.junit.After;
import org.junit.Before;
import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;


/** */
public class TestMixinXml 
{

	Document document;

	/** @throws Exception */
	@Before
	public void setUp() throws Exception {
		// Load Test.xml into DOM
		DocumentBuilder builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
		//String userDir = System.getProperty("user.dir");
		try
		{
			File f = new File("tests/target/generated-sources/main/etch/xml/org/apache/etch/tests/Foo.xml");
			
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
		
		Node child = getDescriptionElement(list.item(0));
		assertNull(child);
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testService() throws Exception
	{
		NodeList list = document.getElementsByTagName("service");
		
		assertEquals(list.getLength(), 1);
		
		String serviceName = list.item(0).getAttributes().getNamedItem("name").getNodeValue();
		assertEquals(serviceName, "Foo");
		
		// verify that no unknown attr exist
		assertTrue(onlyAttrsOfName(list.item(0), new String[] {"name"}));
			
		// verify that no unknown child nodes exist
		assertTrue(onlyChildNodesOfName(list.item(0), new String[] {"description", "consts", "externs", "enums", "structs", "exceptions", "methods"}));
		
		Node child = getDescriptionElement(list.item(0));
		assertNull(child);
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testConsts() throws Exception
	{
		// verify consts
		NodeList list = document.getElementsByTagName("const");
		
		// no consts
		assertEquals(list.getLength(), 0);
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testExterns() throws Exception
	{
		// verify consts
		NodeList list = document.getElementsByTagName("extern");
		
		assertEquals(list.getLength(), 3);
		
		int nodeCount = 0;
		
		Node externNode = list.item(nodeCount++);
		
		checkExternProperties(externNode, "FooTable", "-1404806234", "org.apache.etch.tests.Foo.FooTable");
		checkExternMapProperties(externNode, "java", "org.apache.etch.bindings.java.util.StrStrHashMap", "", "org.apache.etch.bindings.java.util.StrStrHashMapSerializer",  "");
		
		externNode = list.item( nodeCount++ );
		checkExternProperties(externNode, "BazTable", "729647100", "org.apache.etch.tests.Baz.BazTable");
		checkExternMapProperties(externNode, "java", "org.apache.etch.bindings.java.util.StrStrHashMap", "", "org.apache.etch.bindings.java.util.StrStrHashMapSerializer",  "");
		
		externNode = list.item( nodeCount++ );
		checkExternProperties(externNode, "BarTable", "-278631412", "org.apache.etch.tests.Bar.BarTable");
		checkExternMapProperties(externNode, "java", "org.apache.etch.bindings.java.util.StrStrHashMap", "", "org.apache.etch.bindings.java.util.StrStrHashMapSerializer",  "");
	}
		
	/** @throws Exception */
	@org.junit.Test
	public void testEnums() throws Exception
	{
		// verify enums
		NodeList list = document.getElementsByTagName("enum");
		
		assertEquals(list.getLength(), 0);
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testStructs() throws Exception
	{
		// verify structs
		NodeList list = document.getElementsByTagName("struct");
		
		assertEquals(list.getLength(), 3);
		
		int nodeCount = 0;
		
		Node structNode = list.item(nodeCount++);
		
		checkStructProperties(structNode, "FooData", "-86983438", "org.apache.etch.tests.Foo.FooData", "null", null);
		checkStructValueProperties(structNode, "code", "-325551150", "code", "int", "true", "false", null, null);
		checkStructValueProperties(structNode, "fooTable", "-900628371", "fooTable", "org.apache.etch.tests.Foo.FooTable", "false", "false", null, null);
		checkStructValueProperties(structNode, "barData", "-780728328", "barData", "org.apache.etch.tests.Bar.BarData", "false", "false", null, null);
		checkStructValueProperties(structNode, "bazData", "-254148608", "bazData", "org.apache.etch.tests.Baz.BazData", "false", "false", null, null);
		
		
		
		structNode = list.item(nodeCount++);
		
		checkStructProperties(structNode, "BazData", "467213084", "org.apache.etch.tests.Baz.BazData", "null", null);
		checkStructValueProperties(structNode, "code", "-325551150", "code", "int", "false", "false", null, null);
		checkStructValueProperties(structNode, "bazTable", "2073680920", "bazTable", "org.apache.etch.tests.Baz.BazTable", "false", "false", null, null);
		
		structNode = list.item(nodeCount++);
		
		checkStructProperties(structNode, "BarData", "1671967500", "org.apache.etch.tests.Bar.BarData", "null", null);
		checkStructValueProperties(structNode, "code", "-325551150", "code", "int", "false", "false", null, null);
		checkStructValueProperties(structNode, "barTable", "-902376928", "barTable", "org.apache.etch.tests.Bar.BarTable", "false", "false", null, null);
		checkStructValueProperties(structNode, "bazData", "-254148608", "bazData", "org.apache.etch.tests.Baz.BazData", "false", "false", null, null);
		
		
		
	}
		
	/** @throws Exception */
	@org.junit.Test
	public void textExceptions() throws Exception
	{
		// verify exceptions
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
		
		
		assertEquals(list.size(), 0);
		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void testMethods() throws Exception
	{
		// verify methods
		NodeList list = document.getElementsByTagName("method");
		
		assertEquals(18, list.getLength() );
		
		int nodeCount = 0;
		
		Node methodNode = list.item(nodeCount++);
		
		
		checkMethodProperties(methodNode, "foo1", "2107096233", "org.apache.etch.tests.Foo.foo1", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "fooData", "-1798637973", "fooData", 
				"org.apache.etch.tests.Foo.FooData", "false", "false", null, null);
		checkResult(methodNode, "222044908", "org.apache.etch.tests.Foo._result_foo1", "boolean", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_foo1", "222044908", "org.apache.etch.tests.Foo._result_foo1", 
				"none", "true", "both", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "foo2", "2107096234", "org.apache.etch.tests.Foo.foo2", "none", "false", "server", null, 
				null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "fooData", "-1798637973", "fooData", 
				"org.apache.etch.tests.Foo.FooData", "false", "false", null, null);
		checkResult(methodNode, "222044909", "org.apache.etch.tests.Foo._result_foo2", "boolean", "true",  "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_foo2", "222044909", "org.apache.etch.tests.Foo._result_foo2", 
				"none", "true", "client", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "foo3", "2107096235", "org.apache.etch.tests.Foo.foo3", "none", "false", "client", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "fooData", "-1798637973", "fooData", 
				"org.apache.etch.tests.Foo.FooData", "false", "false", null, null);
		checkResult(methodNode, "222044910", "org.apache.etch.tests.Foo._result_foo3", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_foo3", "222044910", "org.apache.etch.tests.Foo._result_foo3", 
				"none", "true", "server", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "baz1", "1844117247", "org.apache.etch.tests.Baz.baz1", "none", "false", "both", null, "Method Baz1 *", null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "bazData", "-254148608", "bazData", 
				"org.apache.etch.tests.Baz.BazData", "false", "false", null, null);
		checkResult(methodNode, "-1773449406", "org.apache.etch.tests.Baz._result_baz1", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_baz1", "-1773449406", "org.apache.etch.tests.Baz._result_baz1", 
				"none", "true", "both", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "baz2", "1844117248", "org.apache.etch.tests.Baz.baz2", "none", "false", "server", null, "Method Baz2 *", null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "bazData", "-254148608", "bazData", 
				"org.apache.etch.tests.Baz.BazData", "false", "false", null, null);
		checkResult(methodNode, "-1773449405", "org.apache.etch.tests.Baz._result_baz2", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_baz2", "-1773449405", "org.apache.etch.tests.Baz._result_baz2", 
				"none", "true", "client", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "baz3", "1844117249", "org.apache.etch.tests.Baz.baz3", "none", "false", "client", null, "Method Baz3 *", null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "bazData", "-254148608", "bazData", 
				"org.apache.etch.tests.Baz.BazData", "false", "false", null, null);
		checkResult(methodNode, "-1773449404", "org.apache.etch.tests.Baz._result_baz3", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_baz3", "-1773449404", "org.apache.etch.tests.Baz._result_baz3", 
				"none", "true", "server", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		
		
		
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "bar1", "-1132465393", "org.apache.etch.tests.Bar.bar1", "none", "false", "both", null, "Method Bar1 *", null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "barData", "-780728328", "barData", 
				"org.apache.etch.tests.Bar.BarData", "false", "false", null, null);
		checkResult(methodNode, "37892946", "org.apache.etch.tests.Bar._result_bar1", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_bar1", "37892946", "org.apache.etch.tests.Bar._result_bar1", 
				"none", "true", "both", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "bar2", "-1132465392", "org.apache.etch.tests.Bar.bar2", "none", "false", "server", null, "Method Bar2 *", null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "barData", "-780728328", "barData", 
				"org.apache.etch.tests.Bar.BarData", "false", "false", null, null);
		checkResult(methodNode, "37892947", "org.apache.etch.tests.Bar._result_bar2", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_bar2", "37892947", "org.apache.etch.tests.Bar._result_bar2", 
				"none", "true", "client", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);

		checkMethodProperties(methodNode, "bar3", "-1132465391", "org.apache.etch.tests.Bar.bar3", "none", "false", "client", null, "Method Bar3 *", null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "barData", "-780728328", "barData", 
				"org.apache.etch.tests.Bar.BarData", "false", "false", null, null);
		checkResult(methodNode, "37892948", "org.apache.etch.tests.Bar._result_bar3", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_bar3", "37892948", "org.apache.etch.tests.Bar._result_bar3", 
				"none", "true", "server", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
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
	
	private void checkStructProperties(Node structNode, String structName, String structTypeId, 
			String structTypeName, String baseTypeName, String description)
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
		
		assertEquals(baseTypeAttr, baseTypeName);
		
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
		assertTrue(onlyAttrsOfName(fieldNode, new String[] {"name", "fieldId", "fieldName", "type", "isPrimitiveType",
			"isArray", "dimension"}));

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
		
		Node dimensionNode = attrs.getNamedItem("dimension");
		String dimensionAttr = dimensionNode == null ? dimension : dimensionNode.getNodeValue();
		
		
		// verify the value of the externmap name
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
	
	private void checkMethodProperties(Node methodNode, String name, String methodId, String methodName, String pool, String isOneway, 
			String direction, String timeout, String description, String result )
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
	
	private void checkResult(Node methodNode, String fieldId, String fieldName, String type, 
		String isPrimitiveType, String isArray, String dimension, String description)
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
			assertEquals(attrs.getLength(), 2);
			
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
