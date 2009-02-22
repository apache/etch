/* $Id$
 *
 * Copyright 2009-2010 Cisco Systems Inc.
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
package cisco.uc.cuae.etchrouter.utils;

import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.Type;

public abstract class LocalTypeImportExportHelper implements ImportExportHelper
{

	protected Type _type = null;
	
	protected Class _typeClass = null;
	
	public LocalTypeImportExportHelper(Type myType) throws Exception
	{
		_type = myType;
		String typeName = myType.getName();
		int idx = typeName.lastIndexOf( '.' );
		if (idx <= 0) throw new Exception(String.format( "Invalid Etch sub-type name '%s'", typeName ));
		String intfName = typeName.substring( 0, idx );
		String className = intfName + "$" + typeName.substring( idx+1 );
		_typeClass = this.getClass().forName( className );
	}

	public Class getTypeClass()
	{
		return _typeClass;
	}
}
