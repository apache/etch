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

import java.util.HashMap;

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;

public class Signal extends Named<Service> {

	public Signal( Service intf, Name name )
	{
		super(intf, name, new HashMap<String, Opt>());
		
		moduleName = intf.parent().name().name;
		serviceName = intf.name().name;
		signalName = name.name;

		fqname = moduleName+'.'+serviceName+'.'+signalName;
		vname = fqname.replace( '.', '_' );
		
		signalSubscribeMessage = createSubscriptionMessage("_Subscribe" + signalName + "Signal");
		signalUnsubscribeMessage = createSubscriptionMessage("_Unsubscribe" + signalName + "Signal");
	}
	
	private final String moduleName;
	
	private final String serviceName;
	
	private final String signalName;
	
	private final String vname;
	
	private final String fqname;
	
	private final Message signalSubscribeMessage;

	private final Message signalUnsubscribeMessage;

	@Override
	public String vname(Backend helper)
	{
		return helper.mtvname( vname );
	}

	@Override
	public String fqname()
	{
		return fqname;
	}

	@Override
	public void dump(String indent)
	{
		System.out.printf( "%signal %s\n", indent, name() );
	}

	@Override
	public void check() throws ParseException
	{
	}

	@Override
	public void treewalk(TreeWalker walker) throws ParseException
	{
	}
	
	public String getSignalName()
	{
		return signalName;
	}
	
	public Message getSignalSubscribeMessage()
	{
		return signalSubscribeMessage;
	}

	public Message getSignalUnsubscribeMessage()
	{
		return signalUnsubscribeMessage;
	}
	
	private Message createSubscriptionMessage(String strName)
	{
		Name name = new Name(null, strName);

		Token type = new Token();
		type.kind = EtchGrammarConstants.VOID;
		type.image = "void";
		TypeRef typeRef = new TypeRef( parent(), type );
	
		HashMap<String, Opt> opts = new HashMap<String, Opt>();

		Message m = new Message( parent(), name, opts, typeRef);
		m.setMsgDir(MessageDirection.SERVER);
		m.setMessageSignal(this);
		
		return m;
	}
}
