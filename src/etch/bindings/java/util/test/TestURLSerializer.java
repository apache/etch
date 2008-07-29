package etch.bindings.java.util.test;


import static org.junit.Assert.assertEquals;

import org.junit.Test;

import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.util.URLSerializer;
import etch.util.URL;

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
		
		Type type = new Type( "url" );
		Class2TypeMap class2type = new Class2TypeMap();
		URLSerializer.init( type, class2type );
		ImportExportHelper helper = type.getImportExportHelper();
		
		StructValue sv = helper.exportValue( url );
//		System.out.println( "sv = "+sv );
		
		assertEquals( sv.type(), type );
		
		URL url2 = (URL) helper.importValue( sv );
//		System.out.println( "url2 = "+url2 );
		
		assertEquals( url, url2 );
	}
}
