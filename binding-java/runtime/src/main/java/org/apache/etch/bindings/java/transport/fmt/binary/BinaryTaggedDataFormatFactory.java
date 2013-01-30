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

package org.apache.etch.bindings.java.transport.fmt.binary;

import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.transport.FormatFactory;
import org.apache.etch.bindings.java.transport.TaggedDataInput;
import org.apache.etch.bindings.java.transport.TaggedDataOutput;

/**
 * Format factory for binary tagged data.
 */
public class BinaryTaggedDataFormatFactory extends FormatFactory
{
	@Override
	public TaggedDataInput newTaggedDataInput( ValueFactory vf, String uri )
	{
		return new BinaryTaggedDataInput( vf, uri );
	}

	@Override
	public TaggedDataOutput newTaggedDataOutput( ValueFactory vf, String uri )
	{
		return new BinaryTaggedDataOutput( vf, uri );
	}
}