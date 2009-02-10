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

package org.apache.etch.bindings.java.msg;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import org.apache.etch.bindings.java.support.DummyValueFactory;
import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.bindings.java.support.Validator_long;
import org.junit.Test;


/**
 * Tests Message.
 */
public class TestMessage
{
	private final MyValueFactory vf = new MyValueFactory();
	private final Type mt1 = new Type( 1, "mt" );
	private final Type rmt = new Type( 2, "rmt" );
	private final Field mf1 = new Field( 3, "x" );
	private final Field mf2 = new Field( 4, "y" );
	
	{
		mt1.putValidator( mf1, Validator_int.get( 0 ) );
		mt1.putValidator( mf2, Validator_int.get( 0 ) );
		mt1.putValidator( vf.mf_messageId, Validator_long.get( 0 ) );
		
		rmt.putValidator( vf.mf_messageId, Validator_long.get( 0 ) );
		rmt.putValidator( vf.mf_inReplyTo, Validator_long.get( 0 ) );
	}
	
	/** */
	@Test
	public void getVf()
	{
		Message msg = new Message( mt1, vf );
		assertSame( vf, msg.vf() );
	}

	/** */
	@Test
	public void reply()
	{
		Message msg = new Message( mt1, vf );
		msg.setMessageId( 12345L );

		Message rmsg = msg.reply( rmt );
		assertSame( rmt, rmsg.type() );
		assertSame( vf, rmsg.vf() );
		assertEquals( 12345L, rmsg.getInReplyTo() );
	}

	/** */
	@Test
	public void getMessageId()
	{
		Message msg = new Message( mt1, vf );
		assertNull( msg.getMessageId() );
		msg.setMessageId( 234L );
		assertEquals( 234L, msg.getMessageId() );
	}

	/** */
	@Test
	public void setMessageId()
	{
		Message msg = new Message( mt1, vf );
		assertNull( msg.getMessageId() );
		msg.setMessageId( 234L );
		assertEquals( 234L, msg.getMessageId() );
		msg.setMessageId( 345L );
		assertEquals( 345L, msg.getMessageId() );
	}

	/** */
	@Test
	public void getInReplyTo()
	{
		Message msg = new Message( rmt, vf );
		assertNull( msg.getInReplyTo() );
		msg.setInReplyTo( 234L );
		assertEquals( 234L, msg.getInReplyTo() );
	}

	/** */
	@Test
	public void setInReplyTo()
	{
		Message msg = new Message( rmt, vf );
		assertNull( msg.getInReplyTo() );
		msg.setInReplyTo( 234L );
		assertEquals( 234L, msg.getInReplyTo() );
		msg.setInReplyTo( 345L );
		assertEquals( 345L, msg.getInReplyTo() );
	}

	/** */
	static class MyValueFactory extends DummyValueFactory
	{
		///////////////
		// MESSAGEID //
		///////////////
		
		@Override
		public Long getMessageId( Message msg )
		{
			return (Long) msg.get( mf_messageId );
		}

		@Override
		public void setMessageId( Message msg, Long msgid )
		{
			msg.put( mf_messageId, msgid );
		}

		@Override
		public Field get_mf__messageId()
		{
			return mf_messageId;
		}

		/** */
		public final Field mf_messageId = new Field( "_messageId" );

		///////////////
		// INREPLYTO //
		///////////////
		
		@Override
		public Long getInReplyTo( Message msg )
		{
			return (Long) msg.get( mf_inReplyTo );
		}

		@Override
		public void setInReplyTo( Message msg, Long msgid )
		{
			msg.put( mf_inReplyTo, msgid );
		}

		@Override
		public Field get_mf__inReplyTo()
		{
			return mf_inReplyTo;
		}

		/** */
		public final Field mf_inReplyTo = new Field( "_inReplyTo" );
	}
}
