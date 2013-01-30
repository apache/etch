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

package org.apache.etch.interoptester;

import java.util.Iterator;

import org.apache.etch.util.core.xml.XmlParser.CdataElement;
import org.apache.etch.util.core.xml.XmlParser.Element;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/**
 * ChildWalker calls a method with each child element under a given tag
 * element.
 */
abstract public class ChildWalker
{
	/**
	 * Iterates over the children of r and calls tag or cdata for each.
	 * @param r
	 */
	public void walk( TagElement r )
	{
		for (Iterator<Element> i = r.getChildren(); i.hasNext();)
		{
			Element e = i.next();
			if (!(e instanceof TagElement))
			{
				if (!cdata( (CdataElement) e ))
					throw new IllegalArgumentException(
						"tag "+r.getQName()+": cdata not allowed here: '"+e+"'" );
				
				continue;
			}
			
			TagElement te = (TagElement) e;
			if (!tag( te ))
				throw new IllegalArgumentException(
					"tag "+r.getQName()+": tag "+te.getQName()+" not allowed here" );
		}
	}

	/**
	 * @param te
	 * @return if tag is allowed.
	 */
	public boolean tag( TagElement te )
	{
		return false;
	}
	
	/**
	 * @param e
	 * @return true if cdata is allowed.
	 */
	public boolean cdata( CdataElement e )
	{
		String s = e.getCdata();
		return s.trim().length() == 0;
	}
}