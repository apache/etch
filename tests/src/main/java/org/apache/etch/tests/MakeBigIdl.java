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

package org.apache.etch.tests;

import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

/**
 * Program to construct a large idl file for testing limits.
 */
public class MakeBigIdl
{
	/**
	 * @param args
	 */
	public static void main( String[] args ) throws IOException
	{
		int n = 1000;
		PrintWriter pw = new PrintWriter( new FileWriter( "src/etch/examples/bigidl/BigIdl.etch" ) );
		pw.println( "module etch.examples.bigidl" );
		pw.println( "service BigIdl" );
		pw.println( "{" );
		for (int i = 0; i < n; i++)
			pw.printf( "\tint add%d( int x, int y )\n", i );
		pw.println( "}" );
		pw.close();
	}
}
