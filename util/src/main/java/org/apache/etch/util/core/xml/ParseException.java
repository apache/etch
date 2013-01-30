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
import java.util.List;

import org.apache.etch.util.core.xml.XmlParser.TagElement;



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
