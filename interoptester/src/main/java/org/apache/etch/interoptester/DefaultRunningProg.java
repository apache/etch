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


import java.util.List;
import java.util.Map;

import org.apache.etch.interoptester.Program.ProcessStreamReader;

/**
 * Default implementation of {@link RunningProg}.
 */
public class DefaultRunningProg implements RunningProg
{
	/**
	 * @param tkns
	 * @param nvs
	 * @param stdoutTag
	 * @param stderrTag
	 */
	public DefaultRunningProg( List<String> tkns, Map<String, String> nvs,
		String stdoutTag, String stderrTag )
	{
		this.tkns = tkns;
		this.nvs = nvs;
		this.stdoutTag = stdoutTag;
		this.stderrTag = stderrTag;
	}

	private final List<String> tkns;

	private final Map<String, String> nvs;

	private final String stdoutTag;

	private final String stderrTag;

	private ProcessBuilder pb;

	private Process p;

	private ProcessStreamReader outrdr;

	private ProcessStreamReader errrdr;

	private Integer sts;

	@Override
	public String toString()
	{
		return "DefaultRunningProg@" + Integer.toHexString( hashCode() );
	}

	/**
	 * @throws Exception
	 */
	public void start() throws Exception
	{
		pb = new ProcessBuilder( tkns );
		pb.environment().putAll( nvs );
		
		h = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					stop();
				}
				catch ( InterruptedException e )
				{
					// ignore
				}
			}
		} );
		
		Runtime.getRuntime().addShutdownHook( h );

		report( "start, tokens = " + tkns + ", nvs = " + nvs );

		p = pb.start();

		report( "started" );

		p.getOutputStream().close();

		outrdr = new ProcessStreamReader( p.getInputStream(), System.out,
			stdoutTag );
		outrdr.start();

		errrdr = new ProcessStreamReader( p.getErrorStream(), System.out,
			stderrTag );
		errrdr.start();
	}
	
	private Thread h;

	public Integer join() throws InterruptedException
	{
		if (p != null)
		{
			report( "join" );

			sts = p.waitFor();
			p = null;
			
			try
			{
				Runtime.getRuntime().removeShutdownHook( h );
			}
			catch ( IllegalStateException e )
			{
				// ignore
			}
			h = null;

			report( "join, sts = " + sts );

			if (outrdr != null)
				outrdr.join();

			if (errrdr != null)
				errrdr.join();

			return sts;
		}

		report( "join: no program" );
		return null;
	}

	public Integer stop() throws InterruptedException
	{
		report( "stop" );

		if (p != null)
			p.destroy();

		return join();
	}

	private void report( String msg )
	{
		//System.out.println( this.toString() + ": " + msg );
	}
}
