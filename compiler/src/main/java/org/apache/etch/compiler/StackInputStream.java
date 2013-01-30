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

import java.io.IOException;
import java.io.InputStream;

/**
 * Input stream to support include in the etch compiler.
 */
public class StackInputStream extends InputStream
{
	static int nOnce = 0;
	
	private InputStream mainStream = null;
	private InputStream includeStream = null;
	private InputStream currentStream = null;

	/**
	 * Constructs the StackInputStream.
	 * @param in1
	 */
	public StackInputStream(InputStream in1)
	{
		mainStream = in1;
		currentStream = mainStream;
	}

	/**
	 * @param in2
	 */
	public void setStreamTwo(InputStream in2)
	{
		includeStream = in2;
	}

	/**
	 * @param val
	 */
	public void ActivateStream(int val)
	{
		if(val == 0)
		{
			currentStream = mainStream;
		}
		else if(val == 1)
		{
			currentStream = includeStream;
		}
	}
	
	@Override
	public int read()
	{
		int val = 0;
		try
		{
			val = currentStream.read();
		}
		catch ( IOException e )
		{
			e.printStackTrace();
		}
		
		if(val == -1 && nOnce == 0 && includeStream != null)
		{
			System.out.println("------Stack Reset To Stream 0 >>>> " + val);
			ActivateStream(1);
			try
			{
				val = currentStream.read();
			}
			catch ( IOException e )
			{
				e.printStackTrace();
			}
			nOnce = 1;
		}
		
	
		System.out.println("------Stack Read >>>> " + val);
		return val;
	}
}
