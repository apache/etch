package org.apache.etch.bindings.cpp.compiler;
/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */


import java.util.List;

import org.apache.etch.compiler.ast.Constant;
import org.apache.etch.compiler.ast.Enumx;
import org.apache.etch.compiler.ast.Except;
import org.apache.etch.compiler.ast.Extern;
import org.apache.etch.compiler.ast.Item;
import org.apache.etch.compiler.ast.Message;
import org.apache.etch.compiler.ast.Mixin;
import org.apache.etch.compiler.ast.Module;
import org.apache.etch.compiler.ast.Parameter;
import org.apache.etch.compiler.ast.Service;
import org.apache.etch.compiler.ast.Struct;
import org.apache.etch.compiler.ast.Thrown;
import org.apache.etch.compiler.ast.TreeWalker;
import org.apache.etch.compiler.ast.TypeRef;

public class IncludeTreeWalker implements TreeWalker {

	private List<String> list;
	private Service service;

	public IncludeTreeWalker(List<String> list, Service s) {
		this.list = list;
		this.service = s;
	}

	@Override
	public void doExtern(Extern extern) {
		// TODO Auto-generated method stub

	}

	@Override
	public void doItem(Item item) {
		// TODO Auto-generated method stub

	}

	@Override
	public void doMixin(Mixin mixin) {
		String s = mixin.name().name(); //.intf().name().name();
		if (!getList().contains(s) && 
			!s.equals(service.name().name())) {
			getList().add(s);
		}

	}

	@Override
	public void doThrown(Thrown thrown) {
		// TODO Auto-generated method stub

	}

	@Override
	public void doTypeRef(TypeRef ref) {
		
	}

	@Override
	public void postConstant(Constant constant) {
		// TODO Auto-generated method stub

	}

	@Override
	public void postEnum(Enumx enumx) {
		// TODO Auto-generated method stub

	}

	@Override
	public void postExcept(Except except) {
		// TODO Auto-generated method stub

	}

	@Override
	public void postMessage(Message message) {
		// TODO Auto-generated method stub

	}

	@Override
	public void postModule(Module module) {
		// TODO Auto-generated method stub

	}

	@Override
	public void postParameter(Parameter parameter) {
		// TODO Auto-generated method stub

	}

	@Override
	public void postService(Service service) {
		// TODO Auto-generated method stub

	}

	@Override
	public void postStruct(Struct struct) {
		// TODO Auto-generated method stub

	}

	@Override
	public void preConstant(Constant constant) {
		// TODO Auto-generated method stub

	}

	@Override
	public void preEnum(Enumx enumx) {
		// TODO Auto-generated method stub

	}

	@Override
	public void preExcept(Except except) {
		// TODO Auto-generated method stub

	}

	@Override
	public void preMessage(Message message) {
		// TODO Auto-generated method stub

	}

	@Override
	public void preModule(Module module) {
		// TODO Auto-generated method stub

	}

	@Override
	public void preParameter(Parameter parameter) {
		// TODO Auto-generated method stub

	}

	@Override
	public void preService(Service service) {
		// TODO Auto-generated method stub

	}

	@Override
	public void preStruct(Struct struct) {
		// TODO Auto-generated method stub

	}

	public List<String> getList() {
		return list;
	}

}
