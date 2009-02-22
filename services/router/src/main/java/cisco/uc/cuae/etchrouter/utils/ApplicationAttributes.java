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
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class ApplicationAttributes extends AttributesBase
{
	/**
	 * Constructor
	 * 
	 * @param paramStr a URL query string, like "name1=value1&name2=value2". Each name field
	 *                 may have pattern like either "&lt;pluginName&gt;.&lt;attrName&gt;"
	 *                 or "&lt;attrName&gt;", where "attrName" doesn't contain ".". When matching
	 *                 a PluginAttributes object, all the attributes whose name starts with the plugin's
	 *                 name + "." or doesn't contain "." will be taken to match the corresponding attribute
	 *                 with the plugin.
	 * @throws UnsupportedEncodingException
	 */
	public ApplicationAttributes( String paramStr ) throws UnsupportedEncodingException
	{
		this( paramStr, null );
	}
	
	/**
	 * 
	 * @param paramStr a URL query string, like "name1=value1&name2=value2". Each name field
	 *                 may have pattern like either "&lt;pluginName&gt;.&lt;attrName&gt;"
	 *                 or "&lt;attrName&gt;", where "attrName" doesn't contain ".". When matching
	 *                 a PluginAttributes object, all the attributes whose name starts with the plugin's
	 *                 name + "." or doesn't contain "." will be taken to match the corresponding attribute
	 *                 with the plugin.
	 * @param charset
	 * @throws UnsupportedEncodingException
	 */
	public ApplicationAttributes( String paramStr, String charset ) throws UnsupportedEncodingException
	{
		super( paramStr, charset );
	}

	public List<String> getKeysForPlugin( String pluginName )
	{
		return getKeysWithPrefix( String.format( "%s.", pluginName ));
	}
	
	public List<String> getGlobalKeys()
	{
		List<String> keyList = new ArrayList<String>();
		Set<String> keys = this.keySet();
		for (String key : keys)
		{
			if (key.indexOf( '.' ) < 0)
				keyList.add( key );
		}
		return keyList;
	}
}
