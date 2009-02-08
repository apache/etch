/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.bindings.java.compiler;

/**
 * The version info of this Etch backend (compiler).
 */
public interface CompilerVersion
{
	// This file is edited by the production build system to replace the value
	// of VERSION below with whatever it wants the version string to actually be.
	
	/** The version of this Etch backend (compiler) */
	public String VERSION = "java apache-etch-1.0.2-incubating (LOCAL-0)";
}
