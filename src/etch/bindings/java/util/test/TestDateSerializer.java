package etch.bindings.java.util.test;


import static org.junit.Assert.assertEquals;

import java.util.Date;

import org.junit.Test;

import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.util.DateSerializer;

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
		
		Type type = new Type( "date" );
		Class2TypeMap class2type = new Class2TypeMap();
		DateSerializer.init( type, class2type );
		ImportExportHelper helper = type.getImportExportHelper();
		
		StructValue sv = helper.exportValue( date );
//		System.out.println( "sv = "+sv );
		
		assertEquals( sv.type(), type );
		
		Date date2 = (Date) helper.importValue( sv );
//		System.out.println( "date2 = "+date2 );
		
		assertEquals( date, date2 );
	}
}
