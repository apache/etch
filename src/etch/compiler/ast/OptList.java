/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.Collection;

public interface OptList
{
	public Collection<Opt> opts();
	public boolean hasOpt( String name );
	public Opt getOpt( String name );
}
