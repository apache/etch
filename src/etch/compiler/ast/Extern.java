/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.Map;

import etch.bindings.java.msg.IdName;
import etch.compiler.Backend;
import etch.compiler.ParseException;

/**
 * An extern declaration from the etch idl.
 */
public class Extern extends ParamList<Service>
{
	/**
	 * @param intf
	 * @param name
	 * @param opts
	 */
	public Extern( Service intf, Name name, Map<String, Opt> opts )
	{
		super( intf, name, opts );

		vname = intf.parent().name().name.replace( '.', '_' )+'_'+intf.name()+'_'+name;
		fqname = intf.parent().name().name+'.'+intf.name().name+'.'+name.name;
		id = IdName.hash( fqname );
	}
	
	private final String vname;
	
	private final String fqname;
	
	private final Integer id;

	@Override
	public Service service()
	{
		return parent();
	}

	@Override
	public void check() throws ParseException
	{
		// nothing to do
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sextern %s\n", indent, name() );
	}

	@Override
	public String fqname()
	{
		return fqname;
	}

	@Override
	public String efqname( Backend helper )
	{
		String lang = helper.getLang();
		etch.compiler.opt.Extern opt = (etch.compiler.opt.Extern) getOpt( "Extern."+lang );
		if (opt == null)
			return null;
		return opt.xname.image;
	}

	@Override
	public Integer id()
	{
		return id;
	}

	@Override
	public String vname( Backend helper )
	{
		return helper.mtvname( vname );
	}
	
	public String sname( Backend helper )
	{
		String lang = helper.getLang();
		etch.compiler.opt.Extern opt = (etch.compiler.opt.Extern) getOpt( "Extern."+lang );
		if (opt == null)
			return null;
		return opt.serializer.image;
	}
	
	public boolean hasImport( Backend helper )
	{
		String lang = helper.getLang();
		etch.compiler.opt.Extern opt = (etch.compiler.opt.Extern) getOpt( "Extern."+lang );
		if (opt == null)
			return false;
		return opt.xnameImport.image.length() > 0;
	}
	
	public String getImport( Backend helper )
	{
		String lang = helper.getLang();
		etch.compiler.opt.Extern opt = (etch.compiler.opt.Extern) getOpt( "Extern."+lang );
		if (opt == null)
			return null;
		return opt.xnameImport.image;
	}
	
	public boolean hasSerializerImport( Backend helper )
	{
		String lang = helper.getLang();
		etch.compiler.opt.Extern opt = (etch.compiler.opt.Extern) getOpt( "Extern."+lang );
		if (opt == null)
			return false;
		return opt.serializerImport.image.length() > 0;
	}
	
	public String getSerializerImport( Backend helper )
	{
		String lang = helper.getLang();
		etch.compiler.opt.Extern opt = (etch.compiler.opt.Extern) getOpt( "Extern."+lang );
		if (opt == null)
			return null;
		return opt.serializerImport.image;
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.doExtern( this );
	}
}
