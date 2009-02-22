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
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;

import etch.bindings.java.msg.Type;

/**
 * Class
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class PluginInstallInfo
{
	private Properties _metaProperties = null;
	
	private String _name = null;
	
	private Map<String, Type> _methodMapByName = null;
	
	private Map<Integer, Type> _methodMapById = null;
	
	/**
	 * Constructor
	 * 
	 * @param name
	 */
	public PluginInstallInfo(String name, Properties metaProperties)
	{
		_metaProperties = metaProperties;
		_name = name;
		_methodMapByName = new HashMap<String, Type>();
		_methodMapById = new HashMap<Integer, Type>();
	}
	
	public void addMethod(Type method)
	{
		if (method==null) return;
		_methodMapByName.put( method.getName(), method );
		_methodMapById.put( method.getId(), method );
	}
	
	public Type getMethod(String name)
	{
		return _methodMapByName.get( name );
	}
	
	public Type getMethod(Integer id)
	{
		return _methodMapById.get( id );
	}
	
	public Collection<Type> getMethods()
	{
		return _methodMapById.values();
	}
	
	public String getName()
	{
		return _name;
	}
	
	public String getMetaProperty( String key )
	{
		return _metaProperties.getProperty( key );
	}
	
	public String getMetaProperty( String key, String defValue )
	{
		return _metaProperties.getProperty( key, defValue );
	}
	
	/**
	 * 
	 * @param prefix
	 * @return
	 */
	public String[] getSortedMetaPropertyKeysWithPrefix( String prefix )
	{
		Set<Object> keySet = _metaProperties.keySet();
		List<String> keyList = new ArrayList<String>();
		for (Object key : keySet)
		{
			if (key.toString().startsWith( prefix ))
				keyList.add( key.toString() );
		}
		String[] arr = new String[keyList.size()];
		arr = keyList.toArray( arr );
		Arrays.sort( arr );
		return arr;
	}
}
