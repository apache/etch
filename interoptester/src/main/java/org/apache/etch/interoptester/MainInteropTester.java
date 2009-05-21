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

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.apache.etch.util.ArrayIterator;
import org.apache.etch.util.StringUtil;
import org.apache.etch.util.cmd.StringArrayIterator;
import org.apache.etch.util.core.xml.XmlParser;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/** */
@SuppressWarnings( "all" )
public class MainInteropTester
{
	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		if (args.length < 1)
		{
			System.err.println( "usage: InteropTester file [name=value ...]" );
			System.exit( 1 );
		}

		StringArrayIterator i = new StringArrayIterator( args );

		TagElement te = new XmlParser().parseOne( new File( i.next() ), null,
			"interoptest" );
		// System.out.println( te );
		InteropTestIntf itest = InteropTest.parse( te );
		te = null;

		Map<String, String> overrides = new HashMap<String, String>();
		while (i.hasNext())
		{
			String s = i.next();
			String[] x = StringUtil.leftSplit( s, '=' );
			if (x == null)
				throw new IllegalArgumentException(
					"malformed 'name=value' token on command line" );
			overrides.put( x[0], x[1] );
		}

		//System.out.println( "main: overrides = " + overrides );

		new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					int c = System.in.read();
					if (c == 'q')
						System.exit( 1 );
				}
				catch ( IOException e )
				{
					// ignore
				}
			}
		} ).start();

		itest.run( overrides );

		System.out.println( "main: done" );
		System.exit( 0 );
	}
}
