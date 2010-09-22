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

package org.apache.etch.compiler.ast;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.opt.AsyncReceiver;
import org.apache.etch.compiler.opt.Authorize;
import org.apache.etch.compiler.opt.Direction;
import org.apache.etch.compiler.opt.Oneway;
import org.apache.etch.compiler.opt.Timeout;


/**
 * Description of Message.
 */
public class Message extends ParamList<Service>
{
	/**
	 * Constructs the Message.
	 * @param intf
	 * @param name
	 * @param nOpts
	 * @param type
	 */
	public Message( Service intf, Name name, Map<String, Opt> nOpts, TypeRef type )
	{
		super( intf, name, nOpts );

		md = getMessageDirection();
		this.type = type;
	}

	private MessageDirection md;

	private final TypeRef type;
	
	/**
	 * @return the suffix of this message's interface given the direction.
	 */
	public String suffix()
	{
		return MsgDirHelper.getSuffix( md );
	}
	
	/**
	 * @return the vname of this message (the fully qualified name with "_" instead of ".").
	 */
	public String mkVname()
	{
		Service intf = parent();
		Name name = name();
		return intf.parent().name().name.replace( '.', '_' )+'_'+intf.name()+'_'+name.name;
	}
	
	/**
	 * @return the fully qualified name of this message.
	 */
	public String mkFqName()
	{
		Service intf = parent();
		Name name = name();
		return intf.parent().name().name+'.'+intf.name().name+'.'+name.name;
	}

	/**
	 * @return the direction of this message.
	 */
	public MessageDirection msgDir()
	{
		return md;
	}

	/**
	 * Sets the message direction of this message.
	 * @param md
	 */
	public void setMsgDir( MessageDirection md )
	{
		this.md = md;
	}

	/**
	 * @param other
	 * @return true if this message's direction equals other.
	 */
	public boolean isMsgDir( MessageDirection other )
	{
		return md == other;
	}

	/**
	 *
	 * @return true if this message's direction is server
	 */
	public boolean isMsgDirServer()
	{
		return md == MessageDirection.SERVER;
	}

	/**
	 *
	 * @return true if this message's direction is client
	 */
	public boolean isMsgDirClient()
	{
		return md == MessageDirection.CLIENT;
	}
	
	/**
	 *
	 * @return trus if this message's direction is both
	 */
	public boolean isMsgDirBoth()
	{
		return md == MessageDirection.BOTH;
	}

	/**
	 * @return the message direction per the Direction opt.
	 */
	public MessageDirection getMessageDirection()
	{
		Opt o = getOpt( "Direction" );

		if (o == null)
			return MessageDirection.SERVER;

		Direction d = (Direction) o;
		return d.getMessageDirection();
	}

	/**
	 * @return the message timeout per the Timeout opt.
	 */
	public int getTimeout()
	{
		Opt o = getOpt( "Timeout" );

		if (o == null)
			return 0;

		Timeout t = (Timeout) o;
		return t.timeout();
	}

	/**
	 * @return the type of this message.
	 */
	public TypeRef type()
	{
		return type;
	}

	@Override
	public String vname( Backend helper )
	{
		return helper.mtvname( mkVname() );
	}

	/**
	 * @return the fully qualified name of this message.
	 */
	@Override
	public String fqname()
	{
		return mkFqName();
	}

//	/**
//	 * @return the id of this message.
//	 */
//	@Override
//	public Integer id()
//	{
//		return mkId();
//	}

	@Override
	public String toString()
	{
		return String.format( "message %s %s %s (%s) throws %s", md, type, name(), getParameters(), thrown );
	}

	/**
	 * @return the message description.
	 */
	public List<String> returnDescr()
	{
		return returnDescr;
	}

	/**
	 * @param returnDescr
	 */
	public void setReturnDescr( List<String> returnDescr )
	{
		this.returnDescr = returnDescr;
	}

	private List<String> returnDescr = new ArrayList<String>();

	/**
	 * @return the message used to return a result from a call.
	 */
	public Message getResultMessage()
	{
		return resultMessage;
	}

	/**
	 * @param resultMessage
	 */
	public void setResultMessage( Message resultMessage )
	{
		this.resultMessage = resultMessage;
	}

	private Message resultMessage;

	/**
	 * @return true if this is a message which contributes to the
	 * types and fields but should not appear in the interface
	 * (because it is a response message)
	 */
	public boolean isHidden()
	{
		return name().name.startsWith( RESULT_MESSAGE_NAME_PREFIX );
	}

	/**
	 * @return the expected name of a result message.
	 */
	public Name getResultMessageName()
	{
		return new Name( null, RESULT_MESSAGE_NAME_PREFIX+name().name );
	}

	/**
	 * The name of the parameter of a result message.
	 */
	public static final String RESULT_PARAM_NAME = "result";

	/**
	 * The prefix to apply to the name of a result message.
	 */
	public static final String RESULT_MESSAGE_NAME_PREFIX = "_result_";

	/**
	 * @return the name of the result parameter.
	 */
	public Name getResultParamName()
	{
		return new Name( null, RESULT_PARAM_NAME );
	}

	/**
	 * @return the result parameter.
	 */
	public Parameter getResultParam()
	{
		return getParameter( RESULT_PARAM_NAME );
	}

	/**
	 * @return true if this is a call (i.e., returns a value.
	 */
	public boolean hasReturn()
	{
		return type.type().kind != EtchGrammarConstants.VOID;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%smethod %s %s %s\n", indent, md, type, name().name );

		String sIndent = indent+"  ";

		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );

		for (Opt o: opts())
			o.dump( sIndent );

		for (Parameter p: this)
			p.dump( sIndent );

		if (hasReturn())
			System.out.printf( "%sreturnDescr %s\n", sIndent, returnDescr );

		for (Thrown n: thrown)
			n.dump( sIndent );
	}

	/**
	 * Adds a name to the list of exceptions thrown by this message.
	 * @param n
	 * @return the representation object for a thrown exception.
	 * @throws ParseException
	 */
	public Thrown addThrown( Name n ) throws ParseException
	{
		if (isOneway())
			throw new ParseException( String.format(
				"Oneway message cannot throw exception at line %d", n.token.beginLine ) );

		thrown.check( n );
		Thrown t = new Thrown( this, n );
		thrown.add( n, t );
		return t;
	}

	/**
	 * @param n
	 * @return any thrown exception by the specified name.
	 */
	public Thrown getThrown( String n )
	{
		return thrown.get( n );
	}

	private final NameList<Thrown> thrown = new NameList<Thrown>();

	/**
	 * @return true if this message has thrown exceptions.
	 */
	public boolean hasThrown()
	{
		return !thrown.list().isEmpty();
	}

	/**
	 * @return the async receiver mode.
	 */
	public AsyncReceiverMode getAsyncReceiver()
	{
		AsyncReceiver ar = (AsyncReceiver) getOpt( "AsyncReceiver" );

		if (ar == null)
			return AsyncReceiverMode.NONE;

		return ar.getMode();
	}

	/**
	 * @return true if this method is marked to have an asynchronous
	 * receiver.
	 */
	public boolean isAsyncReceiver()
	{
		return getAsyncReceiver() != AsyncReceiverMode.NONE;
	}

	/**
	 * @return true if this method is marked to have a queued asynchronous
	 * receiver.
	 */
	public boolean isQueuedAsyncReceiver()
	{
		return getAsyncReceiver() == AsyncReceiverMode.QUEUED;
	}

	/**
	 * @return true if this method is marked to have a free asynchronous
	 * receiver.
	 */
	public boolean isFreeAsyncReceiver()
	{
		return getAsyncReceiver() == AsyncReceiverMode.FREE;
	}

	/**
	 * @return true if this is a oneway message which does not wait
	 * for any response from the other end.
	 */
	public boolean isOneway()
	{
		Oneway oneway = (Oneway) getOpt( "Oneway" );
		return oneway != null && oneway.isOneway();
	}

	/**
	 * @return true if this message requires authorization.
	 */
	public Authorize getAuth()
	{
		return (Authorize) getOpt( "Authorize" );
	}

	/**
	 * @return true if this message requires authorization.
	 */
	public boolean hasAuth()
	{
		Authorize auth = getAuth();
		return auth != null && auth.hasAuth();
	}

	@Override
	public void check() throws ParseException
	{
		checkMessageName( name() );
		
		type.check( parent() );

		for (Parameter p: this)
			p.check();

		for (Thrown n: thrown)
			n.check();

		Authorize auth = getAuth();
		if (auth != null)
			auth.check( parent(), this );
	}

	@Override
	public Service service()
	{
		return parent();
	}

	/**
	 * @return the list of thrown exceptions.
	 */
	public NameList<Thrown> thrown()
	{
		return thrown;
	}

	/**
	 * @return the request message of this result message.
	 */
	public Message getRequestMessage()
	{
		return reqMsg;
	}

	/**
	 * Sets the request message of this result message.
	 * @param reqMsg
	 */
	public void setRequestMessage( Message reqMsg )
	{
		this.reqMsg = reqMsg;
	}

	private Message reqMsg;

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.preMessage( this );

		walker.doTypeRef( type );

		for (Parameter p: this)
			p.treewalk( walker );

		for (Thrown n: thrown)
			n.treewalk( walker );

		walker.postMessage( this );
	}
}
