package etch.examples.configuration;

import java.util.HashMap;
import java.util.Map;

import etch.bindings.java.util.StrStrHashMap;

/* $Id$
 *
 * Created by Badri Narayanan Champakesan on Aug 2, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

/**
 * A fake LogServer impl used for local authentication db.
 */
public class ConfigDb implements ConfigurationServer{
	
	int level;
	
	public Boolean isLoggedIn(){
		return true;
	}

	public void login( String name, String pwd ) throws Failure{// ignore
	}

	public void logout(){// ignore
	}
	
	final StrStrHashMap configPaths1 = new StrStrHashMap();	
	{ configPaths1.put("ConfigurationServer1", "HelloConfig"); }
	
	final StrStrHashMap configPaths2 = new StrStrHashMap();
	{ configPaths2.put("AuthenticationServer1", "HelloAuth"); }
	
	final StrStrHashMap configPaths3 = new StrStrHashMap();	
	{ configPaths3.put("LogServer1", "HelloLog"); }
	
	final Map<String, StrStrHashMap> configs = new HashMap<String, StrStrHashMap>();
	{
		configs.put("HelloConfig", configPaths1); 
		configs.put("HelloAuth", configPaths2);
		configs.put("HelloLog", configPaths3); 
	}
		
	/**
	 * @param path
	 * @return StrStrHashMap instance which is the corresponding path
	 */
	public StrStrHashMap getAllConfig( String path ){
		return configs.get( path );
	}

	public String getConfig(String path, String name) throws Failure{
		StrStrHashMap map = getAllConfig( path );
		if (map == null)
			throw new Failure( "no map for path "+path );
		return map.get( name );
	}
	
	/**
	 * @param path of the specified server
	 */
	public void newPath( String path ){
		configs.put( path, new StrStrHashMap() );
	}
	
	/**
	 * @param path
	 * @param name
	 * @param value
	 * @return String instance of the name & value
	 */
	public String putConfig( String path, String name, String value ){
		return getAllConfig( path ).put( name, value );
	}
}
