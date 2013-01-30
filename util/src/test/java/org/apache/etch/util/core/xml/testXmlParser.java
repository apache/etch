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

package org.apache.etch.util.core.xml;

import java.io.IOException;
import java.io.Writer;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import org.apache.etch.util.Timer;



@SuppressWarnings("all")
public class testXmlParser extends XmlParser
{
	/**
	 * @param args
	 * @throws ParseException
	 * @throws InterruptedException
	 */
	public static void main( String[] args ) throws ParseException, InterruptedException
	{
		testXmlParser xp = new testXmlParser();

		parse( xp, "<x>" );
		
		parse( xp, "<a>" );
		parse( xp, "</a>" );

		parse( xp, "<a:d>" );
		parse( xp, "</a:d>" );

		parse( xp, "<abc>" );
		parse( xp, "<abc:def>" );
		
		parse( xp, "<abc >" );
		parse( xp, "<abc:def >" );
		
		parse( xp, "<abc/>" );
		parse( xp, "<abc:def/>" );
		
		parse( xp, "<abc />" );
		parse( xp, "<abc:def />" );
		
		parse( xp, "</abc:def>" );
		parse( xp, "</abc>" );
		
		parse( xp, "</abc:def >" );
		parse( xp, "</abc >" );

		parse( xp, "<a x='1'/>" );
		parse( xp, "<a x ='1'/>" );
		parse( xp, "<a x= '1'/>" );
		parse( xp, "<a x = '1'/>" );
		
		parse( xp, "<a xyz='&#169;123&#169;'/>" );
		parse( xp, "<a x='1' y='2'/>" );

		parse( xp, "<a q:x='1'/>" );
		parse( xp, "<a q:xyz='123'/>" );
		parse( xp, "<a q:x='1' q:y='2'/>" );

		parse( xp, "<a x=\"1\"/>" );
		parse( xp, "<a xyz=\"123\"/>" );
		parse( xp, "<a x=\"1\" y=\"2\"/>" );
		
		parse( xp, "</x>" );
		TagElement te = xp.finish();
		System.out.println( "parsed: "+te );
		
		xp.clear();
		
		xp.parseOne( TEST_MSG );
		
		xp.dump();
		
		if (false)
		{
			int m = 5;
			int n = 50000*4;
			for (int j = 0; j < m; j++)
			{
				if (j != 0)
					Thread.sleep( 5000 );
				
				long t0 = Timer.currentTimeMillis();
				for (int i = 0; i < n; i += 4)
				{
					xp.parse( TEST_MSG );
					xp.parse( TEST_MSG );
					xp.parse( TEST_MSG );
					xp.parse( TEST_MSG );
				}
				long t1 = Timer.currentTimeMillis();
				
				double t = (t1-t0)/1000.0;
				double r = n / t;
				System.out.println( "took "+t+" for "+n+" iterations ("+r+" per second)" );
			}
		}
	}

	private final static String TEST_MSG =
		"<?xml version=\"1.0\" encoding=\"utf-16\"?>" +
		"<message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">" +
			"<field name=\"heartbeatInterval\">10</field>" +
			"<field name=\"heartbeatPayload\">mediaResources</field>" +
			"<field name=\"machineName\">JAVAMAN</field>" +
			"<field name=\"queueName\">NULL</field>" +
			"<field name=\"serverId\">1</field>" +
			"<field name=\"transactionId\">3</field>" +
			"<field name=\"serverId\">1</field>" +
			"<field name=\"clientId\">NULL</field>" +
			"<messageId>connect</messageId>" +
		"</message>";

	private static void parse( XmlParser xp, String s ) throws ParseException
	{
		System.out.println( "trying '"+s+"'..." );
		System.out.flush();
		
		xp.parse( s );
		
		System.out.println( "... ok!" );
		System.out.flush();
		
		xp.dump();
	}
	
	public TagElement addRootTag( QName qName, Map<QName, String> attrs )
	{
		MessageProcessor mp = new MessageProcessor();
		pushTagElement( mp );
		return mp.addTag( qName, attrs );
	}
	
	/**
	 * Description of MessageProcessor
	 */
	public class MessageProcessor extends AbstractElement implements TagElement
	{
		/**
		 */
		public MessageProcessor()
		{
			// nothing to do
		}

		public boolean matches( String qualifier, String name )
		{
			return false;
		}

		public void addCdata( String cdata )
		{
			if (cdata.trim().length() > 0)
				throw new RuntimeException( "cdata not allowed in message processor: "+cdata );
		}
		
		public TagElement addTag( QName childQName, Map<QName, String> childAttrs )
		{
			if (childQName.matches( MESSAGE_QUALIFIER, MESSAGE_NAME ))
				return new Message( childAttrs );
			
			return new DefaultTagElement( childQName, childAttrs );
		}

		public void finish()
		{
			// nothing to do
		}

		public Iterator<Element> getChildren()
		{
			// Auto-generated method stub
			return null;
		}

		public String getCdataValue()
		{
			// Auto-generated method stub
			return null;
		}

		public Iterator<QName> getAttrNames()
		{
			// Auto-generated method stub
			return null;
		}

		public String getAttr( String qualifier, String name )
		{
			// Auto-generated method stub
			return null;
		}

		public QName getQName()
		{
			// Auto-generated method stub
			return null;
		}

		public boolean matches( QName qName )
		{
			// Auto-generated method stub
			return false;
		}

		public boolean remove( Element e )
		{
			// Auto-generated method stub
			return false;
		}

		public void clear()
		{
			// Auto-generated method stub
			
		}

		public Integer getIntAttr( String qualifier, String name )
		{
			// Auto-generated method stub
			return null;
		}

		public Long getLongAttr( String qualifier, String name )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement find( String path )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement find( String oqualifier, String oname )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement find( QName subQName )
		{
			// Auto-generated method stub
			return null;
		}

		public Iterator<Element> iterator()
		{
			// Auto-generated method stub
			return null;
		}

		public boolean hasChildren()
		{
			// Auto-generated method stub
			return false;
		}

		public boolean hasAttrs()
		{
			// Auto-generated method stub
			return false;
		}

		public TagElement addTag( QName childQName )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement addTag( String childName, Map<QName, String> childAttrs )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement addTag( String childName )
		{
			// Auto-generated method stub
			return null;
		}

		public int countChildren()
		{
			// Auto-generated method stub
			return 0;
		}

		public Element getChild( int index )
		{
			// Auto-generated method stub
			return null;
		}

		public boolean hasCdataValue()
		{
			// Auto-generated method stub
			return false;
		}

		public void toString( Writer wtr, boolean newlineBetweenTags,
			boolean collapseEmptyTags ) throws IOException
		{
			wtr.write( "mp" );
		}

		public TagElement setAttr( String qualifier, String name, String value )
		{
			return this;
		}

		public TagElement setAttr( String qualifier, String name, int value )
		{
			return this;
		}

		public TagElement setAttr( String qualifier, String name, long value )
		{
			return this;
		}
	}
	
	/**
	 * Description of Message.
	 */
	public class Message extends AbstractElement implements TagElement
	{
		/**
		 * Constructs the Message.
		 *
		 * @param attrs
		 */
		public Message( Map<QName, String> attrs )
		{
			type = attrs.get( new QName( null, "t" ) );
		}
		
		private final String type;

		public boolean matches( String qualifier, String name )
		{
			return QName.matches( qualifier, MESSAGE_QUALIFIER, name, MESSAGE_NAME );
		}

		public void addCdata( String cdata )
		{
			if (cdata.trim().length() != 0)
				throw new RuntimeException( "cdata not allowed in message body" );
		}

		public TagElement addTag( QName childQName, Map<QName, String> childAttrs )
		{
			// only allow parameter tags
			if (!childQName.matches( PARAMETER_QUALIFIER, PARAMETER_NAME ))
				throw new RuntimeException( "tag "+childQName+" not allowed in message body" );
			
			Parameter p = new Parameter( childAttrs );
			params.add( p );
			return p;
		}

		public void finish()
		{
			// nothing to do.
			//System.out.println( "message finished: "+this );
		}
		
		private List<Parameter> params = new Vector<Parameter>();

		public Iterator<Element> getChildren()
		{
			// Auto-generated method stub
			return null;
		}

		public String getCdataValue()
		{
			// Auto-generated method stub
			return null;
		}

		public Iterator<QName> getAttrNames()
		{
			// Auto-generated method stub
			return null;
		}

		public String getAttr( String qualifier, String name )
		{
			// Auto-generated method stub
			return null;
		}

		public QName getQName()
		{
			// Auto-generated method stub
			return null;
		}

		public boolean matches( QName qName )
		{
			// Auto-generated method stub
			return false;
		}

		public boolean remove( Element e )
		{
			// Auto-generated method stub
			return false;
		}

		public void clear()
		{
			// Auto-generated method stub
			
		}

		public Integer getIntAttr( String qualifier, String name )
		{
			// Auto-generated method stub
			return null;
		}

		public Long getLongAttr( String qualifier, String name )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement find( String path )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement find( String oqualifier, String oname )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement find( QName subQName )
		{
			// Auto-generated method stub
			return null;
		}

		public Iterator<Element> iterator()
		{
			// Auto-generated method stub
			return null;
		}

		public boolean hasChildren()
		{
			// Auto-generated method stub
			return false;
		}

		public boolean hasAttrs()
		{
			// Auto-generated method stub
			return false;
		}

		public TagElement addTag( QName childQName )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement addTag( String childName, Map<QName, String> childAttrs )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement addTag( String childName )
		{
			// Auto-generated method stub
			return null;
		}

		public int countChildren()
		{
			// Auto-generated method stub
			return 0;
		}

		public Element getChild( int index )
		{
			// Auto-generated method stub
			return null;
		}

		public boolean hasCdataValue()
		{
			// Auto-generated method stub
			return false;
		}

		public void toString( Writer wtr, boolean newlineBetweenTags,
			boolean collapseEmptyTags ) throws IOException
		{
			wtr.write( type );
			wtr.write( ": " );
			wtr.write( params.toString() );
		}

		public TagElement setAttr( String qualifier, String name, String value )
		{
			return this;
		}

		public TagElement setAttr( String qualifier, String name, int value )
		{
			return this;
		}

		public TagElement setAttr( String qualifier, String name, long value )
		{
			return this;
		}
	}
	
	/**
	 * Description of Parameter.
	 */
	public static class Parameter extends AbstractElement implements TagElement
	{
		/**
		 * Constructs the Parameter.
		 *
		 * @param attrs
		 */
		public Parameter( Map<QName, String> attrs )
		{
			n = attrs.get( new QName( null, "n" ) );
			if (n == null)
				throw new RuntimeException( "n attribute not specified" );
			
			t = attrs.get( new QName( null, "t" ) );
			if (t == null)
				throw new RuntimeException( "t attribute not specified" );
			
			setValue( attrs.get( new QName( null, "v" ) ) );
		}
		
		private final String n;
		
		private final String t;

		public boolean matches( String qualifier, String name )
		{
			return QName.matches( qualifier, PARAMETER_QUALIFIER, name, PARAMETER_NAME );
		}

		public void addCdata( String cdata )
		{
			setValue( cdata );
		}

		public TagElement addTag( QName childQName, Map<QName, String> childAttrs )
		{
			throw new RuntimeException( "subordinate elements are not yet supported" );
		}

		public void finish()
		{
			if (v == null)
				throw new RuntimeException( "parameter "+n+" was not given a value" );
		}
		
		/**
		 * @param name
		 * @return true if name matches this parameter's name
		 */
		public boolean matchesName( String name )
		{
			return n.equals( name );
		}
		
		/**
		 * @return the name of this parameter
		 */
		public String getName()
		{
			return n;
		}
		
		/**
		 * @return the value of this parameter.
		 */
		public Object getValue()
		{
			return v;
		}
		
		private void setValue( String s )
		{
			if (s == null)
				return;
			
			//System.out.println( "setValue: name '"+n+"' type '"+t+"' value '"+s+"'" );
			
			if (v != null)
				throw new RuntimeException( "parameter "+n+" already has a value" );
			
			if (t.equals( "s" ))
				v = s;
			else if (t.equals( "i" ))
				v = Integer.parseInt( s );
			else if (t.equals( "d" ))
				v = Double.parseDouble( s );
			else
				throw new RuntimeException( "parameter "+n+" type "+t+" not supported" );
		}
		
		private Object v;

		public Iterator<Element> getChildren()
		{
			// Auto-generated method stub
			return null;
		}

		public String getCdataValue()
		{
			// Auto-generated method stub
			return null;
		}

		public Iterator<QName> getAttrNames()
		{
			// Auto-generated method stub
			return null;
		}

		public String getAttr( String qualifier, String name )
		{
			// Auto-generated method stub
			return null;
		}

		public QName getQName()
		{
			// Auto-generated method stub
			return null;
		}

		public boolean matches( QName qName )
		{
			// Auto-generated method stub
			return false;
		}

		public boolean remove( Element e )
		{
			// Auto-generated method stub
			return false;
		}

		public void clear()
		{
			// Auto-generated method stub
			
		}

		public Integer getIntAttr( String qualifier, String name )
		{
			// Auto-generated method stub
			return null;
		}

		public Long getLongAttr( String qualifier, String name )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement find( String path )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement find( String oqualifier, String oname )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement find( QName subQName )
		{
			// Auto-generated method stub
			return null;
		}

		public Iterator<Element> iterator()
		{
			// Auto-generated method stub
			return null;
		}

		public boolean hasChildren()
		{
			// Auto-generated method stub
			return false;
		}

		public boolean hasAttrs()
		{
			// Auto-generated method stub
			return false;
		}

		public TagElement addTag( QName childQName )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement addTag( String childName, Map<QName, String> childAttrs )
		{
			// Auto-generated method stub
			return null;
		}

		public TagElement addTag( String childName )
		{
			// Auto-generated method stub
			return null;
		}

		public int countChildren()
		{
			// Auto-generated method stub
			return 0;
		}

		public Element getChild( int index )
		{
			// Auto-generated method stub
			return null;
		}

		public boolean hasCdataValue()
		{
			// Auto-generated method stub
			return false;
		}

		public void toString( Writer wtr, boolean newlineBetweenTags,
			boolean collapseEmptyTags ) throws IOException
		{
			wtr.write( n );
			wtr.write( " =(" );
			wtr.write( t );
			wtr.write( ") " );
			wtr.write( v.toString() );
		}

		public TagElement setAttr( String qualifier, String name, String value )
		{
			return this;
		}

		public TagElement setAttr( String qualifier, String name, int value )
		{
			return this;
		}

		public TagElement setAttr( String qualifier, String name, long value )
		{
			return this;
		}
	}
	
	/**
	 * Description of MESSAGE_QUALIFIER.
	 */
	public final static String MESSAGE_QUALIFIER = null;
	
	/**
	 * Description of MESSAGE_NAME.
	 */
	public final static String MESSAGE_NAME = "m";
	
	/**
	 * Description of PARAMETER_QUALIFIER.
	 */
	public final static String PARAMETER_QUALIFIER = null;
	
	/**
	 * Description of PARAMETER_NAME.
	 */
	public final static String PARAMETER_NAME = "p";
}
