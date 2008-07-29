/* $Id: ParseException.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Mar 1, 2006.
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.core.xml;

import java.io.IOException;
import java.util.List;

import metreos.core.xml.XmlParser.TagElement;

/**
 * Description of SyntaxException
 */
public class ParseException extends IOException
{
	private static final long serialVersionUID = -4649152474648809695L;

	/**
	 * @param msg
	 */
	public ParseException( String msg )
	{
		super( msg );
	}
	
	/**
	 * @param msg
	 * @param cause 
	 */
	public ParseException( String msg, Throwable cause )
	{
		super( msg );
		initCause( cause );
	}

	/**
	 * @return the context of the error
	 */
	public String getContext()
	{
		return context;
	}
	
	/**
	 * @param context
	 */
	public void setContext( String context )
	{
		this.context = context;
	}

	@Override
	public String toString()
	{
		return super.toString()+" in '"+context+"' at "+tagElementStack;
	}
	
	private String context;

	/**
	 * @param tagElementStack
	 */
	public void setTagElementStack( List<TagElement> tagElementStack )
	{
		this.tagElementStack = tagElementStack;
	}
	
	private List<TagElement> tagElementStack;
}
