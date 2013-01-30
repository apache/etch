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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Stack;
import java.util.StringTokenizer;

import org.apache.etch.util.Assertion;
import org.apache.etch.util.EmptyIterator;
import org.apache.etch.util.SingleIterator;
import org.apache.etch.util.StringUtil;



/**
 * Description of XmlParser.
 */
public class XmlParser
{
	/**
	 * Constructs the xml parser.
	 */
	public XmlParser()
	{
		// nothing to do yet.
	}
	
	/**
	 * Line separator for XML files.
	 */
	public final static String CRLF = "\r\n";
	
	/**
	 * @param file
	 * @param qualifier
	 * @param name
	 * @return the top tag element
	 * @throws IOException if there is a problem reading the file
	 * or if the top tag element isn't as specified
	 */
	public TagElement parseOne( File file, String qualifier, String name )
		throws IOException
	{
		Reader rdr = new BufferedReader( new FileReader( file ) );
		try
		{
			return parseOne( rdr, qualifier, name );
		}
		finally
		{
			rdr.close();
		}
	}
	
	/**
	 * @param is
	 * @param qualifier
	 * @param name
	 * @return the tag element found by parsing the specified input stream
	 * to the very end.
	 * @throws ParseException
	 * @throws IOException
	 */
	public TagElement parseOne( InputStream is, String qualifier, String name )
		throws ParseException, IOException
	{
		return parseOne( new InputStreamReader( is ), qualifier, name );
	}

	/**
	 * @param rdr
	 * @param qualifier
	 * @param name
	 * @return the tag element found by parsing the specified reader
	 * to the very end.
	 * @throws IOException
	 * @throws ParseException
	 */
	public TagElement parseOne( Reader rdr, String qualifier, String name )
		throws ParseException, IOException
	{
		while (parse( rdr.read() ))
		{
			// nothing to do.
		}
		
		TagElement te = finish();
		
		if (name != null && te != null && !te.matches( qualifier, name ))
			throw new IOException(
				"top tag "+te.getQName()+" does not match "+
					new QName( qualifier, name ) );
		
		return te;
	}
	
	/**
	 * @param s
	 * @return false if parsing is finished, true otherwise.
	 * @throws ParseException
	 */
	public TagElement parseOne( String s ) throws ParseException
	{
		clear();
		
		if (parse( s ))
			return finish();
		
		return null;
	}
	/**
	 * @param s
	 * @return false if parsing is finished, true otherwise.
	 * @throws ParseException
	 */
	public boolean parse( String s ) throws ParseException
	{
		int n = s.length();
		for (int i = 0; i < n; i++)
			if (!parse( s.charAt( i ) ))
				return false;
		return true;
	}
	
	/**
	 * @return TagElement if the parse worked.
	 * @throws ParseException
	 */
	public TagElement finish() throws ParseException
	{
		if (parse( -1 ))
			return null;
		
		return rootTagElement;
	}
	
	/**
	 * @param c the next character to parse, or -1 to indicate
	 * there are no more characters.
	 * @return false if parsing is finished, true otherwise.
	 * @throws ParseException
	 */
	public boolean parse( int c ) throws ParseException
	{
		try
		{
			if (c >= 0)
				context.append( (char) c );
			return parse0( c );
		}
		catch ( ParseException e )
		{
			e.setContext( context.toString() );
			e.setTagElementStack( getTagElements() );
			throw e;
		}
		catch ( RuntimeException e )
		{
			ParseException pe = new ParseException( "caught exception", e );
			pe.setContext( context.toString() );
			pe.setTagElementStack( getTagElements() );
			throw e;
		}
	}
	
	private StringBuf context = new CircularStringBuf( null, 100 );
	
	private boolean parse0( int c ) throws ParseException
	{
		if (state < 0 || state >= NSTATES)
			throw new IllegalStateException( "the parser is in an illegal state: "+state );
		
		int cc = getCharClass( c );
		int stateAction = transitions[state][cc];
		int newState = stateAction & STATE_MASK;
		int action = stateAction >>> ACTION_SHIFT & ACTION_MASK;
		switch (action)
		{
			case A_ERROR:
			{
				int s = state;
				state = -1;
				
				if (c == -1)
					throw new ParseException( "unexpected EOF while "+
						describeState( s )+
						": expected one of "+expectedChars( s ) );
				
				throw new ParseException( "unexpected character "+
					describeChar( c )+" while "+describeState( s )+
					": expected one of "+expectedChars( s ) );
			}
			
			case A_IGNORE:
				break;
			
			case A_STOP:
				finishCdata();
				return false;
			
			case A_ADD_CHAR_TO_CDATA:
				addToCdata( c );
				break;
			
			case A_ADD_LT_CHAR_TO_CDATA:
				addToCdata( '<' );
				addToCdata( c );
				break;
			
			case A_ADD_LT_SLASH_CHAR_TO_CDATA:
				addToCdata( '<' );
				addToCdata( '/' );
				addToCdata( c );
				break;
			
			case A_ADD_LT_BANG_CHAR_TO_CDATA:
				addToCdata( '<' );
				addToCdata( '!' );
				addToCdata( c );
				break;
			
			case A_ADD_LT_QUESTION_CHAR_TO_CDATA:
				addToCdata( '<' );
				addToCdata( '?' );
				addToCdata( c );
				break;
			
			case A_ADD_CHAR_TO_TAG_NAME:
				finishCdata();
				addToTagName( (char) c );
				break;
			
			case A_SET_TAG_NAME_QUALIFIER:
				setTagNameQualifier();
				break;
			
			case A_START_TAG:
				startTag();
				break;
			
			case A_EMPTY_TAG:
				emptyTag();
				break;
			
			case A_END_TAG:
				endTag();
				break;
			
			case A_ADD_CHAR_TO_ATTR_NAME:
				addToAttrName( (char) c );
				break;
			
			case A_SET_ATTR_NAME_QUALIFIER:
				setAttrNameQualifier();
				break;
			
			case A_ADD_TO_ATTR_VALUE:
				addToAttrValue( (char) c );
				break;
			
			case A_FINISH_ATTR:
				finishAttr();
				break;

			default:
				throw new UnsupportedOperationException( "unknown action "+action );
		}
		state = newState;
		return true;
	}

	private int state = 0;

	////////////////////
	// ACTION METHODS //
	////////////////////

	private static final int MAX_CDATA_LEN = 1024*1024;

	private static final int MAX_TAG_NAME_LEN = 255;

	private static final int MAX_ATTR_NAME_LEN = 255;

	private static final int MAX_ATTR_VALUE_LEN = 10*1024;

	private static final int MAX_ESCAPE_LEN = 31;
	
	private void addToCdata( int c )
	{
		if (c >= 0)
			cdataBuf.append( (char) c );
	}

	private final PlainStringBuf cdataBuf =
		new StringBufWithEscape( "cdata", MAX_CDATA_LEN,
			MAX_ESCAPE_LEN, escapes );
	
	private void finishCdata() throws ParseException
	{
		String cdata = finishStringBuf( cdataBuf, true );
		if (cdata.length() > 0)
			deliverCdata( cdata );
	}

	private void addToTagName( char c )
	{
		tagNameBuf.append( c );
	}
	
	private final StringBuf tagNameBuf =
		new PlainStringBuf( "tag name", MAX_TAG_NAME_LEN );

	private void setTagNameQualifier() throws ParseException
	{
		tagNameQualifier = finishStringBuf( tagNameBuf, false );
	}
	
	private String tagNameQualifier;

	private void emptyTag() throws ParseException
	{
		String tagName = finishStringBuf( tagNameBuf, false );
		deliverStartTag( tagNameQualifier, tagName, true );
		tagNameQualifier = null;
	}

	private void startTag() throws ParseException
	{
		String tagName = finishStringBuf( tagNameBuf, false );
		deliverStartTag( tagNameQualifier, tagName, false );
		tagNameQualifier = null;
	}

	private void endTag() throws ParseException
	{
		String tagName = finishStringBuf( tagNameBuf, false );
		deliverEndTag( tagNameQualifier, tagName );
		tagNameQualifier = null;
	}

	private void addToAttrName( char c )
	{
		attrNameBuf.append( c );
	}
	
	private final PlainStringBuf attrNameBuf =
		new PlainStringBuf( "attr name", MAX_ATTR_NAME_LEN );

	private void setAttrNameQualifier() throws ParseException
	{
		attrNameQualifier = finishStringBuf( attrNameBuf, false );
	}
	
	private String attrNameQualifier;

	private void addToAttrValue( char c )
	{
		attrValueBuf.append( c );
	}
	
	private final PlainStringBuf attrValueBuf =
		new StringBufWithEscape( "attr value", MAX_ATTR_VALUE_LEN,
			MAX_ESCAPE_LEN, escapes );

	private void finishAttr() throws ParseException
	{
		String attrName = finishStringBuf( attrNameBuf, false );
		String attrValue = finishStringBuf( attrValueBuf, true );
		deliverAttr( attrNameQualifier, attrName, attrValue );
		attrNameQualifier = null;
	}
	
	/**
	 * Clears the state of the xml parser.
	 */
	public void clear()
	{
		attrNameBuf.clear();
		attrNameQualifier = null;
		attrValueBuf.clear();
		cdataBuf.clear();
		context.clear();
		state = 0;
		tagAttrs = null;
		clearTagElements();
		tagNameBuf.clear();
		tagNameQualifier = null;
	}

	//////////////////////
	// DELIVERY METHODS //
	//////////////////////

	/**
	 * Delivers the characters between tags if any.
	 * 
	 * @param cdata (escape sequences have been expanded).
	 * @throws ParseException
	 */
	protected void deliverCdata( String cdata )
		throws ParseException
	{
		TagElement top = peekTagElement();
		
		if (top != null)
			top.addCdata( cdata );
		else
			addRootCdata( cdata );
	}

	/**
	 * Delivers an open tag.
	 * 
	 * @param qualifier any qualifier prepended to the name.
	 * 
	 * @param name the tag name (minus any qualifier).
	 * 
	 * @param empty true if the tag was opened and closed all in one step.
	 */
	protected void deliverStartTag( String qualifier, String name, boolean empty )
	{
		TagElement top = peekTagElement();

		QName qName = new QName( qualifier, name );
		
		TagElement tagElement;
		if (top != null)
			tagElement = top.addTag( qName, tagAttrs );
		else
			rootTagElement = tagElement = addRootTag( qName, tagAttrs );
		tagAttrs = null;
		
		if (empty)
			tagElement.finish();
		else
			pushTagElement( tagElement );
	}

	/**
	 * Delivers a close tag. A close tag will not be delivered for a
	 * tag which was opened and closed all in one step.
	 * 
	 * @param qualifier any qualifier prepended to the name.
	 * 
	 * @param name the tag name (minus any qualifier).
	 * @throws ParseException
	 */
	protected void deliverEndTag( String qualifier, String name )
		throws ParseException
	{
		TagElement top = peekTagElement();
		if (top == null)
			throw new ParseException( "end tag with empty stack: "+
				QName.toString( qualifier, name ) );
		
		if (!top.matches( qualifier, name ))
			throw new ParseException( "end tag "+
				QName.toString( qualifier, name )+
				" does not match current top of stack" );
		
		popTagElement().finish();
	}

	/**
	 * Delivers an attribute of a tag which is being parsed. The tag
	 * will be delivered with deliverOpenTag when it is done, after
	 * any and all attributes have been delivered.
	 * 
	 * @param qualifier any qualifier prepended to the name.
	 * 
	 * @param name the attribute name (minus any qualifier).
	 * 
	 * @param value the attribute value (escape sequences have been expanded).
	 */
	protected void deliverAttr( String qualifier, String name, String value )
	{
		if (tagAttrs == null)
			tagAttrs = new HashMap<QName, String>();
		
		QName qName = new QName( qualifier, name );
		tagAttrs.put( qName, value );
	}

	///////////////////////
	// TAG ELEMENT STACK //
	///////////////////////
	
	private TagElement peekTagElement()
	{
		if (tagElementStack.isEmpty())
			return null;
		
		return tagElementStack.peek();
	}
	
	/**
	 * @param tagElement
	 */
	protected void pushTagElement( TagElement tagElement )
	{
		tagElementStack.push( tagElement );
	}
	
	private TagElement popTagElement()
	{
		return tagElementStack.pop();
	}
	
	private void clearTagElements()
	{
		tagElementStack.clear();
		rootTagElement = null;
	}
	
	/**
	 * Dumps the tag stack of the xml parser.
	 */
	public void dump()
	{
		System.out.println( "tag stack: "+tagElementStack );
		System.out.flush();
	}
	
	/**
	 * @return a list of the current tag element stack.
	 */
	public List<TagElement> getTagElements()
	{
		return new ArrayList<TagElement>( tagElementStack );
	}
	
	private Stack<TagElement> tagElementStack = new Stack<TagElement>();
	
	private Map<QName, String> tagAttrs;
	
	private TagElement rootTagElement;

	/////////////////////////////
	// SUBCLASS RESPONSIBILITY //
	/////////////////////////////
	
	/**
	 * @param cdata
	 * @throws ParseException
	 */
	public void addRootCdata( String cdata ) throws ParseException
	{
		if (cdata.trim().length() > 0)
			throw new ParseException( "cannot add root cdata" );
	}
	
	/**
	 * @param qName
	 * @param attrs
	 * @return a new tag element suitable to be a root tag element.
	 */
	public TagElement addRootTag( QName qName, Map<QName, String> attrs )
	{
		return new RootTagElement( qName, attrs );
	}

	//////////////////////////
	// misc support methods //
	//////////////////////////
	
	private final static Map<String,Character> escapes = new HashMap<String,Character>();
	static
	{
		escapes.put( "amp", new Character( '&' ) );
		escapes.put( "apos", new Character( '\'' ) );
		escapes.put( "gt", new Character( '>' ) );
		escapes.put( "lt", new Character( '<' ) );
		escapes.put( "quot", new Character( '"' ) );
	}

	private static String finishStringBuf( StringBuf sb,
		boolean emptyOk ) throws ParseException
	{
		if (sb.length() == 0)
		{
			if (!emptyOk)
				throw new ParseException( "empty '"+sb.getDescr()+"' not allowed" );
			
			return "";
		}
		
		String s = sb.toString();
		sb.clear();
		return s;
	}
	
	private static String describeChar( int c )
	{
		if (c < 32 || c > 126)
			return "&#"+c+';';
		
		if (c == '\'')
			return "'\\''";
		
		if (c == '\\')
			return "'\\\\'";
		
		return "'"+(char) c+'\'';
	}

	private static String expectedChars( int state )
	{
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < NCC; i++)
		{
			if (transitions[state][i]>>>ACTION_SHIFT != A_ERROR)
			{
				if (sb.length() > 0)
					sb.append( '|' );
				sb.append( describeCharClass( i ) );
			}
		}
		return sb.toString();
	}
	
	private final static int STATE_MASK = 255;
	
	private final static int ACTION_SHIFT = 8;
	
	private final static int ACTION_MASK = 255;

	///////////////////////
	// CHARACTER CLASSES //
	///////////////////////

	private final static int CC_OTHER = 0;
	private final static int CC_EOF = 1;
	private final static int CC_X = 2;
	private final static int CC_M = 3;
	private final static int CC_L = 4;
	private final static int CC_OTHR_LTR = 5;
	private final static int CC_DIGIT = 6;
	private final static int CC_WS = 7;
	private final static int CC_SLASH = 8;
	private final static int CC_BANG = 9;
	private final static int CC_EQ = 10;
	private final static int CC_SQ = 11;
	private final static int CC_DQ = 12;
	private final static int CC_DASH = 13;
	private final static int CC_COLON = 14;
	private final static int CC_LT = 15;
	private final static int CC_GT = 16;
	private final static int CC_DOT = 17;
	private final static int CC_UNDERSCORE = 18;
	private final static int CC_QUESTION = 19;
	private final static int NCC = 20;
	
	private final static String[] ccDescr = new String[NCC];
	
	private final static int NCHARS = 128;
	private final static byte[] ccs = new byte[NCHARS];
	static
	{
		addChar( CC_EOF, "EOF" );
		addChar( CC_OTHER, "all others" );

		addChar( CC_OTHR_LTR, 'a', 'k', "a-k" );
		addChar( CC_L, 'l', "l" );
		addChar( CC_M, 'm', "m" );
		addChar( CC_OTHR_LTR, 'n', 'w', "n-w" );
		addChar( CC_X, 'x', "x" );
		addChar( CC_OTHR_LTR, 'y', 'z', "y-z" );

		addChar( CC_OTHR_LTR, 'A', 'K', "A-K" );
		addChar( CC_L, 'L', "L" );
		addChar( CC_M, 'M', "M" );
		addChar( CC_OTHR_LTR, 'N', 'W', "N-W" );
		addChar( CC_X, 'X', "X" );
		addChar( CC_OTHR_LTR, 'Y', 'Z', "Y-Z" );
		
		addChar( CC_DIGIT, '0', '9', "0-9" );
		
		addChar( CC_WS, ' ', "SP" );
		addChar( CC_WS, '\t', "TAB" );
		addChar( CC_WS, '\r', "CR" );
		addChar( CC_WS, '\n', "LF" );
		
		addChar( CC_SLASH, '/', "/" );
		addChar( CC_BANG, '!', "!" );
		addChar( CC_EQ, '=', "=" );
		addChar( CC_SQ, '\'', "'" );
		addChar( CC_DQ, '"', "\"" );
		addChar( CC_DASH, '-', "-" );
		addChar( CC_COLON, ':', ":" );
		addChar( CC_LT, '<', "<" );
		addChar( CC_GT, '>', ">" );
		addChar( CC_DOT, '.', "." );
		addChar( CC_UNDERSCORE, '_', "_" );
		addChar( CC_QUESTION, '?', "?" );
	}
	
	private static int getCharClass( int c )
	{
		if (c < 0) return CC_EOF;
		if (c >= NCHARS) return CC_OTHER;
		return ccs[c];
	}

	private static void addChar( int cc, String descr )
	{
		addCCDescr( cc, descr );
	}

	private static void addChar( int cc, char c, String descr )
	{
		Assertion.check( ccs[c] == CC_OTHER, "ccs[c] == CC_OTHER" );
		ccs[c] = (byte) cc;
		addCCDescr( cc, descr );
	}

	private static void addChar( int cc, char start, char end, String descr )
	{
		while (start <= end)
		{
			Assertion.check( ccs[start] == CC_OTHER, "ccs[start] == CC_OTHER" );
			ccs[start++] = (byte) cc;
		}
		addCCDescr( cc, descr );
	}

	private static void addCCDescr( int cc, String descr )
	{
		String s = ccDescr[cc];
		if (s != null)
			ccDescr[cc] = s+'|'+descr;
		else
			ccDescr[cc] = descr;
	}

	private static String describeCharClass( int cc )
	{
		return ccDescr[cc];
	}

	/////////////
	// ACTIONS //
	/////////////
	
	private final static int A_ERROR = 0;
	private final static int A_IGNORE = 1;
	private final static int A_STOP = 2;
	private final static int A_ADD_CHAR_TO_CDATA = 3;
	private final static int A_ADD_LT_CHAR_TO_CDATA = 4;
	private final static int A_ADD_LT_SLASH_CHAR_TO_CDATA = 5;
	private final static int A_ADD_LT_BANG_CHAR_TO_CDATA = 15;
	private final static int A_ADD_LT_QUESTION_CHAR_TO_CDATA = 16;
	private final static int A_ADD_CHAR_TO_TAG_NAME = 6;
	private final static int A_SET_TAG_NAME_QUALIFIER = 7;
	private final static int A_START_TAG = 8;
	private final static int A_EMPTY_TAG = 9;
	private final static int A_END_TAG = 10;
	private final static int A_ADD_CHAR_TO_ATTR_NAME = 11;
	private final static int A_SET_ATTR_NAME_QUALIFIER = 12;
	private final static int A_ADD_TO_ATTR_VALUE = 13;
	private final static int A_FINISH_ATTR = 14;

	////////////
	// STATES //
	////////////

	private final static int S_CDATA = 0;
	private final static int S_GOT_LT = 1;
	private final static int S_GET_STAG_NM = 2;
	private final static int S_SKNG_GT = 3;
	private final static int S_SKNG_ATTR_NM = 4;
	private final static int S_GET_ATTR_NM = 5;
	private final static int S_SKNG_ATTR_VAL = 6;
	private final static int S_GET_DQ_VALUE = 7;
	private final static int S_GET_SQ_VALUE = 8;
	private final static int S_GOT_LT_SLASH = 9;
	private final static int S_GET_ETAG_NM = 10;
	private final static int S_SKNG_WS_STAR_GT = 11;
	private final static int S_GOT_LT_BANG = 12;
	private final static int S_SKNG_DASH2 = 13;
	private final static int S_GET_CMMNT = 14;
	private final static int S_SKNG_DASH4 = 15;
	private final static int S_SKNG_EQ = 16;
	private final static int S_GOT_LT_QUESTION = 17;
	private final static int S_GET_DIR_NM = 18;
	private final static int S_1 = 19;
	private final static int S_2 = 20;
	private final static int NSTATES = 22;

	private static String describeState( int state )
	{
		return stateDescr[state];
	}
	
	private static String[] stateDescr = new String[NSTATES];

	//////////////////////
	// TRANSITION TABLE //
	//////////////////////
	
	private final static short[][] transitions = new short[NSTATES][NCC];
	static
	{
		addState( S_CDATA, "gathering CDATA, looking for tag start or EOF" );
		addTransition( CC_LT, S_GOT_LT, A_IGNORE );
		addTransition( CC_EOF, S_CDATA, A_STOP );
		addBackstop( S_CDATA, A_ADD_CHAR_TO_CDATA );

		// pending CDATA: LT
		addState( S_GOT_LT, "looking for stag name, slash, bang, or question" );
		addTransition( CC_X, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_M, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_L, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_OTHR_LTR, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_UNDERSCORE, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_SLASH, S_GOT_LT_SLASH, A_IGNORE );
		addTransition( CC_BANG, S_GOT_LT_BANG, A_IGNORE );
		addTransition( CC_QUESTION, S_GOT_LT_QUESTION, A_IGNORE );
		addBackstop( S_CDATA, A_ADD_LT_CHAR_TO_CDATA );

		// pending CDATA: LT SLASH
		addState( S_GOT_LT_SLASH, "looking for etag name" );
		addTransition( CC_X, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_M, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_L, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_OTHR_LTR, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_UNDERSCORE, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addBackstop( S_CDATA, A_ADD_LT_SLASH_CHAR_TO_CDATA );

		// pending CDATA: LT BANG
		addState( S_GOT_LT_BANG, "looking for directive name or first dash of comment" );
		addTransition( CC_X, S_GET_DIR_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_M, S_GET_DIR_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_L, S_GET_DIR_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_OTHR_LTR, S_GET_DIR_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_UNDERSCORE, S_GET_DIR_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_DASH, S_SKNG_DASH2, A_IGNORE );
		addBackstop( S_CDATA, A_ADD_LT_BANG_CHAR_TO_CDATA );
		
		// pending CDATA: LT QUESTION
		addState( S_GOT_LT_QUESTION, "looking for processing instruction" );
		addTransition( CC_X, S_1, A_IGNORE );
		addTransition( CC_M, S_1, A_IGNORE );
		addTransition( CC_L, S_1, A_IGNORE );
		addTransition( CC_OTHR_LTR, S_1, A_IGNORE );
		addTransition( CC_UNDERSCORE, S_1, A_IGNORE );
		addBackstop( S_CDATA, A_ADD_LT_QUESTION_CHAR_TO_CDATA );
		
		addState( S_1, "gather processing instruction" );
		addTransition( CC_QUESTION, S_2, A_IGNORE );
		addBackstop( S_1, A_IGNORE );
		
		addState( S_2, "ending processing instruction" );
		addTransition( CC_GT, S_CDATA, A_IGNORE );
		addTransition( CC_QUESTION, S_2, A_IGNORE );
		addBackstop( S_1, A_IGNORE );
		
		addState( S_GET_STAG_NM, "gathering stag name" );
		addTransition( CC_X, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_M, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_L, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_OTHR_LTR, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_UNDERSCORE, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_DIGIT, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_DASH, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_DOT, S_GET_STAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_COLON, S_GET_STAG_NM, A_SET_TAG_NAME_QUALIFIER );
		addTransition( CC_WS, S_SKNG_ATTR_NM, A_IGNORE );
		addTransition( CC_SLASH, S_SKNG_GT, A_IGNORE );
		addTransition( CC_GT, S_CDATA, A_START_TAG );

		addState( S_SKNG_GT, "looking for GT to close empty tag" );
		addTransition( CC_GT, S_CDATA, A_EMPTY_TAG );

		addState( S_SKNG_ATTR_NM, "looking for attribute name, or tag close or tag end" );
		addTransition( CC_X, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_M, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_L, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_OTHR_LTR, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_UNDERSCORE, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_WS, S_SKNG_ATTR_NM, A_IGNORE );
		addTransition( CC_SLASH, S_SKNG_GT, A_IGNORE );
		addTransition( CC_GT, S_CDATA, A_START_TAG );

		addState( S_GET_ATTR_NM, "collect attribute name" );
		addTransition( CC_X, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_M, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_L, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_OTHR_LTR, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_UNDERSCORE, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_DIGIT, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_DASH, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_DOT, S_GET_ATTR_NM, A_ADD_CHAR_TO_ATTR_NAME );
		addTransition( CC_COLON, S_GET_ATTR_NM, A_SET_ATTR_NAME_QUALIFIER );
		addTransition( CC_WS, S_SKNG_EQ, A_IGNORE );
		addTransition( CC_EQ, S_SKNG_ATTR_VAL, A_IGNORE );

		addState( S_SKNG_EQ, "seeking the EQ of an attribute spec" );
		addTransition( CC_WS, S_SKNG_EQ, A_IGNORE );
		addTransition( CC_EQ, S_SKNG_ATTR_VAL, A_IGNORE );
		
		addState( S_SKNG_ATTR_VAL, "looking for attribute value" );
		addTransition( CC_WS, S_SKNG_ATTR_VAL, A_IGNORE );
		addTransition( CC_SQ, S_GET_SQ_VALUE, A_IGNORE );
		addTransition( CC_DQ, S_GET_DQ_VALUE, A_IGNORE );

		addState( S_GET_DQ_VALUE, "collecting DQ attribute value" );
		addTransition( CC_DQ, S_SKNG_ATTR_NM, A_FINISH_ATTR );
		addBackstop( S_GET_DQ_VALUE, A_ADD_TO_ATTR_VALUE );

		addState( S_GET_SQ_VALUE, "collecting SQ attribute value" );
		addTransition( CC_SQ, S_SKNG_ATTR_NM, A_FINISH_ATTR );
		addBackstop( S_GET_SQ_VALUE, A_ADD_TO_ATTR_VALUE );

		addState( S_GET_ETAG_NM, "collecting etag name" );
		addTransition( CC_X, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_M, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_L, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_OTHR_LTR, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_UNDERSCORE, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_DIGIT, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_DASH, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_DOT, S_GET_ETAG_NM, A_ADD_CHAR_TO_TAG_NAME );
		addTransition( CC_COLON, S_GET_ETAG_NM, A_SET_TAG_NAME_QUALIFIER );
		addTransition( CC_WS, S_SKNG_WS_STAR_GT, A_IGNORE );
		addTransition( CC_GT, S_CDATA, A_END_TAG );

		addState( S_SKNG_WS_STAR_GT, "looking for end of closing tag" );
		addTransition( CC_WS, S_SKNG_WS_STAR_GT, A_IGNORE );
		addTransition( CC_GT, S_CDATA, A_END_TAG );
		
		addState( S_SKNG_DASH2, "looking for second dash of comment" );
		addTransition( CC_DASH, S_GET_CMMNT, A_IGNORE );

		addState( S_GET_CMMNT, "collecting comment" );
		addTransition( CC_DASH, S_SKNG_DASH4, A_IGNORE );
		addBackstop( S_GET_CMMNT, A_IGNORE );

		addState( S_SKNG_DASH4, "looking for second dash of comment end" );
		addTransition( CC_DASH, S_GOT_LT_BANG, A_IGNORE );
		addBackstop( S_GET_CMMNT, A_IGNORE );
	}
	
	private static void addState( int state, String descr )
	{
		Assertion.check( stateDescr[state] == null, "states[state] == null" );
		staticNextState = state;
		stateDescr[state] = descr;
	}
	
	private static int staticNextState = -1;
	
	private static void addTransition( int cc, int newState, int action )
	{
		Assertion.check( transitions[staticNextState][cc] == 0, "stateActions[staticNextState][cc] == 0" );
		Assertion.check( (newState & ~STATE_MASK) == 0, "(newState & ~STATE_MASK) == 0" );
		Assertion.check( (action & ~ACTION_MASK) == 0, "(action & ~ACTION_MASK) == 0" );
		int sa = newState | action << ACTION_SHIFT;
		transitions[staticNextState][cc] = (short) sa;
	}
	
	private static void addBackstop( int newState, int action )
	{
		Assertion.check( (newState & ~STATE_MASK) == 0, "(newState & ~STATE_MASK) == 0" );
		Assertion.check( (action & ~ACTION_MASK) == 0, "(action & ~ACTION_MASK) == 0" );
		int sa = newState | action << ACTION_SHIFT;
		for (int i = 0; i < NCC; i++)
		{
			if (i == CC_EOF)
				continue;
			
			if (transitions[staticNextState][i] == 0)
				transitions[staticNextState][i] = (short) sa;
		}
	}
	
	/**
	 * Description of Element
	 */
	public interface Element
	{
		/**
		 * @return the element dumped in appropriate format.
		 */
		public String toString();
		
		/**
		 * @param newlineBetweenTags
		 * @param collapseEmptyTags
		 * @return the element dumped in appropriate format.
		 */
		public String toString( boolean newlineBetweenTags,
			boolean collapseEmptyTags );
		
		/**
		 * the element dumped in appropriate format to the string buffer.
		 * @param wtr
		 * @throws IOException
		 */
		public void toString( Writer wtr ) throws IOException;
		
		/**
		 * the element dumped in appropriate format to the string buffer.
		 * @param wtr
		 * @param newlineBetweenTags
		 * @param collapseEmptyTags if true allows <foo></foo> to be
		 * collapsed into <foo/>
		 * @throws IOException
		 */
		public void toString( Writer wtr, boolean newlineBetweenTags,
			boolean collapseEmptyTags ) throws IOException;
	}
	
	/**
	 * Description of AbstractElement.
	 */
	abstract public static class AbstractElement implements Element
	{
		@Override
		final public String toString()
		{
			try
			{
				StringWriter sw = new StringWriter();
				toString( sw );
				return sw.toString();
			}
			catch ( IOException e )
			{
				throw new RuntimeException( "caught exception", e );
			}
		}
		
		final public String toString( boolean newlineBetweenTags, boolean collapseEmptyTags )
		{
			try
			{
				StringWriter sw = new StringWriter();
				toString( sw, newlineBetweenTags, collapseEmptyTags );
				return sw.toString();
			}
			catch ( IOException e )
			{
				throw new RuntimeException( "caught exception", e );
			}
		}

		final public void toString( Writer wtr ) throws IOException
		{
			toString( wtr, true, false );
		}
	}
	
	/**
	 * Description of CdataElement.
	 */
	public interface CdataElement extends Element
	{
		/**
		 * @return the cdata of this element.
		 */
		public String getCdata();
	}
	
	/**
	 * Description of DefaultCdataElement.
	 */
	public static class DefaultCdataElement extends AbstractElement
		implements CdataElement
	{
		/**
		 * Constructs the DefaultCdataElement.
		 *
		 * @param cdata
		 */
		public DefaultCdataElement( String cdata )
		{
			this.cdata = cdata;
		}
		
		public String getCdata()
		{
			return cdata;
		}
		
		private final String cdata;
		
		public void toString( Writer wtr, boolean newlineBetweenTags,
			boolean collapseEmptyTags ) throws IOException
		{
			ProtectCData.write( wtr, cdata );
		}
	}
	
	/**
	 * Description of TagElement.
	 */
	public interface TagElement extends Element, Iterable<Element>
	{
		/**
		 * @return the qualified name of this tag element.
		 */
		public QName getQName();
		
		/**
		 * @param qualifier
		 * @param name
		 * @return true if this tag element matches qualifier and name.
		 */
		public boolean matches( String qualifier, String name );

		/**
		 * @param qName
		 * @return true if the specified qName matches this tag element.
		 */
		public boolean matches( QName qName );

		/**
		 * @param cdata adds some cdata to this tag element.
		 */
		public void addCdata( String cdata );

		/**
		 * @param childQName
		 * @param childAttrs
		 * @return a new tag element suitable to be a child of this element.
		 */
		public TagElement addTag( QName childQName, Map<QName, String> childAttrs );

		/**
		 * @param childQName
		 * @return a new tag element suitable to be a child of this element.
		 */
		public TagElement addTag( QName childQName );

		/**
		 * @param childName
		 * @param childAttrs
		 * @return the new tag that was added.
		 */
		public TagElement addTag( String childName, Map<QName, String> childAttrs );

		/**
		 * @param childName
		 * @return the new tag that was added.
		 */
		public TagElement addTag( String childName );
		
		/**
		 * Removes the specified child element.
		 * @param e
		 * @return true if the element was removed.
		 */
		public boolean remove( Element e );
		
		/**
		 * Removes all the child elements.
		 */
		public void clear();
		
		/**
		 * @return the count of children.
		 */
		public int countChildren();
		
		/**
		 * @param index
		 * @return the specified child.
		 */
		public Element getChild( int index );

		/**
		 * Called as this tag element is popped off the tag element stack.
		 * This means processing of this tag element is now finished.
		 */
		public void finish();

		/**
		 * @return true if there are children.
		 */
		public boolean hasChildren();
		
		/**
		 * @return an iterator over the children of this tag element.
		 */
		public Iterator<Element> getChildren();

		/**
		 * @return returns true if getCdataValue() would return ok.
		 * @see #getCdataValue()
		 */
		public boolean hasCdataValue();

		/**
		 * @return the string value of the child cdata element.
		 * 
		 * @throws NoSuchElementException if there is more than
		 * one child or if there is one child and it is not a
		 * cdata element.
		 */
		public String getCdataValue();
		
		/**
		 * @return true if there are any attributes.
		 */
		public boolean hasAttrs();
		
		/**
		 * @return the set of attr names for this tag
		 */
		public Iterator<QName> getAttrNames();
		
		/**
		 * @param qualifier
		 * @param name
		 * @return attr matching specified parameters, else null.
		 */
		public String getAttr( String qualifier, String name );

		/**
		 * @param qualifier
		 * @param name
		 * @param value
		 * @return this tag element.
		 */
		public TagElement setAttr( String qualifier, String name, String value );
		
		/**
		 * @param qualifier
		 * @param name
		 * @return attr matching the specified parameters, else null.
		 */
		public Integer getIntAttr( String qualifier, String name );

		/**
		 * @param qualifier
		 * @param name
		 * @param value
		 * @return this tag element.
		 */
		public TagElement setAttr( String qualifier, String name, int value );
		
		/**
		 * @param qualifier
		 * @param name
		 * @return attr matching the specified parameters, else null.
		 */
		public Long getLongAttr( String qualifier, String name );

		/**
		 * @param qualifier
		 * @param name
		 * @param value
		 * @return this tag element.
		 */
		public TagElement setAttr( String qualifier, String name, long value );

		/**
		 * Finds the tag element given the specified path. Note that it
		 * will only match the first named element at every step.
		 * @param path a qName path specified as qName!qName!qName...
		 * @return the terminal element of the specified path, else null.
		 */
		public TagElement find( String path );

		/**
		 * Finds the tag element under this one with the specified name.
		 * Note that it will only match the first named element.
		 * @param oqualifier
		 * @param oname
		 * @return the named element, else null.
		 */
		public TagElement find( String oqualifier, String oname );

		/**
		 * Finds the tag element under this one with the specified name.
		 * Note that it will only match the first named element.
		 * @param subQName
		 * @return the named element, else null.
		 */
		public TagElement find( QName subQName );
	}
	
	/**
	 * Default implementation of TagElement.
	 */
	public static class DefaultTagElement extends AbstractElement
		implements TagElement
	{
		/**
		 * Constructs the DefaultTagElement.
		 *
		 * @param qName
		 * @param attrs
		 */
		public DefaultTagElement( QName qName, Map<QName, String> attrs )
		{
			this.qName = qName;
			this.attrs = attrs;
			//System.out.println( "constructed DefaultTagElement( "+qName+" )" );
		}
		
		/**
		 * Constructs the DefaultTagElement.
		 *
		 * @param qName
		 */
		public DefaultTagElement( QName qName )
		{
			this( qName, null );
		}
		
		/**
		 * Constructs the DefaultTagElement.
		 *
		 * @param name
		 * @param attrs
		 */
		public DefaultTagElement( String name, Map<QName, String> attrs )
		{
			this( new QName( name ), attrs );
		}
		
		/**
		 * Constructs the DefaultTagElement.
		 *
		 * @param name
		 */
		public DefaultTagElement( String name )
		{
			this( new QName( name ), null );
		}

		private final QName qName;
		
		private Map<QName, String> attrs;

		///////////////////////////////
		// TagElement Implementation //
		///////////////////////////////
		
		public QName getQName()
		{
			return qName;
		}
		
		public void addCdata( String cdata )
		{
			add( new DefaultCdataElement( cdata ) );
		}

		public TagElement addTag( QName childQName, Map<QName, String> childAttrs )
		{
			DefaultTagElement tagElement = new DefaultTagElement( childQName, childAttrs );
			add( tagElement );
			return tagElement;
		}

		public TagElement addTag( QName childQName )
		{
			return addTag( childQName, null );
		}

		public TagElement addTag( String childName, Map<QName, String> childAttrs )
		{
			return addTag( new QName( childName ), null );
		}

		public TagElement addTag( String childName )
		{
			return addTag( new QName( childName ), null );
		}

		public boolean remove( Element e )
		{
			Iterator<Element> i = getChildren();
			while (i.hasNext())
			{
				if (i.next() == e)
				{
					i.remove();
					return true;
				}
			}
			return false;
		}

		public void clear()
		{
			if (firstElement != null)
				firstElement = null;
			
			if (elements != null)
			{
				elements.clear();
				elements = null;
			}
		}

		public boolean hasAttrs()
		{
			return attrs != null && !attrs.isEmpty();
		}
		
		public Iterator<QName> getAttrNames()
		{
			if (attrs == null)
				return new EmptyIterator<QName>();
			
			return attrs.keySet().iterator();
		}
		
		public String getAttr( String qualifier, String name )
		{
			if (attrs == null)
				return null;
			
			return attrs.get( new QName( qualifier, name ) );
		}
		
		public TagElement setAttr( String qualifier, String name, String value )
		{
			if (attrs == null)
				attrs = new HashMap<QName, String>();
			
			attrs.put( new QName( qualifier, name ), value );
			
			return this;
		}

		public boolean matches( String qualifier, String name )
		{
			return qName.matches( qualifier, name );
		}

		public boolean matches( QName otherQName )
		{
			return qName.matches( otherQName );
		}

		public boolean hasChildren()
		{
			if (firstElement != null)
				return true;
			
			return elements != null && !elements.isEmpty();
		}
		
		public Iterator<Element> getChildren()
		{
			if (firstElement != null)
				return new SingleIterator<Element>( firstElement );
			
			if (elements == null)
				return new EmptyIterator<Element>();
			
			return elements.iterator();
		}

		public Iterator<Element> iterator()
		{
			return getChildren();
		}

		public int countChildren()
		{
			if (firstElement != null)
				return 1;
			
			return elements != null ? elements.size() : 0;
		}

		public Element getChild( int index )
		{
			if (firstElement != null && index == 0)
				return firstElement;
			
			if (elements == null)
				throw new NoSuchElementException();
			
			return elements.get( index );
		}
		
		public void finish()
		{
			// nothing to do.
		}
		
		public boolean hasCdataValue()
		{
			int n = countChildren();
			if (n == 0)
				return true;
			
			if (n > 1)
				return false;
			
			Element e = getChild( 0 );
			
			return e instanceof CdataElement;
		}

		public String getCdataValue()
		{
			int n = countChildren();
			
			if (n == 0)
				return "";
			
			if (n > 1)
				throw new NoSuchElementException( "not just one child" );
			
			Element e = getChild( 0 );
			
			if (!(e instanceof CdataElement))
				throw new NoSuchElementException( "child not cdata element" );
			
			return ((CdataElement) e).getCdata();
		}

		/**
		 * @param element
		 * @return the element added
		 */
		public Element add( Element element )
		{
			if (elements == null)
			{
				if (firstElement == null)
				{
					firstElement = element;
				}
				else
				{
					elements = new ArrayList<Element>( 4 );
					elements.add( firstElement );
					firstElement = null;
					elements.add( element );
				}
			}
			else
			{
				elements.add( element );
			}
			return element;
		}
		
		private List<Element> elements;
		
		private Element firstElement;
		
		public void toString( Writer wtr, boolean newlineBetweenTags,
			boolean collapseEmptyTags ) throws IOException
		{
			if (hasChildren() || !collapseEmptyTags)
			{
				// <tagname attrs>children</tagname>
				wtr.write( '<' );
				qName.toString( wtr );
				if (hasAttrs())
					dumpAttrs( wtr );
				wtr.write( '>' );
				if (newlineBetweenTags && wantNewlineBeforeChildren())
					wtr.write( CRLF );
				
				dumpChildren( wtr, newlineBetweenTags, collapseEmptyTags );
				
				wtr.write( "</" );
				qName.toString( wtr );
				wtr.write( '>' );
			}
			else
			{
				// <tagname attrs/>
				wtr.write( '<' );
				qName.toString( wtr );
				if (hasAttrs())
					dumpAttrs( wtr );
				wtr.write( "/>" );
			}
			
			if (newlineBetweenTags)
				wtr.write( CRLF );
		}
		
		private boolean wantNewlineBeforeChildren()
		{
			// we want newline unless there are no children
			// or only a single child and the child is a
			// cdata.
			
			int n = countChildren();
			
			if (n > 1)
				return true;
			
			if (n == 0)
				return false;
			
			return !(getChild( 0 ) instanceof CdataElement);
		}
		
		private void dumpAttrs( Writer wtr ) throws IOException
		{
			for (Map.Entry<QName, String> me : attrs.entrySet())
			{
				QName qn = me.getKey();
				String value = me.getValue();
				
				wtr.write( ' ' );
				qn.toString( wtr );
				wtr.write( "=\"" );
				ProtectAttrValue.write( wtr, value );
				wtr.write( '"' );
			}
		}

		private void dumpChildren( Writer wtr, boolean newlineBetweenTags,
			boolean collapseEmptyTags ) throws IOException
		{
			for (Element e : this)
				e.toString( wtr, newlineBetweenTags, collapseEmptyTags );
		}

		public TagElement find( String path )
		{
			StringTokenizer st = new StringTokenizer( path, "!" );
			TagElement e = this;
			while (e != null && st.hasMoreTokens())
			{
				String token = st.nextToken();
				QName subQName = new QName( token );
				e = e.find( subQName );
			}
			return e;
		}

		public TagElement find( String oqualifier, String oname )
		{
			for (Element e: this)
			{
				if (e instanceof TagElement)
				{
					TagElement te = (TagElement) e;
					if (te.matches( oqualifier, oname ))
						return te;
				}
			}
			return null;
		}

		public TagElement find( QName subQName )
		{
			for (Element e: this)
			{
				if (e instanceof TagElement)
				{
					TagElement te = (TagElement) e;
					if (te.matches( subQName ))
						return te;
				}
			}
			return null;
		}

		public Integer getIntAttr( String qualifier, String name )
		{
			String s = getAttr( qualifier, name );
			
			if (s == null)
				return null;
			
			return new Integer( s );
		}
		
		public TagElement setAttr( String qualifier, String name, int value )
		{
			return setAttr( qualifier, name, Integer.toString( value ) );
		}

		public Long getLongAttr( String qualifier, String name )
		{
			String s = getAttr( qualifier, name );
			
			if (s == null)
				return null;
			
			return new Long( s );
		}
		
		public TagElement setAttr( String qualifier, String name, long value )
		{
			return setAttr( qualifier, name, Long.toString( value ) );
		}
	}
	
	/**
	 * Subclass of DefaultTagElement for root tags. Needed to put
	 * out xml version tag prefix on toString, etc.
	 */
	public static class RootTagElement extends DefaultTagElement
	{
		/**
		 * @param qName
		 * @param attrs
		 */
		public RootTagElement( QName qName, Map<QName, String> attrs )
		{
			super( qName, attrs );
		}

		/**
		 * @param qName
		 */
		public RootTagElement( QName qName )
		{
			super( qName );
		}

		/**
		 * @param name
		 * @param attrs
		 */
		public RootTagElement( String name, Map<QName, String> attrs )
		{
			super( name, attrs );
		}

		/**
		 * @param name
		 */
		public RootTagElement( String name )
		{
			super( name );
		}
		
		public void toString( Writer wtr, boolean newlineBetweenTags,
			boolean collapseEmptyTags ) throws IOException
		{
			// TODO get proper encoding from somewhere
			wtr.write( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" );
			wtr.write( CRLF );
			super.toString( wtr, newlineBetweenTags, collapseEmptyTags );
		}
	}
	
	/**
	 * Description of QName
	 */
	public static class QName
	{
		/**
		 * @param qualifier
		 * @param name
		 */
		public QName( String qualifier, String name )
		{
			this.qualifier = qualifier;
			this.name = name;
			//System.out.println( "constructed QName( "+this+" )" );
		}

		/**
		 * @param qName
		 */
		public QName( String qName )
		{
			String[] x = StringUtil.leftSplit( qName, ':' );
			if (x != null)
			{
				qualifier = x[0];
				name = x[1];
			}
			else
			{
				qualifier = null;
				name = qName;
			}
		}

		/**
		 * Description of qualifier
		 */
		public final String qualifier;
		
		/**
		 * Description of name
		 */
		public final String name;
		
		/**
		 * @param oqualifier
		 * @param oname
		 * @return true if oqualifier and oname match us
		 */
		public boolean matches( String oqualifier, String oname )
		{
			return matches( oqualifier, qualifier, oname, name );
		}
		
		/**
		 * @param other
		 * @return true if other matches us.
		 */
		public boolean matches( QName other )
		{
			return matches( other.qualifier, qualifier, other.name, name );
		}
		
		/**
		 * @param qualifier1
		 * @param qualifier2
		 * @param name1
		 * @param name2
		 * @return true if the qualifiers match each other and the names match
		 * each other.
		 */
		public static boolean matches( String qualifier1, String qualifier2,
			String name1, String name2 )
		{
			return StringUtil.equals( qualifier1, qualifier2) &&
				StringUtil.equals( name1, name2 );
		}

		@Override
		public int hashCode()
		{
			// must match the usage of StringUtil.equals above.
			return StringUtil.hashCode( qualifier ) ^ StringUtil.hashCode( name );
		}

		@Override
		public boolean equals( Object obj )
		{
			if (obj == this)
				return true;
			
			if (obj == null)
				return false;
			
			if (obj.getClass() != QName.class)
				return false;
			
			QName other = (QName) obj;
			
			return matches( qualifier, other.qualifier, name, other.name );
		}

		@Override
		public String toString()
		{
			try
			{
				StringWriter wtr = new StringWriter();
				toString( wtr );
				return wtr.toString();
			}
			catch ( IOException e )
			{
				throw new RuntimeException( "caught exception", e );
			}
		}
		
		/**
		 * @param wtr
		 * @throws IOException
		 */
		public void toString( Writer wtr ) throws IOException
		{
			if (qualifier != null)
			{
				wtr.write( qualifier );
				wtr.write( ':' );
			}
			wtr.write( name );
		}
		
		/**
		 * @param qualifier
		 * @param name
		 * @return printable form of a qualified name
		 */
		public static String toString( String qualifier, String name )
		{
			if (qualifier != null)
				return qualifier+':'+name;
			return name;
		}
	}
	
	/**
	 * 
	 */
	abstract public static class ProtectBase extends Writer
	{
		/**
		 * @param wtr
		 */
		public ProtectBase( Writer wtr )
		{
			this.wtr = wtr;
		}
		
		/**
		 * Description of wtr
		 */
		protected final Writer wtr;
		
		/**
		 * 
		 */
		public static final String LT = "&lt;";
		
		/**
		 * 
		 */
		public static final String GT = "&gt;";
		
		/**
		 * 
		 */
		public static final String AMP = "&amp;";
		
		/**
		 * 
		 */
		public static final String QUOT = "&quot;";
		
		/**
		 * 
		 */
		public static final String TAB = "&#9;";
		
		/**
		 * 
		 */
		public static final String CR = "&#13;";
		
		/**
		 * 
		 */
		public static final String LF = "&#10;";

		@Override
		public void close()
		{
			// ignore.
		}

		@Override
		public void flush()
		{
			// ignore.
		}

		@Override
		public void write( char[] cbuf, int off, int len ) throws IOException
		{
			for (int i = 0; i < len; i++)
				write( cbuf[off+i] );
		}

		@Override
		public void write( String str, int off, int len ) throws IOException
		{
			for (int i = 0; i < len; i++)
				write( str.charAt( off+i ) );
		}

		@Override
		abstract public void write( int c ) throws IOException;

		/**
		 * Writes a character the normal way: ascii character range
		 * (9, 10, 13, 32-126) is put out as itself, while any other
		 * characters are put out as &#nnnnn;
		 * @param wtr
		 * @param c
		 * @throws IOException
		 */
		protected static void defaultWrite( Writer wtr, int c ) throws IOException
		{
			if (c >= 32 && c <= 126)
			{
				wtr.write( c ); // printable ascii is ok
			}
			else if (c == '\t' || c == '\n' || c == '\r')
			{
				wtr.write( c ); // tab, linefeed, and carriage return are ok.
			}
			else
			{
				wtr.write( "&#" );
				wtr.write( Integer.toString( c ) );
				wtr.write( ';' );
			}
		}
	}
	
	/**
	 * 
	 */
	final public static class ProtectAttrValue extends ProtectBase
	{
		/**
		 * @param wtr
		 */
		public ProtectAttrValue( Writer wtr )
		{
			super( wtr );
		}

		@Override
		public void write( int c ) throws IOException
		{
			write( wtr, c );
		}

		/**
		 * @param wtr
		 * @param s
		 * @throws IOException
		 */
		public static void write( Writer wtr, String s ) throws IOException
		{
			int n = s.length();
			for (int i = 0; i < n; i++)
				write( wtr, s.charAt( i ) );
		}
		
		/**
		 * @param s an attribute name which might have special characters.
		 * @return the string protected as appropriate for an
		 * attribute.
		 * @throws IOException
		 */
		public static String toString( String s ) throws IOException
		{
			StringWriter sw = new StringWriter();
			write( sw, s );
			return sw.toString();
		}
		
		/**
		 * @param wtr
		 * @param c
		 * @throws IOException
		 */
		public static void write( Writer wtr, int c ) throws IOException
		{
			switch (c)
			{
				case '<':
					wtr.write( LT );
					break;
				case '>':
					wtr.write( GT );
					break;
				case '&':
					wtr.write( AMP );
					break;
				case '"':
					wtr.write( QUOT );
					break;
				case '\t':
					wtr.write( TAB );
					break;
				case '\r':
					wtr.write( CR );
					break;
				case '\n':
					wtr.write( LF );
					break;
				default:
					defaultWrite( wtr, c );
					break;
			}
		}
	}
	
	/**
	 * 
	 */
	final public static class ProtectCData extends ProtectBase
	{
		/**
		 * @param wtr
		 */
		public ProtectCData( Writer wtr )
		{
			super( wtr );
		}

		@Override
		public void write( int c ) throws IOException
		{
			write( wtr, c );
		}

		/**
		 * @param wtr
		 * @param s
		 * @throws IOException
		 */
		public static void write( Writer wtr, String s ) throws IOException
		{
			int n = s.length();
			for (int i = 0; i < n; i++)
				write( wtr, s.charAt( i ) );
		}
		
		/**
		 * @param s an attribute name which might have special characters.
		 * @return the string protected as appropriate for an
		 * attribute.
		 * @throws IOException
		 */
		public static String toString( String s ) throws IOException
		{
			StringWriter sw = new StringWriter();
			write( sw, s );
			return sw.toString();
		}
		
		/**
		 * @param wtr
		 * @param c
		 * @throws IOException
		 */
		public static void write( Writer wtr, int c ) throws IOException
		{
			switch (c)
			{
				case '<':
					wtr.write( LT );
					break;
				case '>':
					wtr.write( GT );
					break;
				case '&':
					wtr.write( AMP );
					break;
				default:
					defaultWrite( wtr, c );
					break;
			}
		}
	}
}
