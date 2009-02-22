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

import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.Set;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class PluginAttributes extends AttributesBase
{	

	private String _pluginName = null;
	
	/**
	 * Constructor 
	 * 
	 * @param pluginName name of the plugin
	 * @param paramStr a URL query string, like "name1=value1&name2=value2". Note that each name field
	 *                 must not contain "." - the "." is used in ApplicationAttribute name as a delimiter
	 *                 between plugin name and plugin attribute name.
	 * @throws UnsupportedEncodingException
	 */
	public PluginAttributes( String pluginName, String paramStr ) throws UnsupportedEncodingException
	{
		this( pluginName, paramStr, null );
	}
	
	/**
	 * Constructor 
	 * 
	 * @param pluginName name of the plugin
	 * @param paramStr a URL query string, like "name1=value1&name2=value2". Note that each name field
	 *                 must not contain "."
	 * @param charset
	 * @throws UnsupportedEncodingException
	 */
	public PluginAttributes( String pluginName, String paramStr, String charset ) throws UnsupportedEncodingException
	{
		super( paramStr, charset );
		_pluginName = pluginName;
	}

	/**
	 * Check whether attributes specified with an application match the plugin attributes
	 * 
	 * It assumes that the plugin's each attribute name doesn't contain ".". Each 
	 * ApplicationAttributes' attribute name may be either like 
	 * "&lt;pluginName&gt;.&lt;pluginAattrNamePattern&gt;" or "&lt;pluginAttrNamePattern&gt;"
	 * 
	 * An ApplicationAttributes object matches this PluginAttributes only when all its attributes,
	 * whose name either starts with "&lt;thisPluginName&gt;." or doesn't contain ".", are equal to
	 * the attributes of this plugin attrs.
	 * 
	 * @param appAttrs the application's attributes object
	 * 
	 * @return true if matches 
	 */
	public boolean matches( ApplicationAttributes appAttrs )
	{
		if (appAttrs==null) return true;
		List<String> appKeys = appAttrs.getKeysForPlugin( _pluginName );
		int startIdx = _pluginName.length() + 1;
		String suffix, thisAttr, appAttr;
		for (String appKey : appKeys)
		{
			suffix = appKey.substring( startIdx );
			thisAttr = this.get( suffix );
			appAttr = appAttrs.get( appKey );
			if (thisAttr==null)
			{
				if (appAttr!=null) return false;
			}
			else if (!thisAttr.equals( appAttr ))
			{
				return false;
			}
		}
		appKeys = appAttrs.getGlobalKeys();
		for (String appKey : appKeys)
		{
			thisAttr = this.get( appKey );
			appAttr = appAttrs.get( appKey );
			if (thisAttr==null)
			{
				if (appAttr!=null) return false;
			}
			else if (!thisAttr.equals( appAttr ))
			{
				return false;
			}
		}
		return true;
	}
}
