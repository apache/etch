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
package cisco.uc.cuae.etchrouter;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
import org.apache.etch.bindings.java.support.Validator_long;
import org.apache.etch.bindings.java.support.Validator_string;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class DynamicValueFactory extends DefaultValueFactory
{
	private static final Logger _LOGGER = Logger.getLogger( DynamicValueFactory.class.getName());
	
	private static final Object _SYNC_OBJ = new Object();
	
	private static TypeMap _types = new TypeMap();
	
	private static Class2TypeMap _class2type = new Class2TypeMap();
	
	static
	{
		DefaultValueFactory.init( _types, _class2type );
	}
		
	/**
	 * 
	 * @param uri
	 * @param types
	 * @param class2type
	 */
	public DynamicValueFactory(String uri)
	{
		super(uri, _types, _class2type);
	}
	
	/**
	 * Add collection of types to dynamic value factory
	 * 
	 * @param types
	 * @return list of types that were added successfully 
	 */
	public List<Type> addTypeDefinitions(Collection<Type> types)
	{
		List<Type> typesAdded = new ArrayList<Type>(types.size());
		synchronized(_SYNC_OBJ)
		{
			for (Type myType : types)
			{
				try
				{
					_types.add( myType );
					typesAdded.add( myType );
				}
				catch (Exception e)
				{
					_LOGGER.log( Level.FINER, "Failed to add type definition \"{0}\" to DynamicValueFactory: {1}", new Object[] { myType.getName(), e.getLocalizedMessage() } );
				}
			}
		}
		return typesAdded;
	}
	
	/**
	 * 
	 * @param clss
	 * @param myType
	 */
	public void addClass2TypeMap( Class clss, Type myType)
	{
		synchronized(_SYNC_OBJ)
		{
			_class2type.put( clss, myType );
		}
	}
	
}
