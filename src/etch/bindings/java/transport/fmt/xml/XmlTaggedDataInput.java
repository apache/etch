/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt.xml;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.util.Iterator;
import java.util.Stack;
import java.util.StringTokenizer;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.ArrayValue;
import etch.bindings.java.transport.TaggedDataInput;
import etch.util.FlexBuffer;
import etch.util.core.xml.XmlParser;
import etch.util.core.xml.XmlParser.CdataElement;
import etch.util.core.xml.XmlParser.Element;
import etch.util.core.xml.XmlParser.TagElement;

/**
 * XmlTaggedDataInput has methods to support reading xml data from an
 * input stream.
 */
final public class XmlTaggedDataInput extends XmlTaggedData
	implements TaggedDataInput, XmlTags
{
	/**
	 * Constructs the XmlTaggedDataInput.
	 * @param vf the value factory
	 * @param rdr the reader
	 */
	public XmlTaggedDataInput( ValueFactory vf, Reader rdr )
	{
		super( vf );
		this.rdr = rdr;
	}
	
	private Reader rdr;
	
	/**
	 * @param rdr
	 */
	public void setReader( Reader rdr )
	{
		this.rdr = rdr;
	}
	
	private final Stack<TagElement> stack = new Stack<TagElement>();
	
	private final Stack<Iterator<Element>> elementList = new Stack<Iterator<Element>>();

	public Message startMessage() throws IOException
	{
		stack.clear();
		
		TagElement te = new XmlParser().parseOne( rdr, null, MESSAGE_TAG );
		stack.push( te );
		elementList.push( te.getChildren() );
		
		Integer id = te.getIntAttr( null, STRUCT_TYPE_ATTR );
		Type type = vf.getType( id );
		return new Message( type, vf );
	}

	public void endMessage( Message msg )
	{
		elementList.pop();
		if (!elementList.isEmpty())
			throw new IllegalStateException( "!elementList.isEmpty()" );
		
		stack.pop();
		if (!stack.isEmpty())
			throw new IllegalStateException( "!stack.isEmpty()" );
			
	}

	public StructValue startStruct()
	{
		TagElement te = stack.peek();
		elementList.push( te.getChildren() );
		
		Integer id = te.getIntAttr( null, STRUCT_TYPE_ATTR );
		Type type = vf.getType( id );
		return new StructValue( type );
	}

	public boolean readStructElement( StructElement se ) throws IOException
	{
		TagElement te = readNextTagElement();
		if (te == null)
			return false;
		
		Type t = null; // TODO this needs to come from somewhere.
		Integer id = te.getIntAttr( null, KEY_ATTR );
		se.key = t.getField( id );
		
		stack.push( te );
		se.value = intReadValue();
		stack.pop();
		
		return true;
	}
	
	public static class StructElement
	{
		public Field key;
		public Object value;
	}

	public void endStruct( StructValue struct )
	{
		elementList.pop();
	}

	public ArrayValue startArray()
	{
		TagElement te = stack.peek();
		elementList.push( te.getChildren() );
		return new ArrayValue( null );
	}
	
	public static class ArrayElement
	{
		public Object value;
	}

	public boolean readArrayElement( ArrayElement ae ) throws IOException
	{
		TagElement te = readNextTagElement();
		if (te == null)
			return false;
		
		stack.push( te );
		ae.value = intReadValue();
		stack.pop();
		
		return true;
	}

	public void endArray( ArrayValue array )
	{
		elementList.pop();
	}

	public void close() throws IOException
	{
		rdr.close();
	}
	
	private TagElement readNextTagElement() throws IOException
	{
		if (!elementList.peek().hasNext())
			return null;
		
		Element e = elementList.peek().next();
		while (!(e instanceof TagElement))
		{
			CdataElement ce = (CdataElement) e;
			String s = ce.getCdata();
			if (s.trim().length() != 0)
				throw new IOException( "unexpected cdata in element: "+s );
			
			if (!elementList.peek().hasNext())
				return null;
			
			e = elementList.peek().next();
		}
		
		return (TagElement) e;
	}
	
	private Object intReadValue() throws IOException
	{
		TagElement te = stack.peek();
		System.out.printf( "readValue: %s\n", te );
		String type = te.getAttr( null, ELEMENT_TYPE_ATTR );
		
		if (type.equals( NULL_TYPE ))
			return null;
		
		if (type.equals( BOOLEAN_TYPE ))
			return new Boolean( te.getCdataValue() );
		
		if (type.equals( INTEGER_TYPE ))
			return new Integer( te.getCdataValue() );
		
		if (type.equals( LONG_TYPE ))
			return new Long( te.getCdataValue() );
		
		if (type.equals( FLOAT_TYPE ))
			return new Float( te.getCdataValue() );
		
		if (type.equals( DOUBLE_TYPE ))
			return new Double( te.getCdataValue() );

		if (type.equals( BYTES_TYPE ))
			return string2bytes( te.getCdataValue() );
		
		if (type.equals( STRING_TYPE ))
			return te.getCdataValue();
		
		if (type.equals( ARRAY_TYPE ))
			return readArray(null);
		
		if (type.equals( STRUCT_TYPE ))
			return readStruct();
		
		if (type.equals( CUSTOM_TYPE ))
			return vf.importCustomValue( readStruct() );
		
		throw new UnsupportedOperationException( "unsupported type code "+type );
	}
	
	private byte[] string2bytes( String s )
	{
		// TODO byte encoding could be base64 or something else more efficient.

		ByteArrayOutputStream baos = new ByteArrayOutputStream();
		
		StringTokenizer st = new StringTokenizer( s, " \t\r\n" );
		while (st.hasMoreTokens())
			baos.write( (byte) Integer.parseInt( st.nextToken(), 16 ) );
		
		return baos.toByteArray();
	}

	/**
	 * @param vf the value factory.
	 * @param s the message text.
	 * @return the message read from the string.
	 * @throws IOException
	 */
	public static Message readMessage( ValueFactory vf, String s )
		throws IOException
	{
		return readMessage( vf, new StringReader( s ) );
	}
	
	/**
	 * @param vf the value factory.
	 * @param rdr the source of message text.
	 * @return the message read from the reader.
	 * @throws IOException
	 */
	public static Message readMessage( ValueFactory vf, Reader rdr ) throws IOException
	{
		return new XmlTaggedDataInput( vf, rdr ).readMessage( null );
	}
	
	public Message readMessage( FlexBuffer buf ) throws IOException
	{
		// TODO implement readMessage function
		return null;
	}

	public StructValue readStruct() throws IOException
	{
		// TODO implement readStruct function
		return null;
	}

	public ArrayValue readArray(Validator v) throws IOException
	{
		// TODO implement readArray function
		return null;
	}
}
