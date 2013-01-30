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

/**
 * Interfaces used by the tree walker to report what it finds.
 */
public interface TreeWalker
{
	/**
	 * Reports a module, before we visit the children of the module.
	 * @param module
	 */
	public void preModule( Module module );
	
	/**
	 * Reports a module, after we visit the children of the module.
	 * @param module
	 */
	public void postModule( Module module );

	/**
	 * Reports a service, before we visit the children of the service.
	 * @param service
	 */
	public void preService( Service service );
	
	/**
	 * Reports a service, after we visit the children of the service.
	 * @param service
	 */
	public void postService( Service service );
	
	/**
	 * Reports a mixin.
	 * @param mixin
	 */
	public void doMixin( Mixin mixin );
	
	/**
	 * Reports a constant, before we visit the children of the constant.
	 * @param constant
	 */
	public void preConstant( Constant constant );
	
	/**
	 * Reports a constant, after we visit the children of the constant.
	 * @param constant
	 */
	public void postConstant( Constant constant );
	
	/**
	 * Reports an enum, before we visit the children of the enum.
	 * @param enumx
	 */
	public void preEnum( Enumx enumx );
	
	/**
	 * Reports an enum, after we visit the children of the enum.
	 * @param enumx
	 */
	public void postEnum( Enumx enumx );
	
	/**
	 * Reports an item of an enum.
	 * @param item
	 */
	public void doItem( Item item );
	
	/**
	 * Reports a struct, before we visit the children of the struct.
	 * @param struct
	 */
	public void preStruct( Struct struct );
	
	/**
	 * Reports a struct, after we visit the children of the struct.
	 * @param struct
	 */
	public void postStruct( Struct struct );
	
	/**
	 * Reports an extern.
	 * @param extern
	 */
	public void doExtern( Extern extern );
	
	/**
	 * Reports an except, before we visit the children of the except.
	 * @param except
	 */
	public void preExcept( Except except );
	
	/**
	 * Reports an except, after we visit the children of the except.
	 * @param except
	 */
	public void postExcept( Except except );
	
	/**
	 * Reports a message, before we visit the children of the message.
	 * @param message
	 */
	public void preMessage( Message message );
	
	/**
	 * Reports a message, after we visit the children of the message.
	 * @param message
	 */
	public void postMessage( Message message );
	
	/**
	 * Reports a parameter, before we visit the children of the parameter.
	 * @param parameter
	 */
	public void preParameter( Parameter parameter );
	
	/**
	 * Reports a parameter, after we visit the children of the parameter.
	 * @param parameter
	 */
	public void postParameter( Parameter parameter );
	
	/**
	 * Reports a thrown exception of a message.
	 * @param thrown
	 */
	public void doThrown( Thrown thrown );
	
	/**
	 * Reports a type reference.
	 * @param ref
	 */
	public void doTypeRef( TypeRef ref );
}
