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

package org.apache.etch.compiler;

import static org.junit.Assert.assertEquals;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import org.apache.etch.compiler.ast.Message;
import org.apache.etch.compiler.ast.Module;
import org.apache.etch.compiler.ast.Named;
import org.apache.etch.compiler.ast.ParamList;
import org.apache.etch.compiler.ast.Service;
import org.apache.etch.compiler.ast.TypeRef;
import org.junit.Before;
import org.junit.Test;


/**
 * UnitTest for the mapWords(String, Map<String, String>) method in Backend.
 * These tests rely on mapWordsTestFileOne and mapWordsTestFileTwo being in the
 * same directory as this file for testing.  Testing includes parsing these
 * files, checking recovery from bad syntax and expected results from succesful
 * parsing.
 */
public class TestMapWords
{
	/**
	 * A concrete implementation of Backend.  The only function you may assume
	 * works is mapWords(String, Map<String, String>.
	 */
	private class WordMapper extends Backend
	{
		/**
		 *
		 * @throws Exception
		 */
		public WordMapper()
			throws Exception
		{
			// nothing to do.
		}

		@Override
		public String asyncReceiverPoolName( Message msg )
		{
			return null;
		}

		@Override
		public String enum_efqname( String fqname, String moduleName,
			String serviceName, String enumName )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String except_efqname( String fqname, String moduleName,
			String serviceName, String enumName )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String formatString( ParamList<Service> n, boolean isExcept )
			throws ParseException, IOException
		{
			// Not needed for testing
			return null;
		}

		@Override
		public void generate( Module module, CmdLineOptions options )
			throws Exception
		{
			// Not needed for testing
		}

		@Override
		public String getLang()
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String getNativeTypeName( TypeRef type )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String getTypeName( TypeRef type )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String getTypeValue( TypeRef type, Token value )
			throws IOException
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String getValidator( Named<?> named )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String mfvname( String vname )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String mtvname( String vname )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String qualifyConstantName( Service intf, Token name )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String qualifyEnumName( Service intf, Token name )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String qualifyParameterName( Token name )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public String struct_efqname( String fqname, String moduleName,
			String serviceName, String enumName )
		{
			// Not needed for testing
			return null;
		}

		@Override
		public void mapWords(String fileName, Map<String, String> wordMap)
			throws IOException, FileNotFoundException
		{
			super.mapWords(fileName, wordMap);
		}

		@Override
		public void addDefaults( Service service ) throws ParseException
		{
			// Not needed for testing
		}
	}


	private static final String tmpPath1 = "org/apache/etch/compiler/";
	private static final String tmpPath2 = null;
	private static final String tmpPath3 = "resources/org/apache/etch/compiler/";
	private static final String[] path = {tmpPath1, tmpPath2, tmpPath3};

	private static final String firstFile = "mapWordsTestFileOne.kwd";
	private static final String secondFile = "mapWordsTestFileTwo.kwd";

	private static Map<String, String> myMap;
	private static WordMapper myMapper;

	private static Map<String, String> firstMapping;
	private static Map<String, String> firstSecondMapping;


	/**
	 * Reinitialize baseline before each test.  To be safe.
	 *
	 * @throws Exception
	 */
	@Before
	public void startUp()
		throws Exception
	{
		myMap = new HashMap<String, String>();

		myMapper = new WordMapper();
		myMapper.mapWords(getPath(path, firstFile), myMap);

		firstMapping = new HashMap<String, String>();
		firstSecondMapping = new HashMap<String, String>();

		// The results of parsing the firstFile into a new map
		for (int i = 1; i < 5; ++i)
			firstMapping.put("key" + Integer.toString(i), "xkey" + Integer.toString(i));
		for (int i = 5; i < 16; ++i)
			firstMapping.put("key" + Integer.toString(i), "value" + Integer.toString(i));
		firstMapping.put("key16", "value17");
		firstMapping.put("key17", "value17");
		firstMapping.put("key18", "xkey18");
		firstMapping.put("Key19", "XKey19");
		firstMapping.put("KEY20", "XKEY20");
		firstMapping.put("kEy21", "xkEy21");

		// The results of parsing the firstFile into a new map, and then parsing
		// the secondFile into the same map
		firstSecondMapping.putAll(firstMapping);
		firstSecondMapping.put("key1", "foo");
		firstSecondMapping.put("key5", "xkey5");
		firstSecondMapping.put("Key19", "bar");
		firstSecondMapping.put("KEY20", "KEY20");
		firstSecondMapping.put("key22", "xkey22");
		firstSecondMapping.put("key23", "value23");
	}


	/**
	 * Basic test to ensure files can be parsed without comments, whitespace or
	 * bad syntax crashing the function.
	 *
	 * @throws Exception
	 */
	@Test
	public void simpleParse()
		throws Exception
	{
		assertEquals(firstMapping.size(), myMap.size());
	}

	/**
	 * Overall test to see if the first file parsed with expected results.
	 *
	 * @throws Exception
	 */
	@Test
	public void expectedResults()
		throws Exception
	{
		assertEquals(firstMapping, myMap);
	}

	/**
	 * Assert comments trailing mappings aren't parsed.
	 *
	 * @throws Exception
	 */
	@Test
	public void trailingComments()
		throws Exception
	{
		// comments after implicit mappings
		assertEquals(firstMapping.get("key1"), myMap.get("key1"));
		assertEquals(firstMapping.get("key2"), myMap.get("key2"));
		assertEquals(firstMapping.get("key3"), myMap.get("key3"));
		assertEquals(firstMapping.get("key4"), myMap.get("key4"));

		// comments after explicit mappings
		assertEquals(firstMapping.get("key6"), myMap.get("key6"));
		assertEquals(firstMapping.get("key7"), myMap.get("key7"));
		assertEquals(firstMapping.get("key8"), myMap.get("key8"));
		assertEquals(firstMapping.get("key10"), myMap.get("key10"));
		assertEquals(firstMapping.get("key11"), myMap.get("key11"));
		assertEquals(firstMapping.get("key13"), myMap.get("key13"));
		assertEquals(firstMapping.get("key14"), myMap.get("key14"));
		assertEquals(firstMapping.get("key15"), myMap.get("key15"));
	}

	/**
	 * Assert explicit mappings work.
	 *
	 * @throws Exception
	 */
	@Test
	public void explicitMappings()
		throws Exception
	{
		assertEquals(firstMapping.get("key5"), myMap.get("key5"));
		assertEquals(firstMapping.get("key6"), myMap.get("key6"));
		assertEquals(firstMapping.get("key7"), myMap.get("key7"));
		assertEquals(firstMapping.get("key8"), myMap.get("key8"));
		assertEquals(firstMapping.get("key9"), myMap.get("key9"));
		assertEquals(firstMapping.get("key10"), myMap.get("key10"));
		assertEquals(firstMapping.get("key11"), myMap.get("key11"));
		assertEquals(firstMapping.get("key12"), myMap.get("key12"));
		assertEquals(firstMapping.get("key13"), myMap.get("key13"));
		assertEquals(firstMapping.get("key14"), myMap.get("key14"));
		assertEquals(firstMapping.get("key15"), myMap.get("key15"));
	}

	/**
	 * Assert implicit mappings work.
	 *
	 * @throws Exception
	 */
	@Test
	public void implicitMappings()
		throws Exception
	{
		assertEquals(firstMapping.get("key1"), myMap.get("key1"));
		assertEquals(firstMapping.get("key2"), myMap.get("key2"));
		assertEquals(firstMapping.get("key3"), myMap.get("key3"));
		assertEquals(firstMapping.get("key4"), myMap.get("key4"));
		assertEquals(firstMapping.get("key18"), myMap.get("key18"));
		assertEquals(firstMapping.get("key19"), myMap.get("key19"));
		assertEquals(firstMapping.get("key20"), myMap.get("key20"));
		assertEquals(firstMapping.get("key21"), myMap.get("key21"));
	}

	/**
	 * Assert repeated mappings overwrite.
	 *
	 * @throws Exception
	 */
	@Test
	public void overwrittenMappings()
		throws Exception
	{
		assertEquals(firstMapping.get("key16"), myMap.get("key16"));
		assertEquals(firstMapping.get("key17"), myMap.get("key17"));
		assertEquals(firstMapping.get("key18"), myMap.get("key18"));
	}

	/**
	 * Assert implicit mappings have an uppercase 'x' where appropriate.
	 *
	 * @throws Exception
	 */
	@Test
	public void upcasing()
		throws Exception
	{
		assertEquals(firstMapping.get("key19"), myMap.get("key19"));
		assertEquals(firstMapping.get("key20"), myMap.get("key20"));
		assertEquals(firstMapping.get("key21"), myMap.get("key21"));
	}

	/**
	 * Assert that repeated mappings cause change when mapping a second file.
	 *
	 * @throws Exception
	 */
	@Test
	public void twoParsings()
		throws Exception
	{
		assertEquals(firstMapping, myMap);
		myMapper.mapWords(getPath(path, secondFile), myMap);
		assertEquals(firstSecondMapping, myMap);
	}

	/**
	 * Assert that repeated mappings change the mappings they should
	 *
	 * @throws Exception
	 */
	@Test
	public void multiParsings()
		throws Exception
	{
		assertEquals(firstMapping, myMap);
		myMapper.mapWords(getPath(path, secondFile), myMap);
		assertEquals(firstSecondMapping, myMap);
		myMapper.mapWords(getPath(path, firstFile), myMap);

		// Add the new parsings and re-check
		firstMapping.put("key22", "xkey22");
		firstMapping.put("key23", "value23");
		assertEquals(firstMapping.size(), myMap.size());
		assertEquals(firstMapping, myMap);
	}

	// -------
	// Helpers
	// -------

	/**
	 * Returns the correct path to a resource.
	 * @param path the paths to try for opening the resources
	 * @param fn the file name of the file to open
	 * @return the correct path to a resource
	 */
	private static String getPath(String[] path, String fn)
		throws FileNotFoundException
	{
		for (String p: path)
		{
			if (p == null)
				continue;
			
			String s = p + fn;
			
			URL temp = ClassLoader.getSystemResource(s);
			if (temp != null)
					return s;
		}
		throw new FileNotFoundException(fn);
	}
}
