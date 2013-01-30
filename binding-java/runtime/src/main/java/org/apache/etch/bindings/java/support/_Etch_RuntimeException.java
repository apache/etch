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

package org.apache.etch.bindings.java.support;

/**
 * A RuntimeException which exposes its msg field. Returned by the stub
 * to the remote when the stub catches a RuntimeException.
 * 
 * Some common runtime exceptions are:
 * 
 * {@link ArithmeticException}
 * {@link ClassCastException}
 * {@link IllegalArgumentException}
 * {@link IndexOutOfBoundsException}
 * {@link NullPointerException}
 * {@link UnsupportedOperationException}
 */
public class _Etch_RuntimeException
	extends RuntimeException
{
	private static final long serialVersionUID = 3051515541923877063L;

	/**
	 * Constructs the EtchRuntimeException. Don't init anything.
	 */
	public _Etch_RuntimeException()
	{
		// don't init anything.
	}
	
	/**
	 * Constructs the EtchRuntimeException.
	 * @param msg description of the exception that was caught by the stub
	 * on the remote side.
	 */
	public _Etch_RuntimeException( String msg )
	{
		this.msg = msg;
	}

	@Override
	public String getMessage()
	{
		return msg;
	}

	/**
	 * Description of the exception that was caught by the stub
	 * on the remote side.
	 */
	public String msg;
	
	@Override
	public String toString()
	{
		return "Remote side threw this runtime exception: "+getMessage();
	}
}
