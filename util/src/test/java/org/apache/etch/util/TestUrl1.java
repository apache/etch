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

package org.apache.etch.util;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.junit.Test;

/**
 * @author jacapps
 *
 */
public class TestUrl1
{
	String nullScheme = null;
	String nullUser = null;
	String nullPassword = null;
	String nullHost = null;
	Integer nullPort = null;
	String nullUri = null;
	Set<String> nullParams = new HashSet<String>();
	Map<String,Object> nullTerms = new HashMap<String, Object>();
	String nullFragment = null;

	String defScheme = "tcp";
	String defUser = "admin";
	String defPassword = "metreos";
	String defHost = "localhost";
	int defPort = 10000;
	String defUri = "uri";
	Set<String> defParams = new HashSet<String>();
	Map<String,Object> defTerms = new HashMap<String, Object>();
	String defFragment = "frag";

	String urlStr1 = "tcp://admin:metreos@localhost:10000/uri;param1;param2?term1=value1&term2=value2#frag";
	String urlStr2 = "tcp://admin:metreos@localhost:10000/uri;param2;param1?term1=value1&term2=value2#frag";

//	private URL url = new URL();

	/**
	 * Verify all fields in url against expected values.
	 *
	 * @param url url to verify
	 * @param fragment expected fragment
	 * @param host expected host
	 * @param numParams expected number of params
	 * @param password expected password
	 * @param port expected port
	 * @param scheme expected scheme
	 * @param hasTerms value for hasTerms
	 * @param uri expected uri
	 * @param user expected user
	 */
	public void verifyURL(URL url, String fragment, String host,
			int numParams, String password, Integer port,
			String scheme, boolean hasTerms, String uri, String user)
	{
		// Verify all fields in url against expected values.
		assertEquals(scheme, url.getScheme());
		assertEquals(user, url.getUser());
		assertEquals(password, url.getPassword());
		assertEquals(host, url.getHost());
		assertEquals(port, url.getPort());
		assertEquals(uri, url.getUri());
		assertEquals(fragment, url.getFragment());

		// Verify that the number of params matches the expected value
		int count = url.getParams().length;
		assertEquals(count, numParams);

		/**
		 * Verify that hasTerms matches the expected value
		 */
		assertEquals(hasTerms, url.hasTerms());
	}

	/**
	 * Verify creation of empty URL
	 *
	 * @throws Exception
	 *
	 */
	@Test
	public void testConstructor1() throws Exception
	{
		URL url = new URL();

		int numParams = 0;
		boolean hasTerms = false;

		verifyURL(url, nullFragment, nullHost, numParams, nullPassword, nullPort,
				nullScheme, hasTerms, nullUri, nullUser);
	}

	/**
	 * Verify creation of URL from string
	 *
	 * @throws Exception
	 *
	 */
	@Test
	public void testConstructor2() throws Exception
	{
		URL url = new URL(urlStr1);

		int numParams = 2;
		boolean hasTerms = true;

		verifyURL(url, defFragment, defHost, numParams, defPassword, defPort,
				defScheme, hasTerms, defUri, defUser);

	}

	/**
	 * Verify creation of URL with null string
	 *
	 * @throws Exception
	 *
	 */
	@Test
	public void testConstructor3() throws Exception
	{
		assertEquals( new URL(), new URL( (String) null ) );
	}

	/**
	 * Verify creation of URL with empty string
	 *
	 * @throws Exception
	 *
	 */
	@Test
	public void testConstructor3a() throws Exception
	{
		assertEquals( new URL(), new URL( "" ) );
	}

	/**
	 * Verify creation of URL with invalid string
	 *
	 * @throws Exception
	 *
	 */
	@Test(expected = IllegalArgumentException.class)
	public void testConstructor4() throws Exception
	{
		new URL("whateverblah~*}}[");
	}

	/**
	 * Verify creation of URL from URL
	 *
	 * @throws Exception
	 */
	@Test
	public void testConstructor5() throws Exception
	{
		int numParams = 0;
		boolean hasTerms = false;
		String uri = "";

		URL url = new URL("tcp://localhost:10000");

		verifyURL(url, nullFragment, defHost, numParams, nullPassword, defPort,
				defScheme, hasTerms, uri, nullUser);
	}

	/**
	 * Verify addParam - no params exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testAddParam1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		url.addParam("param1=value1");
		assertEquals("param1=value1", url.getParam("param1="));
	}

	/**
	 * Verify addParam - params exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testAddParam2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		url.addParam("param1=value1");
		url.addParam("param2=value2");
		assertEquals("param1=value1", url.getParam("param1="));
		assertEquals("param2=value2", url.getParam("param2="));
	}

	/**
	 * Verify addParam - param prefix is duplicate
	 *
	 * @throws Exception
	 */
	@Test
	public void testAddParam3() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		url.addParam("param1=value1");
		url.addParam("param1=value1");

		assertEquals("param1=value1", url.getParam("param1="));
	}

	/**
	 * Verify addParam - param prefix is null
	 *
	 * @throws Exception
	 */
	@Test(expected = NullPointerException.class)
	public void testAddParam4() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		url.addParam(null);
	}

	/**
	 * Verify addParam - param prefix is blank
	 *
	 * @throws Exception
	 */
	@Test
	public void testAddParam5() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		url.addParam("");
		assertEquals( "", url.getParam("") );
	}

	/**
	 * Verify addTerm add double term
	 *
	 * @throws Exception
	 */
	@Test
	public void testAddTerm_double1() throws Exception
	{
		double value = 1.7976931348623157e308d;
		String name = "term1";

		URL url = new URL("tcp://localhost:10000");
		url.addTerm(name, value);

		double termValue = new Double(url.getTerm(name));

		assertEquals(value, termValue);

	}

	/**
	 * Verify addTerm add double term
	 *
	 * @throws Exception
	 */
	@Test
	public void testAddTerm_Double1() throws Exception
	{
		Double value = 1.7976931348623157e308d;
		String name = "term1";

		URL url = new URL("tcp://localhost:10000");
		url.addTerm(name, value);

		double termValue = new Double (url.getTerm(name));
		assertEquals(value, termValue);
	}

	/**
	 * Verify addTerm add int term
	 *
	 * @throws Exception
	 */
	@Test
	public void testAddTerm_int1() throws Exception
	{
		int value = 2147483647;
		String name = "term1";

		URL url = new URL("tcp://localhost:10000");
		url.addTerm(name, value);

		int termValue = new Integer (url.getTerm(name));
		assertEquals(value, termValue);
	}

	/**
	 * Verify addTerm add Integer term
	 *
	 * @throws Exception
	 */
	@Test
	public void testAddTerm_int2() throws Exception
	{
		Integer value = 2147483647;
		String name = "term1";

		URL url = new URL("tcp://localhost:10000");
		url.addTerm(name, value);

		int termValue = new Integer (url.getTerm(name));
		assertEquals(value, termValue);
	}

	/**
	 * Verify addTerm add String term
	 *
	 * @throws Exception
	 */
	@Test
	public void testAddTermString() throws Exception
	{
		String value = "value1";
		String name = "term1";

		URL url = new URL("tcp://localhost:10000");
		url.addTerm(name, value);

		assertEquals(value, url.getTerm(name));
	}

	/**
	 * Verify clearParams params exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testClearParams1() throws Exception
	{

		URL url = new URL(urlStr1);

		assertTrue(url.hasParams());
		url.clearParams();
		assertFalse(url.hasParams());
	}

	/**
	 * Verify clearParams params do not exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testClearParams2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		assertFalse(url.hasParams());
		url.clearParams();
		assertFalse(url.hasParams());
	}

	/**
	 * Verify clearTerms terms exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testClearTerms1() throws Exception
	{

		URL url = new URL(urlStr1);

		assertTrue(url.hasTerms());
		url.clearTerms();
		assertFalse(url.hasTerms());
	}

	/**
	 * Verify clearTerms terms do not exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testClearTerms2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		assertFalse(url.hasTerms());
		url.clearTerms();
		assertFalse(url.hasTerms());
	}

	/**
	 * Verify dump
	 *
	 * @throws Exception
	 */
	@Test
	public void testDump() throws Exception
	{
		URL url = new URL(urlStr1);
		url.dump();
	}

	/**
	 * Verify equals true
	 *
	 * @throws Exception
	 */
	@Test
	public void testEquals1() throws Exception
	{
		URL url = new URL(urlStr1);
		URL url2 = new URL(urlStr1);

		assertTrue(url.equals(url2));
	}

	/**
	 * Verify equals false
	 *
	 * @throws Exception
	 */
	@Test
	public void testEquals2() throws Exception
	{
		URL url = new URL(urlStr1);
		URL url2 = new URL("tcp://localhost:10000");

		assertFalse(url.equals(url2));
	}

	/**
	 * Verify equals obj not url
	 *
	 * @throws Exception
	 */
	@Test
	public void testEquals3() throws Exception
	{
		URL url = new URL(urlStr1);
		String url2 = "tcp://localhost:10000";

		assertFalse(url.equals(url2));
	}

	/**
	 * Verify equals obj null
	 *
	 * @throws Exception
	 */
	@Test
	public void testEquals4() throws Exception
	{
		URL url = new URL(urlStr1);
		URL url2 = new URL();

		assertFalse(url.equals(url2));
	}

	/**
	 * Verify getBooleanTerm values are boolean
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetBooleanTerm1() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=true&term2=false#defFragment");
		assertTrue(url.getBooleanTerm("term1"));
		assertFalse(url.getBooleanTerm("term2"));
	}

	/**
	 * Verify getBooleanTerm values are not boolean
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetBooleanTerm2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=value1&term2=value2#defFragment");
		assertFalse(url.getBooleanTerm("term1"));
	}

	/**
	 * Verify getBooleanTerm (String, boolean)
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetBooleanTerm3() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=true&term2=false&term3=other#defFragment");

		/** Term exists and is true */
		assertTrue(url.getBooleanTerm("term1",false));

		/** Term exists and is false */
		assertFalse(url.getBooleanTerm("term2",true));

		/** Term exists and is not a boolean value */
		assertFalse(url.getBooleanTerm("term3",true));

		/** Term does not exists and default value is true */
		assertTrue(url.getBooleanTerm("term4",true));

		/** Term does not exists and default value is false */
		assertFalse(url.getBooleanTerm("term5",false));

		/** Term does not exists and default value is false */
		assertFalse(url.getBooleanTerm("term5",false));

		/** Term invalid */
		assertFalse(url.getBooleanTerm("&&?#",false));
	}
	
	/** @throws Exception */
	@Test(expected = IllegalArgumentException.class )
	public void testGetBooleanTerm4() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=true&term2=false&term3=other#defFragment");

		/** Term null */
		url.getBooleanTerm(null);
	}
	
	/** @throws Exception */
	@Test(expected = IllegalArgumentException.class )
	public void testGetBooleanTerm5() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=true&term2=false&term3=other#defFragment");

		/** Term blank */
		url.getBooleanTerm("");
	}

	/**
	 * Verify getDoubleTerm
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetDoubleTerm1() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=1.7976931348623157e308d&term2=1d#defFragment");

		/** Term exists and is double */
		assertEquals(1.7976931348623157e308d, url.getDoubleTerm("term1"));

		/** Term does not exist */
		assertEquals(null, url.getDoubleTerm("DNE"));
	}
	
	/** @throws Exception */
	@Test(expected = IllegalArgumentException.class )
	public void testGetDoubleTerm2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=1.7976931348623157e308d&term2=1d#defFragment");

		url.getDoubleTerm(null);
	}
	
	/** @throws Exception */
	@Test(expected = IllegalArgumentException.class )
	public void testGetDoubleTerm3() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=1.7976931348623157e308d&term2=1d#defFragment");

		url.getDoubleTerm("");
	}

	/**
	 * Verify getFragment
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetFragment1() throws Exception
	{
		URL url = new URL("tcp://localhost@10000#defFragment");
		URL url2 = new URL("tcp://localhost@10000");

		/** Fragment exists */
		assertEquals("defFragment", url.getFragment());

		/** Fragment does not exist */
		assertEquals(null, url2.getFragment());
	}

	/**
	 * Verify getHost
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetHost1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		URL url2 = new URL();

		/** Host defined for url */
		assertEquals("localhost", url.getHost());

		/** Host not defined for url */
		assertNull(url2.getHost());

	}

	/**
	 * Verify getIntegerTerm(String)
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetIntegerTerm1() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=1&term2=10#defFragment");

		/** Term exists and is integer */
		assertEquals(1, url.getIntegerTerm("term1"));

		/** Term does not exist */
		assertEquals(null, url.getIntegerTerm("DNE"));
	}
	
	/** @throws Exception */
	@Test(expected = IllegalArgumentException.class)
	public void testGetIntegerTerm5() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=1&term2=10#defFragment");

		url.getIntegerTerm(null);
	}
	
	/** @throws Exception */
	@Test(expected = IllegalArgumentException.class)
	public void testGetIntegerTerm6() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=1&term2=10#defFragment");

		url.getIntegerTerm("");
	}

	/**
	 * Verify getIntegerTerm (String, int)
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetIntegerTerm2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=1&term2=1.7d&term3=stringValue#defFragment");

		/** Term exists and is integer */
		assertEquals(1,url.getIntegerTerm("term1",1000));

		/** Term does not exist and default value is integer */
		assertEquals(1000,url.getIntegerTerm("term4",1000));

		/** Term invalid */
		assertEquals(1000,url.getIntegerTerm("&&?#",1000));
	}

	/**
	 * Verify getIntegerTerm (String, int)
	 * Term exists and is a double
	 *
	 * @throws Exception
	 */
	@Test(expected = NumberFormatException.class)
	public void testGetIntegerTerm3() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=1&term2=1.7d&term3=stringValue#defFragment");

		/** Term exists and is a double */
		assertEquals(1.7d,url.getIntegerTerm("term2",1000));
	}

	/**
	 * Verify getIntegerTerm (String, int)
	 * Term exists and is a String
	 *
	 * @throws Exception
	 */
	@Test(expected = NumberFormatException.class)
	public void testGetIntegerTerm4() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=1&term2=1.7d&term3=stringValue#defFragment");

		/** Term exists and is a string */
		assertEquals("stringValue", url.getIntegerTerm("term3",1000));
	}


	/**
	 * Verify getParams - no params
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetParams1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertEquals( 0, url.getParams().length );

	}

	/**
	 * Verify getParams - params exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetParams2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/defUri;param1=value1;param2=value2;param3");
		assertEquals( 3, url.getParams().length );

		url.removeParam("param1=");
		assertEquals( 2, url.getParams().length );

		url.removeParam("param2=");
		assertEquals( 1, url.getParams().length );

		url.removeParam("param3");
		assertEquals( 0, url.getParams().length );
	}

	/**
	 * Verify getPassword - password exists
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetPassword1() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");
		assertEquals("metreos",url.getPassword());
	}

	/**
	 * Verify getPassword - password does not exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetPassword2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertNull(url.getPassword());
	}

	/**
	 * Verify getScheme - scheme exists
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetScheme1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertEquals("tcp",url.getScheme());
	}

	/**
	 * Verify getScheme - port does not exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetScheme2() throws Exception
	{
		URL url = new URL();
		assertNull(url.getScheme());
	}

	/**
	 * Verify getTerm(String) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetTerm1() throws Exception
	{
		URL url = new URL();
		assertNull(url.getTerm("term1"));
	}

	/**
	 * Verify getTerm - term exists
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetTerm2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=value1&term2=#defFragment");

		/** Term exists and has value */
		assertEquals("value1",url.getTerm("term1"));

		/** Term exists but has no value */
		assertEquals("",url.getTerm("term2"));
	}

	/**
	 * Verify getTerm (String, String) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetTerm3() throws Exception
	{
		URL url = new URL();

		/** No terms exist */
		assertEquals("default", url.getTerm("term1","default"));
	}

	/**
	 * Verify getTerm (String, String) - term exists and have value
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetTerm4() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=value1&term2=#defFragment");

		/** Term exists and has value */
		assertEquals("value1", url.getTerm("term1","default"));

		/** TODO I think this should set the value for term2 but it doesn't */
		/** Term exists and has no value */
		//assertEquals("default", url.getTerm("term2","default"));

		/** Terms exists but not this term */
		assertEquals("default", url.getTerm("term3","default"));
	}

	/**
	 * Verify getTermNames - no terms exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetTermNames1() throws Exception
	{
		URL url = new URL();
		assertEquals( 0, url.getTermNames().length );
	}

	/**
	 * Verify getTermNames - terms exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetTermNames2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=value1&term2=value2#defFragment");

		for( String s: url.getTermNames())
		{
			assertTrue(s.equals("term1") || s.equals("term2"));
		}
	}

	/**
	 * Verify getTerms - no terms exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetTerms1() throws Exception
	{
		URL url = new URL();
		assertEquals( 0, url.getTerms("term1").length );
	}

	/**
	 * Verify getTerms - terms exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetTerms2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=value1&term2=value2#defFragment");

		assertEquals( 1, url.getTerms("term1").length );
		assertEquals( "value1", url.getTerms("term1")[0] );
	}

	/**
	 * Verify getUri - uri exists
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetUri1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/defUri");
		assertEquals("defUri",url.getUri());
	}

	/**
	 * Verify getUri - uri does not exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetUri2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertEquals("",url.getUri());
	}

	/**
	 * Verify getUser - user exists
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetUser1() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");
		assertEquals("admin",url.getUser());
	}

	/**
	 * Verify getUri - uri does not exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testGetUser2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertNull(url.getUser());
	}

	/**
	 * Verify hasMultipleValues - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasMultipleValues1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertFalse(url.hasMultipleValues("term1"));
	}

	/**
	 * Verify hasMultipleValues - term exists
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasMultipleValues2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri;param1;param2?term1=value1&term2=value2&term3=#defFragment");

		/** Term exists and has 1 value */
		assertFalse(url.hasMultipleValues("term1"));

		/** Term exists and has multiple values */
		/** TODO: Find out format for multiple values in query term */
		//assertTrue(url.hasMultipleValues("term2"));

		/** Term exists and has no values */
		assertFalse(url.hasMultipleValues("term3"));
	}

	/**
	 * Verify hasParams - no params
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasParams1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertFalse(url.hasParams());
	}

	/**
	 * Verify hasParams - params exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasParams2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/defUri;param1;param2");

		/**  Url has params */
		assertTrue(url.hasParams());
	}

	/**
	 * Verify hasTerm(String) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertFalse(url.hasTerm("term1"));
	}

	/**
	 * Verify hasTerm(String) - url has terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/defUri;param1;param2?term1=value1&term2=value2&term3=");

		/** Term exists */
		assertTrue(url.hasTerm("term1"));

		/** Term does not exist */
		assertFalse(url.hasTerm("DNE"));
	}

	/**
	 * Verify hasTerm(String, double) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm3() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertFalse(url.hasTerm("term1",1.7d));
	}

	/**
	 * Verify hasTerm(String, double) - url has terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm4() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/defUri;param1;param2?term1=1.7");

		/**  Term exists with matching value  */
		assertTrue(url.hasTerm("term1",1.7d));

		/** Term exists but value does not match */
		assertFalse(url.hasTerm("term1",3.0d));
	}

	/**
	 * Verify hasTerm(String, Double) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm5() throws Exception
	{
		Double d = new Double(1.7d);

		URL url = new URL("tcp://localhost:10000");
		assertFalse(url.hasTerm("term1",d));
	}

	/**
	 * Verify hasTerm(String, Double) - url has terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm6() throws Exception
	{
		Double d1 = new Double(1.7d);
		Double d2 = new Double(3.0d);

		URL url = new URL("tcp://localhost:10000/defUri;param1;param2?term1=1.7");

		/**  Term exists with matching value  */
		assertTrue(url.hasTerm("term1",d1));

		/** Term exists but value does not match */
		assertFalse(url.hasTerm("term1",d2));
	}

	/**
	 * Verify hasTerm(String, int) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm7() throws Exception
	{
		int i = 10;

		URL url = new URL("tcp://localhost:10000");
		assertFalse(url.hasTerm("term1",i));
	}

	/**
	 * Verify hasTerm(String, int) - url has terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm8() throws Exception
	{
		int i1 = 10;
		int i2 = 100;

		URL url = new URL("tcp://localhost:10000/defUri;param1;param2?term1=10");

		/**  Term exists with matching value  */
		assertTrue(url.hasTerm("term1",i1));

		/** Term exists but value does not match */
		assertFalse(url.hasTerm("term1",i2));
	}

	/**
	 * Verify hasTerm(String, Integer) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm9() throws Exception
	{
		Integer i = new Integer(10);

		URL url = new URL("tcp://localhost:10000");
		assertFalse(url.hasTerm("term1",i));
	}

	/**
	 * Verify hasTerm(String, Integer) - url has terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm10() throws Exception
	{
		Integer i1 = new Integer(10);
		Integer i2 = new Integer(100);

		URL url = new URL("tcp://localhost:10000/defUri;param1;param2?term1=10");

		/**  Term exists with matching value  */
		assertTrue(url.hasTerm("term1",i1));

		/** Term exists but value does not match */
		assertFalse(url.hasTerm("term1",i2));
	}

	/**
	 * Verify hasTerm(String, String) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm11() throws Exception
	{
		String s = "value1";

		URL url = new URL("tcp://localhost:10000");
		assertFalse(url.hasTerm("term1",s));
	}

	/**
	 * Verify hasTerm(String, String) - url has terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerm12() throws Exception
	{
		String s1 = "value1";
		String s2 = "DNE";

		URL url = new URL("tcp://localhost:10000/defUri;param1;param2?term1=value1");

		/**  Term exists with matching value  */
		assertTrue(url.hasTerm("term1",s1));

		/** Term exists but value does not match */
		assertFalse(url.hasTerm("term1",s2));
	}

	/**
	 * Verify hasTerms - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerms1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");
		assertFalse(url.hasTerms());
	}

	/**
	 * Verify hasTerms - terms exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testHasTerms2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/defUri;param1;param2?term1=value1&term2=value2");

		/**  Url has params */
		assertTrue(url.hasTerms());
	}

	/**
	 * Verify isScheme - no scheme
	 *
	 * @throws Exception
	 */
	@Test
	public void testIsScheme1() throws Exception
	{
		URL url = new URL();
		assertFalse(url.isScheme("tcp"));
	}

	/**
	 * Verify isScheme - scheme exists
	 *
	 * @throws Exception
	 */
	@Test
	public void testIsScheme2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		/**  true - same case */
		assertTrue(url.isScheme("tcp"));

		/**  true different case */
		assertTrue(url.isScheme("TCP"));

		/** false */
		assertFalse(url.isScheme("udp"));
	}

	/**
	 * Verify removeParam - no params
	 *
	 * @throws Exception
	 */
	@Test
	public void testRemoveParam1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		/** No params */
		assertNull(url.removeParam("param1"));
	}

	/**
	 * Verify removeParam - params exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testRemoveParam2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/defUri;param1=value1;param2=");

		/** Param exists and has value */
		assertEquals("param1=value1",url.getParam("param1="));
		assertEquals("param1=value1",url.removeParam("param1="));
		assertNull(url.getParam("param1="));

		/** Param exists and has blank value */
		assertEquals("param2=",url.getParam("param2="));
		assertEquals("param2=",url.removeParam("param2="));
		assertNull(url.getParam("param2="));

		/** Param does not exist */
		assertNull(url.removeParam("DNE="));
	}

	/**
	 * Verify removeTerm(String) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testRemoveTerm1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		/** No terms */
		assertFalse( url.removeTerm("DNE") );
		
		url.addTerm( "a", 1 );
		assertFalse( url.removeTerm("DNE") );
	}

	/**
	 * Verify removeTerm(String) - Terms exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testRemoveTerm2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/defUri;param1=value1;param2=?term1=value1&term2=");

		/** Term exists and has value */
		assertEquals("value1",url.getTerm("term1"));
		assertTrue(url.removeTerm("term1"));
		assertNull(url.getTerm("term1"));

		/** Term exists and has blank value */
		assertEquals("",url.getTerm("term2"));
		assertTrue(url.removeTerm("term2"));
		assertNull(url.getTerm("term2"));

		/** Param does not exist */
		assertFalse(url.removeTerm("DNE"));
	}

	/**
	 * Verify removeTerm(String,String) - no terms
	 *
	 * @throws Exception
	 */
	@Test
	public void testRemoveTerm3() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		/** No terms */
		assertFalse(url.removeTerm("DNE","value1"));
	}

	/**
	 * Verify removeTerm(String,String) - Terms exist
	 *
	 * @throws Exception
	 */
	@Test
	public void testRemoveTerm4() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/defUri;param1=value1;param2=?term1=value1&term2=");

		/** Term exists and has value */
		assertEquals("value1",url.getTerm("term1"));
		assertTrue(url.removeTerm("term1","value1"));
		assertNull(url.getTerm("term1"));

		/** Term exists and has blank value */
		assertEquals("",url.getTerm("term2"));
		assertTrue(url.removeTerm("term2",""));
		assertNull(url.getTerm("term2"));

		/** Param does not exist */
		assertFalse(url.removeTerm("DNE","value1"));
	}

	/**
	 * Verify setFragment - no fragment in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetFragment1() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		/** Set fragment.  No fragment exists in url */
		url.setFragment("fragment1");
		assertEquals("fragment1",url.getFragment());
	}

	/**
	 * Verify setFragment - fragment exists in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetFragment2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/#defFragment");

		/** Set fragment.  Fragment exists in url */
		url.setFragment("fragment1");
		assertEquals("fragment1",url.getFragment());

	}

	/**
	 * Verify setFragment - set fragment to null
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetFragment3() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/#defFragment");

		/** Set fragment to null */
		url.setFragment(null);
		assertNull(url.getFragment());
	}

	/**
	 * Verify setFragment - set fragment to blank
	 *
	 * @throws Exception
	 */
	@Test(expected = IllegalArgumentException.class)
	public void testSetFragment4() throws Exception
	{
		URL url = new URL("tcp://localhost:10000/#defFragment");

		/** Set fragment to blank */
		url.setFragment("");
		assertEquals("",url.getFragment());
	}

	/**
	 * Verify setFHost - no host in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetHost1() throws Exception
	{
		URL url = new URL();

		/** Set host.  No host exists in url */
		url.setHost("host1");
		assertEquals("host1",url.getHost());
	}

	/**
	 * Verify setHost - host exists in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetHost2() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		/** Set host.  Host exists in url */
		url.setHost("host1");
		assertEquals("host1",url.getHost());
	}

	/**
	 * Verify setHost- set host to null
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetHost3() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		/** Set host to null */
		url.setHost(null);
		assertNull(url.getHost());
	}

	/**
	 * Verify setHostt - set host to blank
	 *
	 * @throws Exception
	 */
	@Test(expected = IllegalArgumentException.class)
	public void testSetHost4() throws Exception
	{
		URL url = new URL("tcp://localhost:10000");

		/** Set host to blank */
		url.setHost("");
		assertEquals("",url.getHost());
	}

	/**
	 * Verify setPassword - no password in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetPassword1() throws Exception
	{
		URL url = new URL();

		/** Set password.  No password exists in url */
		url.setPassword("password1");
		assertEquals("password1",url.getPassword());
	}

	/**
	 * Verify setPassword - password exists in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetPassword2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set password.  Password exists in url */
		url.setPassword("password1");
		assertEquals("password1",url.getPassword());
	}

	/**
	 * Verify setPassword- set password to null
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetPassword3() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set password to null */
		url.setPassword(null);
		assertNull(url.getPassword());
	}

	/**
	 * Verify setPassword - set password to blank
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetPassword4() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set password to blank */
		url.setPassword("");
		assertEquals("",url.getPassword());
	}

	/**
	 * Verify setPort - no port in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetPort1() throws Exception
	{
		URL url = new URL();

		/** Set port.  No port exists in url */
		url.setPort(10000);
		assertEquals(10000,url.getPort());
	}

	/**
	 * Verify setPort - port exists in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetPort2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set port.  Port exists in url */
		url.setPort(5000);
		assertEquals(5000,url.getPort());
	}

	/**
	 * Verify setPort - set port less than min value
	 *
	 * @throws Exception
	 */
	@Test(expected = IllegalArgumentException.class)
	public void testSetPort3() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set port less than min value */
		url.setPort(-1);
	}

	/**
	 * Verify setPort - set port greater than max value
	 *
	 * @throws Exception
	 */
	@Test(expected = IllegalArgumentException.class)
	public void testSetPort4() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set port to negative number */
		url.setPort(65536);
	}

	/**
	 * Verify setScheme - no scheme in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetScheme1() throws Exception
	{
		URL url = new URL();

		/** Set scheme.  No scheme exists in url */
		url.setScheme("scheme1");
		assertEquals("scheme1",url.getScheme());
	}

	/**
	 * Verify setScheme - scheme exists in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetScheme2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set scheme.  Scheme exists in url */
		url.setScheme("scheme1");
		assertEquals("scheme1",url.getScheme());
	}

	/**
	 * Verify setScheme- set scheme to null
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetScheme3() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set scheme to null */
		url.setScheme(null);
		assertNull(url.getScheme());
	}

	/**
	 * Verify setScheme - set scheme to blank
	 *
	 * @throws Exception
	 */
	@Test(expected = IllegalArgumentException.class)
	public void testSetScheme4() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set scheme to blank */
		url.setScheme("");
		assertEquals("",url.getScheme());
	}

	/**
	 * Verify setUri - no uri in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetUri1() throws Exception
	{
		URL url = new URL();

		/** Set uri.  No uri exists in url */
		url.setUri("uri1");
		assertEquals("uri1",url.getUri());
	}

	/**
	 * Verify setUri - uri exists in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetUri2() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri");

		/** Set uri.  Uri exists in url */
		url.setUri("uri1");
		assertEquals("uri1",url.getUri());
	}

	/**
	 * Verify setUri- set uri to null
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetUri3() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri");

		/** Set uri to null */
		url.setUri(null);
		assertNull(url.getUri());
	}

	/**
	 * Verify setUri - set uri to blank
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetUri4() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000/defUri");
		assertEquals("defUri",url.getUri());
		
		/** Set uri to blank */
		url.setUri("");
		assertEquals("",url.getUri());
	}

	/**
	 * Verify setUser - no user in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetUser1() throws Exception
	{
		URL url = new URL();
		assertNull( url.getUser() );
		
		/** Set user.  No user exists in url */
		url.setUser("user1");
		assertEquals("user1",url.getUser());
	}

	/**
	 * Verify setUser - user exists in url
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetUser2() throws Exception
	{
		URL url = new URL(urlStr1);
		assertEquals("admin",url.getUser());

		/** Set user.  User exists in url */
		url.setUser("user1");
		assertEquals("user1",url.getUser());
	}

	/**
	 * Verify setUser- set user to null
	 *
	 * @throws Exception
	 */
	@Test
	public void testSetUser3() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set user to null */
		url.setUser(null);
		assertNull(url.getUser());
	}

	/**
	 * Verify setUser - set user to blank
	 *
	 * @throws Exception
	 */
	@Test(expected = IllegalArgumentException.class)
	public void testSetUser4() throws Exception
	{
		URL url = new URL("tcp://admin:metreos@localhost:10000");

		/** Set user to blank */
		url.setUser("");
		assertEquals("",url.getUser());
	}

	/**
	 * Verify toString - url is null
	 *
	 * @throws Exception
	 */
	@Test
	public void testToString1a() throws Exception
	{
		assertEquals( "null:", new URL().toString() );
	}

	/**
	 * Verify toString - url is null
	 *
	 * @throws Exception
	 */
	@Test
	public void testToString1b() throws Exception
	{
		assertEquals( "null:", new URL( (String) null ).toString() );
	}

	/**
	 * Verify toString - url is null
	 *
	 * @throws Exception
	 */
	@Test
	public void testToString1c() throws Exception
	{
		assertEquals( "null:", new URL( "" ).toString() );
	}

	/**
	 * Verify toString
	 *
	 * @throws Exception
	 */
	@Test
	public void testToString2() throws Exception
	{
//		checkToString( urlStr1 );
		
		checkToString( "null:" );
		
		checkToString( "a:b" );
		checkToString( "a:b/c" );
		checkToString( "a:/b" );
		checkToString( "a:b/" );
		
//		checkToString( "a://host" ); // cannot distinguish from next case.
		checkToString( "a://h/" );
		checkToString( "a://h/b" );
//		checkToString( "a:///b" ); // host is blank.

//		checkToString( "a://h:-1" ); // port too negative
		checkToString( "a://h:0/" );
		checkToString( "a://h:1/" );
		checkToString( "a://h:65535/" );
//		checkToString( "a://h:65536" ); // port too big
//		checkToString( "a://h:/" ); // port is blank
//		checkToString( "a://h:b/" ); // port is not integer

		checkToString( "a://u:p@h/" );
//		checkToString( "a://:p@h/" ); // user is blank
//		checkToString( "a://u:@h/" ); // password is blank
//		checkToString( "a://:@h/" ); // user is blank

		checkToString( "a:?b=c" );
		checkToString( "a:?d=e" );

		checkToString( "a:;p" );
		checkToString( "a:;q" );

		checkToString( "a:#f" );
		checkToString( "a:#g" );
	}

	private void checkToString( String s )
	{
		assertEquals( s, new URL( s ).toString() );
	}
}


