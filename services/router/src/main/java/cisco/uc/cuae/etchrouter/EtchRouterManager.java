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
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

import cisco.uc.cuae.etchrouter.ConnectionStackInfo.ConnectionType;
import cisco.uc.cuae.etchrouter.EtchRouter.EtchRouterException;
import cisco.uc.cuae.etchrouter.EtchRouter.PluginServiceState;
import cisco.uc.cuae.etchrouter.plugin.PluginGroup;
import cisco.uc.cuae.etchrouter.plugin.PluginMemberConnection;
import cisco.uc.cuae.etchrouter.plugin.PluginStateMonitor;
import cisco.uc.cuae.etchrouter.utils.LocalTypeImportExportHelper;
import cisco.uc.cuae.etchrouter.utils.StructValueImportExportHelper;
import cisco.uc.cuae.etchrouter.utils.XmlBindingData;
import cisco.uc.cuae.etchrouter.utils.XmlBindingDataParser;
import org.apache.etch.bindings.java.msg.Direction;
import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;

/**
 * Main class for the Etch Router service
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class EtchRouterManager
{
	/**
	 * Key in the resources map for this EtchRouterManager
	 */
	public static final String _ETCH_ROUTER_MANAGER = "EtchRouterManager";
	
	private static final Logger _LOGGER = Logger.getLogger( EtchRouterManager.class.getName());
	
	private Properties _properties = null;
	
	private File _homeDir = null;
	
	private File _appsRoot = null;
	
	private File _pluginsRoot = null;
	
	private String _url = null;
	
	private final Object _syncObj = new Object();
	
	private DynamicValueFactory _valueFactory = null;
	
	private Map<String, PluginGroup> _pluginMap = null;
	
	private Map<Type, PluginGroup> _method2PluginMap = null;
	
	private Map<String, ApplicationInstallInfo> _appMap = null;
	
	// all methods defined by the EtchRouter's own XML binding file:
	private Map<Type, Boolean> _localMethodsMap = null;
	
	private Map<EtchRouterMessageFilter, ConnectionStackInfo> _connectionInfoMapByFilter = null;
	
	private Map<DeliveryService, ConnectionStackInfo> _connInfoMapByDs = null;
	
	private ERStubHelper _stubHelper = new ERStubHelper();
	
	private Map<ConnectionStackInfo, ApplicationConnectionInfo> _appConnectionInfoMap = null;
	
	private Map<ConnectionStackInfo, ApplicationConnectionInfo> _pluginServerConn2AppConnMap = null;
	
	private static final String _REGISTER_ROUTER_APP_METHOD_NAME = "registerRouterApplication";
	
	private static final String _ROUTER_PLUGIN_SERVICE_STATE_CHANGE_METHOD_NAME = "routerPluginServiceStateChange";
	
	private Type _registerApplicationMethodType = null;
	
	private Field _mf_applicationName = new Field("applicationName");
	private Field _mf_applicationParam = new Field("applicationParam");
	
	private Type _pluginServiceStateChangeMethodType = null;
	private Field _mf_pluginGroupName = new Field("pluginGroupName");
	private Field _mf_state = new Field("state");
	private Field _mf_detail = new Field("detail");
	
	//number of miliseconds
	private int _connStartMaxDelay = 4000;
	
	// number of miliseconds
	private int _connStopMaxDelay = 4000;
	
	/**
	 * Constructor
	 * 
	 * @param properties
	 * @throws Exception
	 */
	public EtchRouterManager(Properties properties) throws Exception
	{
		_properties = properties;
		initProperties();
		
		_pluginMap = Collections.synchronizedMap( new HashMap<String, PluginGroup>() );
		_method2PluginMap = Collections.synchronizedMap( new HashMap<Type, PluginGroup>() );
		_appMap = Collections.synchronizedMap( new HashMap<String, ApplicationInstallInfo>() );
		_valueFactory = new DynamicValueFactory(_url);
		_localMethodsMap = new HashMap<Type, Boolean>();
		_connectionInfoMapByFilter = Collections.synchronizedMap( new HashMap<EtchRouterMessageFilter, ConnectionStackInfo>() );
		_connInfoMapByDs = Collections.synchronizedMap( new HashMap<DeliveryService, ConnectionStackInfo>() );
		_appConnectionInfoMap = Collections.synchronizedMap( new HashMap<ConnectionStackInfo, ApplicationConnectionInfo>() );
		_pluginServerConn2AppConnMap = Collections.synchronizedMap( new HashMap<ConnectionStackInfo, ApplicationConnectionInfo>() );
	}
	
	private void initProperties() throws Exception
	{
		_url = _properties.getProperty( "listener.url" );
		_homeDir = new File(_properties.getProperty( "etch.router.home", "." ));
		if (_url==null)
			throw new Exception("The property 'listener.url' is not specified in properties");
		if (!(_homeDir.exists() && _homeDir.isDirectory()))
			throw new Exception(String.format( "Invalid home directory: %s", _homeDir.getPath() ));
		String appsRoot = _properties.getProperty( "applications.root.dir" );
		_appsRoot = appsRoot==null ? new File(_homeDir, "applications") : new File(appsRoot);
		if ((!_appsRoot.exists()) || (!_appsRoot.isDirectory()))
			throw new Exception(String.format( "The applications root directory does not exist: %s", _appsRoot.getAbsolutePath()));
		String pluginsRoot = _properties.getProperty( "plugins.root.dir" );
		_pluginsRoot = pluginsRoot==null ? new File(_homeDir, "plugins") : new File(pluginsRoot);
		if ((!_pluginsRoot.exists()) || (!_pluginsRoot.isDirectory()))
			throw new Exception(String.format( "The plugins root directory does not exist: %s", _pluginsRoot.getAbsolutePath()));
	
		try
		{
			_connStartMaxDelay = Integer.parseInt( _properties.getProperty( "connection.start.max.delay" ) );
		}
		catch (Exception e)
		{
			
		}
		try
		{
			_connStopMaxDelay = Integer.parseInt( _properties.getProperty( "connection.stop.max.delay" ) );
		}
		catch (Exception e)
		{
			
		}
	}
	
	public String getProperty(String key, String defaultValue)
	{
		return _properties.getProperty( key, defaultValue );
	}
	
	/**
	 * Max delay time when calling _startAndWaitUp
	 * 
	 * @return
	 */
	public int getConnectionStartMaxDelay()
	{
		return _connStartMaxDelay;
	}
	
	/**
	 * Max delay time when calling _stopAndWaitDown
	 * 
	 * @return
	 */
	public int getConnectionStopMaxDelay()
	{
		return _connStopMaxDelay;
	}
	
	/**
	 * Call back method
	 * 
	 * @param svc
	 * @param sender
	 * @param msg
	 */
	public void handleStubHelperCall(DeliveryService svc, Who sender, Message msg)
	{
		_LOGGER.log( Level.FINEST, "Method called, msg's type name is \"{0}\"", msg.type().getName());
	}
	
	/**
	 * 
	 * @param event
	 * @param connStackInfo
	 * @throws Exception
	 */
	public void sessionNotify(Object event, ConnectionStackInfo connStackInfo) throws Exception
	{
		if (event != Session.DOWN) return;
		_LOGGER.log( Level.INFO, "Method starts, event is: {0}, connection is: {1}",
			new Object[] { event, connStackInfo } );
    	ConnectionType connType = connStackInfo.getConnectionType();
    	if (connType==null)
    	{
    		removeConnectionStackInfo( connStackInfo );
    	}
    	else if (connType==ConnectionType.APP_CLIENT_CONN)
    	{
   			ApplicationConnectionInfo appConnInfo = this.getAppConnectionInfoByAppClientStackInfo( connStackInfo );
   			if (appConnInfo!=null)
   			{
   				appConnInfo.stopAllPluginConnections();
   				_appConnectionInfoMap.remove( connStackInfo );
   	   			Collection<PluginGroup> pluginGrps = appConnInfo.getReferencedPluginGroups();
       			for (PluginGroup pluginGrp : pluginGrps)
       			{
       				pluginGrp.removeApplicationConnection( appConnInfo );
       			}
   			}
   			removeConnectionStackInfo( connStackInfo );
   		}
    	else if (connType==ConnectionType.PLUGIN_MONITOR_CONN)
    	{
   			PluginGroup pluginGroup = this.getPluginGroup( connStackInfo.getPluginOrAppName() );
   			pluginGroup.onPluginMemberConnectionDown( connStackInfo );
   			removeConnectionStackInfo( connStackInfo );
     	}
    	else if (connType==ConnectionType.PLUGIN_SERVER_CONN)
    	{
   			ApplicationConnectionInfo appConnInfo = this.getAppConnectionInfoByPluginServerStackInfo( connStackInfo );
   			if (appConnInfo!=null)
   			{
   				PluginGroup pluginGrp = appConnInfo.getPluginGroupByName( connStackInfo.getPluginOrAppName() );
   				PluginMemberConnection instConn = appConnInfo.getPluginConnectionByPluginGroup( pluginGrp );
   				//remove the corresponding plugin instance from available list before re-init:
   				pluginGrp.onPluginMemberConnectionDown( instConn.getMember().getConnInfo() );
   				appConnInfo.stopPluginConnection( pluginGrp, true );
   				appConnInfo.initPluginConnection( pluginGrp );
   			}
   			removeConnectionStackInfo( connStackInfo );
     	}
	}
	
	
	/**
	 * This is called by ERMessagizer.sessionPacket
	 * 
	 * @param sender
	 * @param connStackInfo
	 * @param msg
	 */
	public void handleLocalMethodMessage( Who sender, ConnectionStackInfo connStackInfo, Message msg)
	{
		ConnectionType connType = connStackInfo.getConnectionType();
		Type methodType = msg.type();
		Message rmsg = (methodType.getResult()==null) ? null : msg.reply();
		Object result = null;
		try
		{
			if (_registerApplicationMethodType.equals( methodType ))
			{
				if (connType==null)
				{
					result = registerApplication( sender, connStackInfo, msg );
				}
				else
					throw new EtchRouterException(2, String.format( "Cannot register application because the connection was already established as type %s", connType.name() ));
			}
			else 
			{
				result = handleOtherLocalMessage( sender, connStackInfo, msg );
			}
		}
		catch (Throwable e)
		{
			if (e instanceof EtchRouterException) result = e;
			else if ( e instanceof RuntimeException ) result = e;
			else result = new RuntimeException(e);
		}
		if (rmsg!=null)
		{
			if (result!=null) rmsg.put( DynamicValueFactory._mf_result, result);
			try
			{
				connStackInfo.getDeliveryService().transportMessage( sender, rmsg );
			}
			catch (Exception ee)
			{
				_LOGGER.log( Level.SEVERE, "Got Exception: ", ee);
			}
		}
		if (result instanceof Throwable)
			_LOGGER.log( Level.SEVERE, "Got Exception: ", (Throwable)result);
			
	}
	
	private Object handleOtherLocalMessage( Who sender, ConnectionStackInfo connStackInfo, Message msg ) throws Exception
	{
		throw new EtchRouterException(1, String.format( "Etch-router API is not implemented: %s", msg.type().getName() ));
	}
	
	private String registerApplication( Who sender, ConnectionStackInfo connStackInfo, Message msg ) throws Exception
	{
		String applicationName = (String)msg.get( _mf_applicationName );
		String applicationParam = (String)msg.get( _mf_applicationParam );
		return registerApplication(applicationName, applicationParam, connStackInfo);
	}
	
	/**
	 * 
	 * @param connStackInfo
	 * @param pluginMethod
	 * @throws Exception
	 */
	public void registerAnonymousApplication( ConnectionStackInfo connStackInfo, Type pluginMethod ) throws Exception
	{
		synchronized (_syncObj)
		{
			ApplicationConnectionInfo connInfo = _appConnectionInfoMap.get( connStackInfo );
			if (connInfo==null)
			{
				connStackInfo.setConnectionType( ConnectionType.APP_CLIENT_CONN, null );
				connInfo = new ApplicationConnectionInfo( connStackInfo, this, pluginMethod );
				_appConnectionInfoMap.put( connStackInfo, connInfo );
			}
		}
	}
	
	/**
	 * 
	 * @param applicationName
	 * @param appConnInfo
	 * @throws Exception
	 */
	private String registerApplication( String applicationName, String applicationParam, ConnectionStackInfo appConnInfo )
		throws Exception
	{
		//allow lazy loading of the application install directory
		ApplicationInstallInfo appInstallInfo = null;
		try
		{
			loadApplicationInstallInfo(applicationName);
		}
		catch (Exception ee)
		{
			//_LOGGER.log( Level.FINE, "Registering anonymous app connection as application \"{0}\": {1}", new Object[]{ applicationName, appConnInfo} );
		}
		synchronized (_syncObj)
		{
			ApplicationConnectionInfo connInfo = _appConnectionInfoMap.get( appConnInfo );
			if (connInfo==null)
			{
				appConnInfo.setConnectionType( ConnectionType.APP_CLIENT_CONN, appInstallInfo==null ? null : applicationName );
				connInfo = new ApplicationConnectionInfo( applicationParam, appInstallInfo, appConnInfo, this);
				_appConnectionInfoMap.put( appConnInfo, connInfo );
			}
			else
			{
				if (connInfo.isAnonymous())
				{
					_LOGGER.log( Level.FINE, "Registering anonymous app connection as application \"{0}\": {1}", new Object[]{ applicationName, appConnInfo} );
					connInfo.setApplicationName( applicationName );
					connInfo.setApplicationParam( applicationParam );
				}
				else
				{
					throw new EtchRouterException(1, "The application client is already registered with Etch Router manager");
				}
			}
		}
		return "OK";
	}
	
	/**
	 * 
	 * @param pluginGroupName
	 * @param state
	 * @param appClientConn
	 * @throws Exception
	 */
	public void callAppClientMethod_pluginServiceStateChange( String pluginGroupName, PluginServiceState state, String detail, ConnectionStackInfo appClientConn ) throws Exception
	{
		Message msg = new Message(_pluginServiceStateChangeMethodType, _valueFactory);
		msg.put( _mf_pluginGroupName, pluginGroupName );
		msg.put( _mf_state, state );
		msg.put( _mf_detail, detail );
		callClientMethod( msg, appClientConn );
	}
	
	/**
	 * 
	 * @param msgDetail
	 * @param connStack
	 * @throws Exception
	 */
	public void sentNotifyMsgToConnection( String msgDetail, ConnectionStackInfo connStack ) throws Exception
	{
		Message msg = new Message(_valueFactory.get_mt__exception(), _valueFactory);
		msg.put( _valueFactory._mf_result, new RuntimeException(msgDetail) );
		callClientMethod( msg, connStack );
	}
	
	private Object callClientMethod( Message msg, ConnectionStackInfo connInfo ) throws Exception
	{
		Type methodType = msg.type();
		if (methodType.getDirection()==Direction.SERVER)
		{
			throw new EtchRouterException(1, String.format( "Cannot send method '%s' to an EtchRouter client.", methodType.getName() ));
		}
		DeliveryService ds = connInfo.getDeliveryService();
		Type resultType = methodType.getResult();
		if (resultType==null)
		{
			//is oneWay:
			try
			{
				ds.transportMessage( null, msg );
				return null;
			}
			catch (Exception e)
			{
				throw new RuntimeException(String.format( "Unknown error while calling oneway client method '%s'", methodType.getName() ), e);
			}
		}
		try
		{
			Mailbox mbox = ds.begincall( msg );
			return ds.endcall( mbox, resultType );
		}
		catch (Exception e)
		{
			throw new RuntimeException(String.format( "Unknown error while calling twoway client method '%s'", methodType.getName() ), e);
		}
	}
	
	
	/**
	 *
	 * @param info
	 */
	public void addConnectionStackInfo(ConnectionStackInfo info)
	{
		_connectionInfoMapByFilter.put( info.getMessageFilter(), info );
		_connInfoMapByDs.put( info.getDeliveryService(), info );
	}
	
	public ConnectionStackInfo getConnectionStackInfo( EtchRouterMessageFilter filter)
	{
		return _connectionInfoMapByFilter.get( filter );
	}
	
	public ConnectionStackInfo getConnectionStackInfo( DeliveryService ds)
	{
		return _connInfoMapByDs.get( ds );
	}
	
	public void removeConnectionStackInfo( ConnectionStackInfo info )
	{
		_connectionInfoMapByFilter.remove( info.getMessageFilter() );
		_connInfoMapByDs.remove( info.getDeliveryService() );
	}
	
	public ApplicationInstallInfo getApplicationInstallInfo(String appName)
	{
		return _appMap.get( appName );
	}
	
	public Collection<PluginGroup> getPluginGroups()
	{
		return _pluginMap.values();
	}
	
	public PluginGroup getPluginGroup(String pluginGroupName)
	{
		return _pluginMap.get( pluginGroupName );
	}
	
	public PluginGroup getPluginGroup( Type method)
	{
		return _method2PluginMap.get(method);
	}
	
	public ApplicationConnectionInfo getAppConnectionInfoByAppClientStackInfo( ConnectionStackInfo appStackInfo )
	{
		return _appConnectionInfoMap.get( appStackInfo );
	}
	
	public ApplicationConnectionInfo getAppConnectionInfoByPluginServerStackInfo( ConnectionStackInfo pluginServerStackInfo )
	{
		return _pluginServerConn2AppConnMap.get( pluginServerStackInfo );
	}
	
	public void addAppConnectionInfo( ConnectionStackInfo pluginServerStackInfo, ApplicationConnectionInfo appConnInfo )
	{
		_pluginServerConn2AppConnMap.put( pluginServerStackInfo, appConnInfo );
	}
	
	public void removeAppConnectionInfo( ConnectionStackInfo pluginServerStackInfo )
	{
		_pluginServerConn2AppConnMap.remove( pluginServerStackInfo );
	}
	
	public DynamicValueFactory getValueFactory()
	{
		return _valueFactory;
	}
	
	public boolean isLocalMethodType( Type aType )
	{
		return _localMethodsMap.get( aType )!=null;
	}
	
	private ServerFactory _listener = null;
	
	private PluginStateMonitor _pluginMon = null;
	
	/**
	 * 
	 * @throws Exception
	 */
	public void start() throws Exception
	{
		_LOGGER.log( Level.INFO, "Etch Router Manager starting up..." );
		loadEtchRouterXmlBinding();
		loadInstalledPlugins();
		loadInstalledApplications();
		_listener = EtchRouterManagerHelper.newListener( this, _url, null );
		_listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		_pluginMon = new PluginStateMonitor( this );
		Thread monThread = new Thread( _pluginMon );
		_LOGGER.log( Level.INFO, "Starting plugin state monitor thread..." );
		monThread.start();
		_LOGGER.log( Level.INFO, "Etch Router Manager running..." );
	}

	/**
	 * 
	 * @throws Exception
	 */
	public void stop() throws Exception
	{
		_LOGGER.log( Level.INFO, "Stopping Etch Router Manager..." );
		_pluginMon.stop();
		_listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
	}
	
	private void loadEtchRouterXmlBinding() throws Exception
	{
		File xmlFile = new File(_homeDir, "EtchRouter.xml");
		_LOGGER.log( Level.FINE, "Loading EtchRouter XML binding file: {0}", xmlFile.getAbsolutePath() );
		List<Type> loadedEnums = new ArrayList<Type>();
		List<Type> loadedStructs = new ArrayList<Type>();
		List<Type> loadedExceptions = new ArrayList<Type>();
		List<Type> loadedMethods = new ArrayList<Type>();
		XmlBindingData data = XmlBindingData.loadXmlBindingFile( xmlFile );
		if (data.getExterns().size()>0)
			throw new Exception("No \"extern\" statement is allowed in EctchRouter IDL!");
		XmlBindingDataParser parser = new XmlBindingDataParser( data );
		Collection<Type> myTypes = parser.getEnumTypes();
		initImpExpHelpers( myTypes, parser );
		loadedEnums.addAll( _valueFactory.addTypeDefinitions( myTypes ) );
		myTypes = parser.getExceptionTypes();
		initImpExpHelpers( myTypes, parser );
		loadedExceptions.addAll( _valueFactory.addTypeDefinitions( myTypes ) );
		myTypes = parser.getStructTypes();
		initImpExpHelpers( myTypes, parser );
		loadedStructs.addAll( _valueFactory.addTypeDefinitions( myTypes ) );
		myTypes = parser.getMethodTypes();
		loadedMethods.addAll( _valueFactory.addTypeDefinitions( myTypes )  );
		//only primitive types are supported in the EtchRouter APIs:
		if (loadedMethods.isEmpty())
		{
			_LOGGER.log( Level.WARNING, "No method is loaded to the dynamic value factory from the EtchRouter XML binding file: {0}", xmlFile.getAbsolutePath() );
		}
		for (Type methodType : loadedMethods)
		{	
			methodType.setStubHelper( _stubHelper );
			_localMethodsMap.put( methodType, true );
			if (_REGISTER_ROUTER_APP_METHOD_NAME.equals( methodType.getName() ) || methodType.getName().endsWith( "."+_REGISTER_ROUTER_APP_METHOD_NAME ))
				_registerApplicationMethodType = methodType;
			else if (_ROUTER_PLUGIN_SERVICE_STATE_CHANGE_METHOD_NAME.equals( methodType.getName() ) || methodType.getName().endsWith( "."+ _ROUTER_PLUGIN_SERVICE_STATE_CHANGE_METHOD_NAME ))
				_pluginServiceStateChangeMethodType = methodType;
		}
		if (_registerApplicationMethodType==null)
			throw new Exception(String.format( "Cannot find \"%s\" method definition in EtchRouter IDL", _REGISTER_ROUTER_APP_METHOD_NAME) );
		if (_pluginServiceStateChangeMethodType==null)
			throw new Exception( String.format( "Cannot find \"%s\" method definition in EtchRouter IDL", _ROUTER_PLUGIN_SERVICE_STATE_CHANGE_METHOD_NAME ) );
	}

	private void initImpExpHelpers(Collection<Type> types, XmlBindingDataParser parser)
	{
		for (Type myType : types)
		{
			LocalTypeImportExportHelper helper = parser.getLocalImportExportHelper( myType.getName() );
			if (helper != null)
			{
				_valueFactory.addClass2TypeMap( helper.getTypeClass(), myType );
				myType.setComponentType( helper.getTypeClass() );
				myType.setImportExportHelper( helper );
			}
			else
			{
				StructValueImportExportHelper structHelper = parser.getStructValueImportExportHelper( myType.getName() );
				if (structHelper!=null) 
				{
					myType.setComponentType( StructValue.class );
					myType.setImportExportHelper( structHelper );
				}
			}
		}
	}
	
	/**
	 * 
	 * @throws Exception
	 */
	public void loadInstalledPlugins() throws Exception
	{
		String preloadPlugins = _properties.getProperty( "preload.plugins", "true" );
		if (!"true".equalsIgnoreCase( preloadPlugins ))
		{
			_LOGGER.log( Level.FINE, "Plugin install info will be lazily loaded with plugin registration" );
			return;
		}
		_LOGGER.log( Level.FINE, "Pre-loading all Plugins' install info from directory \"{0}\"...", _pluginsRoot.getAbsolutePath() );
		File[] subFiles = _pluginsRoot.listFiles();
		for (File subFile : subFiles)
		{
			if (subFile.isDirectory())
				loadPluginInstallInfo(null, subFile);
				
		}
	}
	
	/**
	 * 
	 * @throws Exception
	 */
	public void loadInstalledApplications() throws Exception
	{
		String preloadApps = _properties.getProperty( "preload.applications", "true" );
		if (!"true".equalsIgnoreCase( preloadApps ))
		{
			_LOGGER.log( Level.FINE, "Application install info will be lazily loaded with application registration" );
			return;
		}
		_LOGGER.log( Level.FINE, "Pre-loading all Applications' install info from directory \"{0}\"...", _appsRoot.getAbsolutePath() );
		File[] subFiles = _appsRoot.listFiles();
		for (File subFile : subFiles)
		{
			if (subFile.isDirectory())
				loadApplicationInstallInfo(null, subFile);
				
		}
	}
	
	/**
	 * 
	 * @param pluginName
	 * @return
	 * @throws Exception
	 */
	public PluginGroup loadPluginInstallInfo(String pluginName) throws Exception
	{
		PluginGroup pluginGrp = getPluginGroup(pluginName);
		if (pluginGrp!=null)
		{
			_LOGGER.log( Level.FINER, "The plugin group info for \"{0}\" is already loaded to Etch Router manager", pluginName );
			return pluginGrp;
		}
		File pluginDir = new File(_pluginsRoot, pluginName);
		if ((!pluginDir.exists()) || (!pluginDir.isDirectory()))
			throw new Exception(String.format( "The plugin directory does not exist: %s", pluginDir.getAbsolutePath()));
		loadPluginInstallInfo(pluginName, pluginDir);
		return getPluginGroup(pluginName);
	}
	
	private void loadPluginInstallInfo(String pluginName, File pluginDir) throws Exception
	{
		if (pluginName==null) pluginName = pluginDir.getName();
		_LOGGER.log( Level.FINE, "Loading Plugin install info \"{0}\" from directory \"{1}\" ", new Object[]{ pluginName, pluginDir.getAbsolutePath() }  );
		File[] subFiles = pluginDir.listFiles();
		List<File> xmlFiles = new ArrayList<File>(subFiles.length);
		File metaFile = null;
		for (File subFile : subFiles)
		{
			String lowName = subFile.getName().toLowerCase();
			if (lowName.endsWith( ".xml" ))
				xmlFiles.add( subFile );
			else if (lowName.equals( "metadata.txt" ))
				metaFile = subFile;
		}
		if (xmlFiles.size()==0)
			throw new Exception(String.format( "The plugin directory does not contain any xml binding file: %s", pluginDir.getAbsolutePath()));
		Properties metaProp = readMetaDataFile(metaFile);
		PluginInstallInfo info = new PluginInstallInfo(pluginName, metaProp);
		PluginGroup pluginGrp = PluginGroup.newPluginGroup( pluginName, this, info );
		_LOGGER.log( Level.FINE, "Plugin group name: \"{0}\", type: \"{1}\"", new Object[]{ pluginName, pluginGrp.getClass().getName() } );
		loadPluginXmlFiles(xmlFiles, pluginGrp);
		if (_pluginMap.get( pluginName )==null)
			_pluginMap.put( pluginName, pluginGrp );
	}
	
	private Properties readMetaDataFile(File metaDataFile) throws Exception
	{
		Properties prop = new Properties();
		if (metaDataFile==null) return prop;
		prop.load( new FileInputStream(metaDataFile) );
		return prop;
	}
	
	private void loadPluginXmlFiles(List<File> xmlFiles, PluginGroup pluginGroup) throws Exception
	{
		List<Type> allEnums = new ArrayList<Type>();
		List<Type> allStructs = new ArrayList<Type>();
		List<Type> allExceptions = new ArrayList<Type>();
		List<Type> allMethods = new ArrayList<Type>();
		List<Type> allExterns = new ArrayList<Type>();
		XmlBindingDataParser parser = null;
		for (File xmlBindingFile : xmlFiles)
		{
			XmlBindingData data = XmlBindingData.loadXmlBindingFile( xmlBindingFile );
			parser = new XmlBindingDataParser( data );
			Collection<Type> myTypes = parser.getEnumTypes();
			allEnums.addAll( _valueFactory.addTypeDefinitions( myTypes ) );
			myTypes = parser.getExceptionTypes();
			allExceptions.addAll( _valueFactory.addTypeDefinitions( myTypes ) );
			myTypes = parser.getStructTypes();
			allStructs.addAll( _valueFactory.addTypeDefinitions( myTypes ) );
			myTypes = parser.getExternTypes();
			allExterns.addAll(  _valueFactory.addTypeDefinitions( myTypes ) );
			myTypes = parser.getMethodTypes();
			allMethods.addAll( _valueFactory.addTypeDefinitions( myTypes )  );
		}
		initImpExpHelpers( allEnums, parser );
		initImpExpHelpers( allExceptions, parser );
		initImpExpHelpers( allStructs, parser );
		initImpExpHelpers( allExterns, parser );
		if (allMethods.isEmpty())
		{
			_LOGGER.log( Level.WARNING, "No method is loaded to the dynamic value factory from the plugin directory: {0}", xmlFiles.get( 0 ).getCanonicalFile().getParent() );
		}
		for (Type methodType : allMethods)
		{
			pluginGroup.getInstallInfo().addMethod( methodType );
			methodType.setStubHelper( _stubHelper );
			if (methodType.getDirection()!=Direction.CLIENT)
				_method2PluginMap.put( methodType, pluginGroup );
		}
	}
	
	/**
	 * 
	 * @param applicationName
	 * @return
	 * @throws Exception
	 */
	public ApplicationInstallInfo loadApplicationInstallInfo(String applicationName) throws Exception
	{
		ApplicationInstallInfo appInfo = getApplicationInstallInfo( applicationName );
		if (appInfo!=null)
		{
			_LOGGER.log( Level.FINER, "The application install info for \"{0}\" is already loaded to Etch Router manager", applicationName );
			return appInfo;
		}
		File appDir = new File(_appsRoot, applicationName);
		if ((!appDir.exists()) || (!appDir.isDirectory()))
			throw new Exception(String.format( "The application directory does not exist: %s", appDir.getAbsolutePath()));
		loadApplicationInstallInfo(applicationName, appDir);
		return getApplicationInstallInfo( applicationName );
	}
	
	private void loadApplicationInstallInfo(String applicationName, File appDir) throws Exception
	{
		if (applicationName==null) applicationName = appDir.getName();
		_LOGGER.log( Level.FINE, "Loading Application install info \"{0}\" from directory \"{1}\"", new Object[]{ applicationName, appDir.getAbsolutePath() } );
		File[] subFiles = appDir.listFiles();
		List<File> xmlFiles = new ArrayList<File>(subFiles.length);
		for (File subFile : subFiles)
		{
			if (subFile.getName().toLowerCase().endsWith( ".xml" ))
				xmlFiles.add( subFile );
		}
		if (xmlFiles.size()==0)
			throw new Exception(String.format( "The application directory does not contain any xml binding file: %s", appDir.getAbsolutePath()));
		ApplicationInstallInfo info = new ApplicationInstallInfo(applicationName);
		loadApplicationXmlFiles(xmlFiles, info);
		if (_appMap.get( applicationName )==null)
			_appMap.put( applicationName, info );
	}
	
	private void loadApplicationXmlFiles(List<File> xmlFiles, ApplicationInstallInfo appInfo) throws Exception
	{
		String appName = appInfo.getName();
		List<Type> allMethods = new ArrayList<Type>();
		XmlBindingDataParser parser = null;
		for (File xmlBindingFile : xmlFiles)
		{
			XmlBindingData data = XmlBindingData.loadXmlBindingFile( xmlBindingFile );
			parser = new XmlBindingDataParser( data);
			checkApplicationTypeDefinitions(parser.getEnumTypes(), xmlBindingFile, appName, "Enum");
			checkApplicationTypeDefinitions(parser.getExceptionTypes(), xmlBindingFile, appName, "Exception");
			checkApplicationTypeDefinitions(parser.getStructTypes(), xmlBindingFile, appName, "Struct");
			List<Type> methods = parser.getMethodTypes();
			allMethods.addAll(methods);
			checkApplicationTypeDefinitions(methods, xmlBindingFile, appName, "Method");
		}
		if (allMethods.isEmpty())
		{
			_LOGGER.log( Level.WARNING, "No method is found in the application directory: {0}", xmlFiles.get( 0 ).getCanonicalFile().getParent() );
		}
		for (Type methodType : allMethods)
		{
			appInfo.addMethod( methodType );
		}			
	}
	
	private void checkApplicationTypeDefinitions(Collection<Type> types, File xmlFile, String appName, String typeType)
	{
		for (Type aType : types)
		{
			Type typeFound = _valueFactory.getType( aType.getId() );
			if (typeFound==null)
			{
				_LOGGER.log( Level.WARNING, "The \"{0}\" type \"{1}\" in file \"{2}\" for application \"{3}\" is not defined in DynamicValueFactory.", 
					new Object[]{ typeType, aType.getName(), xmlFile.getAbsolutePath(), appName } );
			}
		}
	}
}
