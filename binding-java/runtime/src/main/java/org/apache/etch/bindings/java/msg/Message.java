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

/**
 * A message is modeled as a command and some argments. The command is an arbitrary
 * integer value, and the arguments are key / value pairs, where the
 * key is an arbitrary integer value and the value is any one of the
 * standard java objects, an ArrayValue, a StructValue, or any type
 * which may be serialized by the ValueFactory.
 */
public final class Message extends StructValue
{
	/**
	 * A bogus serial version uid.
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Constructs the Message with default length.
	 *
	 * @param type the type of the message (command).
	 * @param vf the value factory.
	 */
	public Message( Type type, ValueFactory vf )
	{
		super( type, vf );
		this.vf = vf;
	}

	/**
	 * Constructs the Message.
	 *
	 * @param type the type of the message (command).
	 * @param vf the value factory.
	 * @param length the number of name / value pairs expected. 0 means use
	 * default.
	 */
	public Message( Type type, ValueFactory vf, int length )
	{
		super( type, vf, length );
		this.vf = vf;
	}

	private final ValueFactory vf;
	
	/**
	 * @return the value factory.
	 */
	public ValueFactory vf()
	{
		return vf;
	}

	/**
	 * Creates a message which is a reply to the current message.
	 * The current message's value factory is copied to the new
	 * message. The message-id of the current message (if any) is
	 * copied into the in-reply-to field of the new message.
	 * @param rType the type of the reply.
	 * @return a reply message.
	 */
	public Message reply( Type rType )
    {
	    Message rmsg = new Message( rType, vf );
	    rmsg.setInReplyTo( getMessageId() );
	    return rmsg;
    }
	
	/**
	 * @return a message which is the appropriate reply to this message.
	 */
	public Message reply()
	{
	    Message rmsg = new Message( type().getResult(), vf );
	    rmsg.setInReplyTo( getMessageId() );
	    return rmsg;
	}

	/**
	 * @return the connection specific unique identifier of this
	 * message, or null if there was no such identifier.
	 */
	public Long getMessageId()
    {
	    return vf.getMessageId( this );
    }
	
	/**
	 * Sets the message-id field of this message.
	 * @param msgid the connection specific unique identifier of this
	 * message. Null if the message has not been sent yet. NOTE: the
	 * send process overwrites any value the user might set here. So
	 * don't bother trying to set it.
	 */
	public void setMessageId( Long msgid )
	{
		vf.setMessageId( this, msgid );
	}
	
	/**
	 * @return the message-id of the message that this is a response to.
	 * Null if this is an original message or if the original message did
	 * not have a message-id.
	 */
	public Long getInReplyTo()
	{
		return vf.getInReplyTo( this );
	}
	
	/**
	 * Sets the in-reply-to field of this message.
	 * @param msgid the message-id of the message that this is a response to.
	 */
	public void setInReplyTo( Long msgid )
	{
		vf.setInReplyTo( this, msgid );
	}
}
