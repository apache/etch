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

import java.util.Date;

/**
 * Generate unique key
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class UniqueKeyGenerator
{
	private static final Object _SYNC_OBJ = new Object();
	
	private static UniqueKeyGenerator _INSTANCE = null;
	
	private long _lastValue;
	
	public static UniqueKeyGenerator getInstance()
	{
		if (_INSTANCE==null)
		{
			synchronized(_SYNC_OBJ)
			{
				if (_INSTANCE==null)
					_INSTANCE = new UniqueKeyGenerator();
			}
		}
		return _INSTANCE;
	}
	
	private UniqueKeyGenerator()
	{
		_lastValue = (new Date()).getTime();
	}
	
	/**
	 * Generate unique key
	 * 
	 * @param prefix
	 * @return
	 */
	public String generateKey(String prefix)
	{
		synchronized(_SYNC_OBJ)
		{
			long myVal = (new Date()).getTime();
			if (myVal<=_lastValue) {
				myVal = _lastValue + 1;
			}
			_lastValue = myVal;
			return (prefix==null ? ""+_lastValue : prefix+_lastValue);
		}
	}
}
