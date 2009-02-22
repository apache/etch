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
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Set;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public abstract class AttributesBase extends LinkedHashMap<String, String>
{
	public static final String _DEFAULT_CHAR_SET = "ISO-8859-1";
	
	protected AttributesBase( String paramStr, String charset ) throws UnsupportedEncodingException
	{
		if ( paramStr==null ) return;
		if (charset==null || charset.trim().length()==0) charset = _DEFAULT_CHAR_SET;
		String[] toks = paramStr.split( "&" );
		int idx;
		String name, value;
		for (String tok : toks)
		{
			idx = tok.indexOf( '=' );
			if (idx<0)
			{
				continue;
			}
			name = tok.substring( 0, idx );
			value = tok.substring( idx+1 );
			name = URLDecoder.decode( name, charset );
			value = URLDecoder.decode( value, charset );
			put( name, value );
		}	
	}
	
	protected List<String> getKeysWithPrefix( String prefix )
	{
		List<String> result = new ArrayList<String>();
		Set<String> keys = this.keySet();
		for (String key : keys)
		{
			if (key.startsWith( prefix )) result.add( key );
		}
		return result;
	}
	

	/**
	 * 
	 * @return
	 */
	public String getEncodedString()
	{
		return getEncodedString( null );
	}
	
	/**
	 * 
	 * @param charset
	 * @return
	 */
	public String getEncodedString( String charset )
	{
		if (charset==null || charset.trim().length()==0) charset = _DEFAULT_CHAR_SET;
		Set<String> keys = this.keySet();
		StringBuilder sb = new StringBuilder();
		int count = 0;
		String value, encodedStr;
		for (String key : keys)
		{
			if (count>0) sb.append( "&" );
			value = this.get( key );
			try
			{
				encodedStr = URLEncoder.encode( key, charset );
			}
			catch (Exception e)
			{
				encodedStr = key;
			}
			sb.append( encodedStr );
			try
			{
				encodedStr = value==null ? "" : URLEncoder.encode( value, charset );
			}
			catch (Exception e)
			{
				encodedStr = value;
			}
			sb.append( "=" ).append( encodedStr );
			count++;
		}
		return sb.toString();
	}

}
