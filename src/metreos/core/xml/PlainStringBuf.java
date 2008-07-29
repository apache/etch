/* $Id: PlainStringBuf.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Apr 27, 2005.
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.core.xml;

/**
 * Description of PlainStringBuf.
 */
public class PlainStringBuf implements StringBuf
{
	/**
	 * Constructs the PlainStringBuf.
	 *
	 * @param descr
	 * @param maxLen
	 */
	public PlainStringBuf( String descr, int maxLen )
	{
		this.descr = descr;
		this.maxLen = maxLen;
	}
	
	private final String descr;
	
	private final int maxLen;

	/**
	 * @return the descr of this string buf.
	 */
	public String getDescr()
	{
		return descr;
	}

	/* (non-Javadoc)
	 * @see metreos.core.xml.StringBufIntf#append(char)
	 */
	public void append( char c )
	{
		if (sb.length() >= maxLen)
			throw new UnsupportedOperationException(
				"maximum length of "+descr+" exceeded: "+maxLen );

		sb.append( c );
	}

	/* (non-Javadoc)
	 * @see metreos.core.xml.StringBufIntf#append(java.lang.String)
	 */
	public void append( String s )
	{
		// this must process a character at a time so that subclassers
		// will work properly.
		int n = s.length();
		for (int i = 0; i < n; i++)
			append( s.charAt( i )  );
	}

	/* (non-Javadoc)
	 * @see metreos.core.xml.StringBufIntf#length()
	 */
	public int length()
	{
		return sb.length();
	}

	@Override
	public String toString()
	{
		return sb.toString();
	}
	
	/* (non-Javadoc)
	 * @see metreos.core.xml.StringBuf#clear()
	 */
	public void clear()
	{
		sb.setLength( 0 );
	}
	
	private final StringBuffer sb = new StringBuffer();
}
