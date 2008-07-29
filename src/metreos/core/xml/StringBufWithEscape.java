/* $Id: StringBufWithEscape.java 21902 2006-04-12 13:28:38Z wert $
 *
 * Created by wert on Apr 27, 2005.
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.core.xml;

import java.util.Map;

/**
 * Description of StringBufWithEscape.
 */
public class StringBufWithEscape extends PlainStringBuf
{
	/**
	 * Constructs the StringBufWithEscape.
	 *
	 * @param descr
	 * @param maxLen
	 * @param maxEscapeLen
	 * @param escapes
	 */
	public StringBufWithEscape( String descr, int maxLen,
		int maxEscapeLen, Map<String,Character> escapes )
	{
		super( descr, maxLen );
		this.maxEscapeLen = maxEscapeLen;
		this.escapes = escapes;
	}
	
	private final int maxEscapeLen;
	
	private final Map<String,Character> escapes;
	
	@Override
	public void append( char c )
	{
		if (c == '&')
		{
			if (escapeBuffer != null)
				throw new UnsupportedOperationException( "bad char '"+c+"' in escape" );
			escapeBuffer = new PlainStringBuf( "escape", maxEscapeLen );
		}
		else if (escapeBuffer != null)
		{
			if (c == ';')
			{
				String s = escapeBuffer.toString();
				escapeBuffer = null;
				super.append( parseEscape( s ) );
			}
			else
			{
				escapeBuffer.append( c );
			}
		}
		else
		{
			super.append( c );
		}
	}
	
	private char parseEscape( String s )
	{
		if (s.length() == 0)
			throw new UnsupportedOperationException( "empty escape" );
		
		if (s.startsWith( "#" ))
		{
			try
			{
				int k = Integer.parseInt( s.substring( 1 ) );
				
				if (k < 0 || k > 65535)
					throw new UnsupportedOperationException( "numeric escape out of range '"+s+"'" );
				
				return (char) k;
			}
			catch ( NumberFormatException e )
			{
				throw new UnsupportedOperationException( "bad numeric escape '"+s+"'" );
			}
		}
		
		Character c = escapes.get( s );
		if (c != null)
			return c.charValue();
		
		c = escapes.get( s.toLowerCase() );
		if (c != null)
			return c.charValue();
		
		throw new UnsupportedOperationException( "unknown entity escape '"+s+"'" );
	}
	
	@Override
	public int length()
	{
		int n = super.length();
		
		if (escapeBuffer != null)
			return n+1;
		
		return n;
	}

	@Override
	public String toString()
	{
		if (escapeBuffer != null)
			throw new UnsupportedOperationException( "unfinished escape" );
		
		return super.toString();
	}

	private PlainStringBuf escapeBuffer;
}
