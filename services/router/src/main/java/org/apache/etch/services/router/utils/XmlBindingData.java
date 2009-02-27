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
package org.apache.etch.services.router.utils;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;

import org.apache.etch.services.router.xml.Exception;
import org.apache.etch.services.router.xml.Module;


/**
 * Utility class to parse an Etch-compiled XML binding file and return
 * the parsed result in a data object
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class XmlBindingData
{
	private static JAXBContext _jaxbContext = null;

	/**
	 * Factory method to get the parsed result object from an etch XML binding file
	 * 
	 * @param xmlFile
	 * @return
	 * @throws JAXBException
	 * @throws IOException
	 */
	public static XmlBindingData loadXmlBindingFile(File xmlFile) throws JAXBException, IOException 
	{
		if (_jaxbContext==null)
			_jaxbContext = JAXBContext.newInstance( "org.apache.etch.services.router.xml" );
        Unmarshaller u = _jaxbContext.createUnmarshaller();
        Module mod = (Module)(u.unmarshal( new FileInputStream( xmlFile ) ));
        return new XmlBindingData(mod);
	}
	
	private List<Module.Service.Enums.Enum> _enums;
    protected List<Module.Service.Structs.Struct> _structs;
    protected List<org.apache.etch.services.router.xml.Exception> _exceptions;
    protected List<Module.Service.Methods.Method> _methods;
    protected List<Module.Service.Externs.Extern> _externs;
    
    /**
     * Getter
     * 
     * @return
     */
	public List<Module.Service.Enums.Enum> getEnums()
	{
		return _enums;
	}

	/**
	 * getter
	 * 
	 * @return
	 */
	public List<Module.Service.Structs.Struct> getStructs()
	{
		return _structs;
	}

	/**
	 * getter
	 * 
	 * @return
	 */
	public List<org.apache.etch.services.router.xml.Exception> getExceptions()
	{
		return _exceptions;
	}

	/**
	 * getter
	 * 
	 * @return
	 */
	public List<Module.Service.Methods.Method> getMethods()
	{
		return _methods;
	}
	
	public List<Module.Service.Externs.Extern> getExterns()
	{
		return _externs;
	}

	private XmlBindingData(Module mod)
	{
		_enums = new ArrayList<Module.Service.Enums.Enum>();
		_structs = new ArrayList<Module.Service.Structs.Struct>();
		_exceptions = new ArrayList<org.apache.etch.services.router.xml.Exception>();
		_methods = new ArrayList<Module.Service.Methods.Method>();
		_externs = new ArrayList<Module.Service.Externs.Extern>();
		parseModule(mod);
	}
	
	private void parseModule(Module mod)
	{
		List<Module.Service> svcList = mod.getService();
		if (svcList==null) return;
		for (Module.Service svc : svcList)
		{
			parseService(svc);
		}
	}
	
	private void parseService(Module.Service svc)
	{
		List<Module.Service.Enums> enums = svc.getEnums();
		if (enums!=null)
		{
			for (Module.Service.Enums myenum : enums)
			{
				List<Module.Service.Enums.Enum> enumL = myenum.getEnum();
				if (enumL!=null) _enums.addAll( enumL );
			}
		}
		List<Module.Service.Structs> structs = svc.getStructs();
		if (structs!=null)
		{
			for (Module.Service.Structs mystructs : structs)
			{
				List<Module.Service.Structs.Struct> structL = mystructs.getStruct();
				if (structL!=null) _structs.addAll( structL );
			}
		}
		List<Module.Service.Exceptions> exps = svc.getExceptions();
		if (exps!=null)
		{
			for (Module.Service.Exceptions myexps : exps)
			{
				List<Exception> expL = myexps.getException();
				if (expL!=null) _exceptions.addAll( expL );
			}
		}
		List<Module.Service.Methods> methods = svc.getMethods();
		if (methods!=null)
		{
			for (Module.Service.Methods mymethods : methods)
			{
				List<Module.Service.Methods.Method> methodL = mymethods.getMethod();
				if (methodL!=null) _methods.addAll( methodL );
			}
		}
		List<Module.Service.Externs> externs = svc.getExterns();
		if (externs!=null)
		{
			for (Module.Service.Externs anExterns : externs)
			{
				List<Module.Service.Externs.Extern> externL = anExterns.getExtern();
				if (externL!=null) _externs.addAll( externL );
			}
		}
	}
}
