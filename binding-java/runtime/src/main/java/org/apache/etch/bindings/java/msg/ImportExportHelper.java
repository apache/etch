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
 * Interface used to implement ValueFactory based import / export code
 * for service defined types.
 */
public interface ImportExportHelper
{
	/**
	 * Exports a value by creating an equivalent StructValue.
	 * @param vf the value factory for this connection.
	 * @param value a service defined type to export.
	 * @return a StructValue representing the exported value.
	 */
	public StructValue exportValue( ValueFactory vf, Object value );

	/**
	 * Imports a value from an equivalent StructValue.
	 * @param struct a StructValue representing the exported value.
	 * @return a service defined type.
	 */
	public Object importValue( StructValue struct );
}
