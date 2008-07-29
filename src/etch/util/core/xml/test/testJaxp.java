/* $Id: testJaxp.java 21902 2006-04-12 13:28:38Z wert $
 *
 * Created by wert on Sep 16, 2005.
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util.core.xml.test;

import java.io.StringReader;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.CharacterData;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

import etch.util.Timer;

/**
 * Description of testJaxp.
 */
public class testJaxp
{
	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args )
		throws Exception
	{
		DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		DocumentBuilder db = dbf.newDocumentBuilder();
		Document d = db.parse( new InputSource( new StringReader( "<x>blah</x>" ) ) );
		System.out.println( "d = "+d );
		dump( d );
		
		dump( parse( db, TEST_MSG ) );
		
		if (false) return;
		
		int m = 5;
		int n = 50000*4;
		for (int j = 0; j < m; j++)
		{
			if (j != 0)
				Thread.sleep( 5000 );
			
			long t0 = Timer.currentTimeMillis();
			for (int i = 0; i < n; i += 4)
			{
				parse( db, TEST_MSG );
				parse( db, TEST_MSG );
				parse( db, TEST_MSG );
				parse( db, TEST_MSG );
			}
			long t1 = Timer.currentTimeMillis();
			
			double t = (t1-t0)/1000.0;
			double r = n / t;
			System.out.println( "took "+t+" for "+n+" iterations ("+r+" per second)" );
		}
	}
	
	private static void dump( Document d )
	{
		dump( "", d.getDocumentElement() );
		
	}

	private static void dump( String prefix, Element e )
	{
		System.out.println( prefix+"start of element name="+e.getTagName() );
		
		NamedNodeMap attrs = e.getAttributes();
		int n = attrs.getLength();
		for (int i = 0; i < n; i++)
		{
			Node a = attrs.item( i );
			System.out.println( prefix+"  attr name '"+a.getNodeName()+"' value '"+a.getNodeValue()+"'" );
		}
		
		NodeList nl = e.getChildNodes();
		n = nl.getLength();
		for (int i = 0; i < n; i++)
		{
			Node sn = nl.item( i );
			if (sn instanceof Element)
				dump( prefix+"  ", (Element) sn );
			else
				dump( prefix+"  ", (CharacterData ) sn );
		}
		
		System.out.println( prefix+"end of element name="+e.getTagName() );
	}

	private static void dump( String prefix, CharacterData data )
	{
		System.out.println( prefix+"data = '"+data+"'" );
	}

	private static Document parse( DocumentBuilder db, String msg ) throws Exception
	{
		return db.parse( new InputSource( new StringReader( msg ) ) );
	}

	private final static String TEST_MSG =
		"<message>" +
			"<messageId>connect</messageId>" +
			"<field name='machineName'>xxx</field>" +
			"<field name='queueName'>xxx</field>" +
			"<field name='heartbeatInterval'>10</field>" +
			"<field name='heartbeatPayload'>mediaResources</field>" +
			"<field name='serverId'>1</field>" +
			"<field name='transactionId'>1</field>" +
		"</message>";
}
