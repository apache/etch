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

package org.apache.etch.examples.perf;

import java.util.Date;

import org.apache.etch.util.core.io.Session;



/**
 * Your custom implementation of BasePerfServer. Add methods here to provide
 * implementations of messages from the client.
 */
public class ImplPerfServer extends BasePerfServer
{
	/**
	 * Constructs the ImplPerfServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 */
	public ImplPerfServer( RemotePerfClient client )
	{
		this.client = client;
//		name = "ImplPerfServer@"+Integer.toString( hashCode(), 36 );
//		Log.report( "ImplPerfServerUp", "instance", this );
//		t0 = System.nanoTime();
	}
	
	@SuppressWarnings("unused")
	private final RemotePerfClient client;
	
//	private final String name;
	
//	private final long t0;
	
//	@Override
//	public String toString()
//	{
//		return name;
//	}
	
	@Override
	public Integer add( Integer x, Integer y )
	{
//		adds++;
		return x + y;
	}
	
//	private int adds;

	@Override
	public Integer sum( int[] values )
	{
//		sums++;
		int sum = 0;
		for (int value: values)
			sum += value;
		return sum;
	}
	
//	private int sums;

	@Override
	public void report( Integer code, String msg )
	{
//		reports++;
	}
	
//	private int reports;
	
	@Override
	public Point dist( Point a, Point b )
	{
//		dists++;
		return new Point( b.x - a.x, b.y - a.y );
	}
	
//	private int dists;
	
	@Override
	public Date add2( Date ts, Long ms )
	{
//		add2s++;
		return new Date( ts.getTime()+ms );
	}
	
//	private int add2s;
	
	@Override
	public void report2( Date ts, Integer code, String msg )
	{
//		report2s++;
	}
	
//	private int report2s;

	@Override
	public void _sessionNotify( Object event ) throws Exception
	{
		if (event == Session.DOWN)
		{
//			double time = (System.nanoTime() - t0) / 1000000000.0;
//			int total = adds + sums + reports + dists + add2s + report2s;
//			double rate = total / time;
//			Log.report( "ImplPerfServerDown",
//				"instance", this,
//				"adds", adds,
//				"sums", sums,
//				"reports", reports,
//				"dists", dists,
//				"add2s", add2s,
//				"report2s", report2s,
//				"total", total,
//				"time", time,
//				"rate", rate );
		}
	}
}