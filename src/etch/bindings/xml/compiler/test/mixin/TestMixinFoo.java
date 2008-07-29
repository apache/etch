package etch.bindings.xml.compiler.test.mixin;

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
public class TestMixinFoo 
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
			File f = new File("src/etch/bindings/xml/compiler/test/mixin/Foo.xml");
			if (!f.exists())
				f = new File("etch/bindings/xml/compiler/test/mixin/Foo.xml");
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
		assertEquals(moduleName, "etch.bindings.xml.compiler.test.mixin");
		
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
		
		checkExternProperties(externNode, "FooTable", "-2079321780", "etch.bindings.xml.compiler.test.mixin.Foo.FooTable");
		checkExternMapProperties(externNode, "java", "etch.bindings.java.util.StrStrHashMap", "", "etch.bindings.java.util.StrStrHashMapSerializer",  "");
		
		externNode = list.item( nodeCount++ );
		checkExternProperties(externNode, "BazTable", "-1217703979", "etch.bindings.xml.compiler.test.mixin.baz.Baz.BazTable");
		checkExternMapProperties(externNode, "java", "etch.bindings.java.util.StrStrHashMap", "", "etch.bindings.java.util.StrStrHashMapSerializer",  "");
		
		externNode = list.item( nodeCount++ );
		checkExternProperties(externNode, "BarTable", "-414115347", "etch.bindings.xml.compiler.test.mixin.bar.Bar.BarTable");
		checkExternMapProperties(externNode, "java", "etch.bindings.java.util.StrStrHashMap", "", "etch.bindings.java.util.StrStrHashMapSerializer",  "");
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
		
		assertEquals(list.getLength(), 5);
		
		int nodeCount = 0;
		
		Node structNode = list.item(nodeCount++);
		
		checkStructProperties(structNode, "FooData", "-813744692", "etch.bindings.xml.compiler.test.mixin.Foo.FooData", "null", null);
		checkStructValueProperties(structNode, "code", "-325551150", "code", "int", "true", "false", null, null);
		checkStructValueProperties(structNode, "fooTable", "-900628371", "fooTable", "etch.bindings.xml.compiler.test.mixin.Foo.FooTable", "false", "false", null, null);
		checkStructValueProperties(structNode, "barData", "-780728328", "barData", "etch.bindings.xml.compiler.test.mixin.bar.Bar.BarData", "false", "false", null, null);
		checkStructValueProperties(structNode, "bazData", "-254148608", "bazData", "etch.bindings.xml.compiler.test.mixin.baz.Baz.BazData", "false", "false", null, null);
		
		structNode = list.item(nodeCount++);
		
		checkStructProperties(structNode, "ExtendBarData", "-692050817", "etch.bindings.xml.compiler.test.mixin.Foo.ExtendBarData", "etch.bindings.xml.compiler.test.mixin.bar.Bar.BarData", null);
		checkStructValueProperties(structNode, "blah", "-1131119658", "blah", "int", "true", "false", null, null);
		
		structNode = list.item(nodeCount++);
		
		checkStructProperties(structNode, "BazData", "457100547", "etch.bindings.xml.compiler.test.mixin.baz.Baz.BazData", "null", null);
		checkStructValueProperties(structNode, "code", "-325551150", "code", "int", "false", "false", null, null);
		checkStructValueProperties(structNode, "bazTable", "2073680920", "bazTable", "etch.bindings.xml.compiler.test.mixin.baz.Baz.BazTable", "false", "false", null, null);
		
		structNode = list.item(nodeCount++);
		
		checkStructProperties(structNode, "BarData", "-20120341", "etch.bindings.xml.compiler.test.mixin.bar.Bar.BarData", "null", null);
		checkStructValueProperties(structNode, "code", "-325551150", "code", "int", "false", "false", null, null);
		checkStructValueProperties(structNode, "barTable", "-902376928", "barTable", "etch.bindings.xml.compiler.test.mixin.bar.Bar.BarTable", "false", "false", null, null);
		checkStructValueProperties(structNode, "bazData", "-254148608", "bazData", "etch.bindings.xml.compiler.test.mixin.baz.Baz.BazData", "false", "false", null, null);
		
		structNode = list.item(nodeCount++);
		
		checkStructProperties(structNode, "ExtendBazData", "250433305", "etch.bindings.xml.compiler.test.mixin.bar.Bar.ExtendBazData", "etch.bindings.xml.compiler.test.mixin.baz.Baz.BazData", null);
		checkStructValueProperties(structNode, "extendBaz", "-1775885540", "extendBaz", "int", "true", "false", null, null);
		
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
		
		assertEquals(23, list.getLength() );
		
		int nodeCount = 0;
		
		Node methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "blahFunction", "-790682742", "etch.bindings.xml.compiler.test.mixin.FooServer.blahFunction", 
				"none", "false", "server", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "huh", "728574038", "huh",
				"etch.bindings.xml.compiler.test.mixin.Foo.ExtendBarData", "false", "false", null, null);
		checkResult(methodNode, "-560513339", "etch.bindings.xml.compiler.test.mixin.FooClient._result_blahFunction", "boolean", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_blahFunction", "-560513339", "etch.bindings.xml.compiler.test.mixin.FooClient._result_blahFunction", 
				"none", "true", "client", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "foo1", "2098780239", "etch.bindings.xml.compiler.test.mixin.Foo.foo1", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "fooData", "-1798637973", "fooData", 
				"etch.bindings.xml.compiler.test.mixin.Foo.FooData", "false", "false", null, null);
		checkResult(methodNode, "1685615250", "etch.bindings.xml.compiler.test.mixin.Foo._result_foo1", "boolean", "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_foo1", "1685615250", "etch.bindings.xml.compiler.test.mixin.Foo._result_foo1", 
				"none", "true", "both", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "foo2", "283135085", "etch.bindings.xml.compiler.test.mixin.FooServer.foo2", "none", "false", "server", null, 
				null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "fooData", "-1798637973", "fooData", 
				"etch.bindings.xml.compiler.test.mixin.Foo.FooData", "false", "false", null, null);
		checkResult(methodNode, "1753980328", "etch.bindings.xml.compiler.test.mixin.FooClient._result_foo2", "boolean", "true",  "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_foo2", "1753980328", "etch.bindings.xml.compiler.test.mixin.FooClient._result_foo2", 
				"none", "true", "client", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "foo3", "-193368730", "etch.bindings.xml.compiler.test.mixin.FooClient.foo3", "none", "false", "client", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "fooData", "-1798637973", "fooData", 
				"etch.bindings.xml.compiler.test.mixin.Foo.FooData", "false", "false", null, null);
		checkResult(methodNode, "1044540593", "etch.bindings.xml.compiler.test.mixin.FooServer._result_foo3", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_foo3", "1044540593", "etch.bindings.xml.compiler.test.mixin.FooServer._result_foo3", 
				"none", "true", "server", "4000", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "baz1", "-1078181928", "etch.bindings.xml.compiler.test.mixin.baz.Baz.baz1", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "bazData", "-254148608", "bazData", 
				"etch.bindings.xml.compiler.test.mixin.baz.Baz.BazData", "false", "false", null, null);
		checkResult(methodNode, "1718525467", "etch.bindings.xml.compiler.test.mixin.baz.Baz._result_baz1", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_baz1", "1718525467", "etch.bindings.xml.compiler.test.mixin.baz.Baz._result_baz1", 
				"none", "true", "both", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "baz2", "-1610709258", "etch.bindings.xml.compiler.test.mixin.baz.BazServer.baz2", "none", "false", "server", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "bazData", "-254148608", "bazData", 
				"etch.bindings.xml.compiler.test.mixin.baz.Baz.BazData", "false", "false", null, null);
		checkResult(methodNode, "2056690737", "etch.bindings.xml.compiler.test.mixin.baz.BazClient._result_baz2", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_baz2", "2056690737", "etch.bindings.xml.compiler.test.mixin.baz.BazClient._result_baz2", 
				"none", "true", "client", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "baz3", "-2087213073", "etch.bindings.xml.compiler.test.mixin.baz.BazClient.baz3", "none", "false", "client", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "bazData", "-254148608", "bazData", 
				"etch.bindings.xml.compiler.test.mixin.baz.Baz.BazData", "false", "false", null, null);
		checkResult(methodNode, "1347251002", "etch.bindings.xml.compiler.test.mixin.baz.BazServer._result_baz3", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_baz3", "1347251002", "etch.bindings.xml.compiler.test.mixin.baz.BazServer._result_baz3", 
				"none", "true", "server", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "blah", "-1521472624", "etch.bindings.xml.compiler.test.mixin.baz.BazServer.blah", 
				"none", "true", "server", null, null, null);
		checkAuthorize(methodNode, null);
		
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "someFunction", "-1371148389", "etch.bindings.xml.compiler.test.mixin.bar.BarServer.someFunction", "none", "false", "server", null, 
				null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "huhExtend", "-140454480", "huhExtend", 
				"etch.bindings.xml.compiler.test.mixin.bar.Bar.ExtendBazData", "false", "false", null, null);
		checkResult(methodNode, "-744050986", "etch.bindings.xml.compiler.test.mixin.bar.BarClient._result_someFunction", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_someFunction", "-744050986", "etch.bindings.xml.compiler.test.mixin.bar.BarClient._result_someFunction", 
				"none", "true", "client", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "bar1", "-2086985232", "etch.bindings.xml.compiler.test.mixin.bar.Bar.bar1", "none", "false", "both", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "barData", "-780728328", "barData", 
				"etch.bindings.xml.compiler.test.mixin.bar.Bar.BarData", "false", "false", null, null);
		checkResult(methodNode, "-48144333", "etch.bindings.xml.compiler.test.mixin.bar.Bar._result_bar1", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_bar1", "-48144333", "etch.bindings.xml.compiler.test.mixin.bar.Bar._result_bar1", 
				"none", "true", "both", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "bar2", "-906604274", "etch.bindings.xml.compiler.test.mixin.bar.BarServer.bar2", "none", "false", "server", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "barData", "-780728328", "barData", 
				"etch.bindings.xml.compiler.test.mixin.bar.Bar.BarData", "false", "false", null, null);
		checkResult(methodNode, "-1684912567", "etch.bindings.xml.compiler.test.mixin.bar.BarClient._result_bar2", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_bar2", "-1684912567", "etch.bindings.xml.compiler.test.mixin.bar.BarClient._result_bar2", 
				"none", "true", "client", "0", null, "result");
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "result", "-2130379326", "result",
				"boolean", "true", "false", null, null);
		
		methodNode = list.item(nodeCount++);

		checkMethodProperties(methodNode, "bar3", "-1383108089", "etch.bindings.xml.compiler.test.mixin.bar.BarClient.bar3", "none", "false", "client", null, null, null);
		checkAuthorize(methodNode, null);
		checkMethodFieldProperties(methodNode, "barData", "-780728328", "barData", 
				"etch.bindings.xml.compiler.test.mixin.bar.Bar.BarData", "false", "false", null, null);
		checkResult(methodNode, "1900614994", "etch.bindings.xml.compiler.test.mixin.bar.BarServer._result_bar3", "boolean",  "true", "false", null, null);

		methodNode = list.item(nodeCount++);
		
		checkMethodProperties(methodNode, "_result_bar3", "1900614994", "etch.bindings.xml.compiler.test.mixin.bar.BarServer._result_bar3", 
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
				for(int j = 0; j < elementNamesToCheck.length; j++)
				{
					if(0 == elementName.compareTo(elementNamesToCheck[j]) )
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
			for(int j = 0; j < attrNamesToCheck.length; j++)
			{
				if(0 == attrName.compareTo(attrNamesToCheck[j]) )
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
