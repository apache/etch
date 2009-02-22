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

import java.io.File;
import java.io.FileInputStream;
import java.util.Properties;
import java.util.logging.LogManager;

/**
 * Main execution class for the Etch Router service
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class EtchRouterMain
{
	
	private File _routerPropFile = null;
	
	private File _loggerConfFile = null;
	
	public EtchRouterMain( File routerPropFile, File loggerConfFile)
	{
		_routerPropFile = routerPropFile;
		_loggerConfFile = loggerConfFile;
	}
	
	/**
	 * main method
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args) throws Exception
	{
		File routerPropFile = (args!=null && args.length>0 ? new File(args[0]) : new File("EtchRouter.properties"));
		File loggerConfFile = (args!=null && args.length>1 ? new File(args[1]) : null);
		EtchRouterMain mainClass = new EtchRouterMain(routerPropFile, loggerConfFile);
		mainClass.run();
	}
	
	private EtchRouterManager _routerMgr = null;
	
	/**
	 * 
	 * @throws Exception
	 */
	public void run() throws Exception
	{
		if (_loggerConfFile!=null && _loggerConfFile.exists())
		{
			System.out.println(String.format( "Loading logger config file %s", _loggerConfFile.getAbsolutePath()));
			try
			{
				LogManager.getLogManager().readConfiguration(new FileInputStream(_loggerConfFile));
			}
			catch (Exception e)
			{
				e.printStackTrace(System.err);
			}
		}
		Properties prop = loadPropertiesFile();
		_routerMgr = new EtchRouterManager(prop);
		_routerMgr.start();
	}
	
	public EtchRouterManager getRouterManager()
	{
		return _routerMgr;
	}
	
	private Properties loadPropertiesFile()
	{
		Properties prop = new Properties();
		if (_routerPropFile!=null && _routerPropFile.exists())
		{
			System.out.println( String.format("Loading properties file %s", _routerPropFile.getAbsolutePath()) );
			try
			{
				prop.load( new FileInputStream(_routerPropFile) );
			}
			catch (Exception e)
			{
				e.printStackTrace( System.err );
			}
		}
		return prop;
	}

	
}
