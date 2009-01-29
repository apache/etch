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

package org.apache.etch.compiler.ast;

import java.util.Collection;

/**
 * A list of options (annotations, e.g., @Timeout, @Oneway)
 */
public interface OptList
{
	/**
	 * @return the collected options.
	 */
	public Collection<Opt> opts();
	
	/**
	 * @param name
	 * @return true if the item has the specified option.
	 */
	public boolean hasOpt( String name );
	
	/**
	 * @param name
	 * @return the specified option.
	 */
	public Opt getOpt( String name );
}
