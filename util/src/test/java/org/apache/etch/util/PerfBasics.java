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

import java.util.HashMap;
import java.util.Map;

/**
 * Basic perf tests.
 */
public class PerfBasics
{
	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		final int time = 15;
		final int count = 1;
		
//		new PerfTest( "null", time, count )
//		{
//			@Override
//			public void run( long n ) throws Exception
//			{
//				while (n-- > 0);
//			}
//		}.run();
		
//		new PerfTest( "add", time, count )
//		{
//			@Override
//			public void run( long n ) throws Exception
//			{
//				int x = 0;
//				while (n-- > 0)
//					x += 1;
//			}
//		}.run();
		
//		new PerfTest( "func", time, count )
//		{
//			@Override
//			public void run( long n ) throws Exception
//			{
//				while (n-- > 0)
//					func();
//			}
//		}.run();
		
//		new PerfTest( "sfunc", time, count )
//		{
//			@Override
//			public void run( long n ) throws Exception
//			{
//				while (n-- > 0)
//					sfunc();
//			}
//		}.run();
		
//		new PerfTest( "addWithTry", time, count )
//		{
//			@Override
//			public void run( long n ) throws Exception
//			{
//				int x = 0;
//				while (n-- > 0)
//				{
//					try
//					{
//						x += 1;
//					}
//					catch ( Exception e )
//					{
//						// do nothing.
//					}
//				}
//			}
//		}.run();
		
		final String k1 = "a";
		final String k2 = "b";
		final String k3 = "c";
		final String k4 = "d";
		
		final String v1 = "1";
		final String v2 = "2";
		final String v3 = "3";
		
		new PerfTest( "getHit", time, count )
		{
			@Override
			public void run( long n ) throws Exception
			{
				Map<String, Object> d = new HashMap<String, Object>();
				d.put( k1, v1 );
				d.put( k2, v2 );
				d.put( k3, v3 );
				while (n-- > 0)
					d.get( k1 );
			}
		}.run();
		
		new PerfTest( "getMiss", time, count )
		{
			@Override
			public void run( long n ) throws Exception
			{
				Map<String, Object> d = new HashMap<String, Object>();
				d.put( k1, v1 );
				d.put( k2, v2 );
				d.put( k3, v3 );
				while (n-- > 0)
					d.get( k4 );
			}
		}.run();
	}
	
	/**
	 * 
	 */
	public static void func()
	{
		// nothing to do.
	}
	
	/**
	 * 
	 */
	public synchronized static void sfunc()
	{
		// nothing to do.
	}
}
