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

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.StubHelper;
import org.apache.etch.util.core.Who;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 * @param <T>
 */
public class ERStubHelper extends StubHelper<EtchRouterManager>
{

	@Override
	public void run( DeliveryService svc, EtchRouterManager obj, Who sender, Message msg )
		throws Exception
	{
		obj.handleStubHelperCall( svc, sender, msg );
	}

}
