/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

/**
 * Some helpful methods used in conjuction with MessageDirection.
 */
public class MsgDirHelper
{
	/**
	 * @param mc
	 * @return the appropriate suffix for a message name given the
	 * message direction.
	 */
	public static String getSuffix( MessageDirection mc )
	{
		switch (mc)
		{
			case CLIENT: return "Client";
			case SERVER: return "Server";
			case BOTH: return "";
			default: throw new IllegalArgumentException( "unknown message direction "+mc );
		}
	}
}
