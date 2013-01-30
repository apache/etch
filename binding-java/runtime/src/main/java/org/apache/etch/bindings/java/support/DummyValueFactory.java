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

import java.util.Set;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.msg.Validator.Level;


/** Dummy implementation of ValueFactory used for testing. */
public class DummyValueFactory implements ValueFactory
{
	public StructValue exportCustomValue( Object value )
		throws UnsupportedOperationException
	{
		throw new UnsupportedOperationException();
	}

	public Type getCustomStructType( Class<?> c )
		throws UnsupportedOperationException
	{
		throw new UnsupportedOperationException();
	}

	public Long getInReplyTo( Message msg )
	{
		throw new UnsupportedOperationException();
	}

	public Long getMessageId( Message msg )
	{
		throw new UnsupportedOperationException();
	}

	public String getStringEncoding()
	{
		throw new UnsupportedOperationException();
	}

	public Type getType( Integer id )
	{
		throw new UnsupportedOperationException();
	}

	public Type getType( String name )
	{
		throw new UnsupportedOperationException();
	}

	public Set<Type> getTypes()
	{
		throw new UnsupportedOperationException();
	}

	public Type get_mt__exception()
	{
		throw new UnsupportedOperationException();
	}

	public Object importCustomValue( StructValue struct )
		throws UnsupportedOperationException
	{
		throw new UnsupportedOperationException();
	}

	public void setInReplyTo( Message msg, Long msgid )
	{
		throw new UnsupportedOperationException();
	}

	public void setMessageId( Message msg, Long msgid )
	{
		throw new UnsupportedOperationException();
	}

	public void addType( Type type )
	{
		throw new UnsupportedOperationException();
	}

	public Field get_mf__inReplyTo()
	{
		throw new UnsupportedOperationException();
	}

	public Field get_mf__messageId()
	{
		throw new UnsupportedOperationException();
	}

	public void lockDynamicTypes()
	{
		throw new UnsupportedOperationException();
	}

	public void unlockDynamicTypes()
	{
		throw new UnsupportedOperationException();
	}

	public Level setLevel( Level level )
	{
		throw new UnsupportedOperationException();
	}

	public Level getLevel()
	{
		return Level.FULL;
	}
}