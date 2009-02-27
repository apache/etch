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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.etch.bindings.java.msg.AsyncMode;
import org.apache.etch.bindings.java.msg.Direction;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.Validator;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
import org.apache.etch.bindings.java.support.Validator_StructValue;
import org.apache.etch.bindings.java.support.Validator_boolean;
import org.apache.etch.bindings.java.support.Validator_byte;
import org.apache.etch.bindings.java.support.Validator_custom;
import org.apache.etch.bindings.java.support.Validator_double;
import org.apache.etch.bindings.java.support.Validator_float;
import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.bindings.java.support.Validator_long;
import org.apache.etch.bindings.java.support.Validator_object;
import org.apache.etch.bindings.java.support.Validator_short;
import org.apache.etch.bindings.java.support.Validator_string;
import org.apache.etch.bindings.java.support.Validator_void;
import org.apache.etch.services.router.xml.Module;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class XmlBindingDataParser
{

	private static final Logger _LOGGER = Logger.getLogger( XmlBindingDataParser.class.getName());
	
	private Map<String, LocalTypeImportExportHelper> _localImpExpHelperMap = null;
	
	private Map<String, StructValueImportExportHelper> _structValueImpExpHelperMap = null;
	
	private Map<Type, Module.Service.Structs.Struct> _structType2StructMap = null;
	
	private Map<Type, org.apache.etch.services.router.xml.Exception> _excpType2ExcpMap = null;
	
	private Map<Type, Module.Service.Methods.Method> _methodType2MethodMap = null;
	
	private List<Type> _resultMethodTypes = null;
	
	private List<Type> _methodTypes = null;
	
	private List<Type> _enumTypes = null;
	
	private List<Type> _structTypes = null;
	
	private List<Type> _exceptionTypes = null;
	
	private List<Type> _externTypes = null;
	
	// extern name/shortName to fully qualified name map:
	private Map<String, String> _externTypeNameMap = null;
	
	private StructValueImportExportHelper _structValueHelper = null;
	
	private Map<String, Type> _typeName2TypeMap = null;
	
	/**
	 * 
	 * @param data
	 * @throws Exception
	 */
	public XmlBindingDataParser( XmlBindingData data )
		throws Exception
	{
		_structValueHelper = new StructValueImportExportHelper();
		_localImpExpHelperMap = new HashMap<String, LocalTypeImportExportHelper>();
		_structType2StructMap = new HashMap<Type, Module.Service.Structs.Struct>();
		_excpType2ExcpMap = new HashMap<Type, org.apache.etch.services.router.xml.Exception>();
		_methodType2MethodMap = new HashMap<Type, Module.Service.Methods.Method>();
		_resultMethodTypes = new ArrayList<Type>();
		_methodTypes = new ArrayList<Type>();
		_enumTypes = new ArrayList<Type>();
		_structTypes = new ArrayList<Type>();
		_exceptionTypes = new ArrayList<Type>();
		_typeName2TypeMap = new HashMap<String, Type>();
		_structValueImpExpHelperMap = new HashMap<String, StructValueImportExportHelper>();
		_externTypes = new ArrayList<Type>();
		_externTypeNameMap = new HashMap<String, String>(); 
		
		parseData(data);
	}
	
	private void parseData(XmlBindingData data) throws Exception
	{
		parseEnums(data.getEnums());
		parseStructs(data.getStructs());
		parseExceptions(data.getExceptions());
		parseExterns(data.getExterns());
		parseMethods(data.getMethods());
		parseFields();
	}
	
	private void parseExterns(List<Module.Service.Externs.Extern> externs) throws Exception
	{
		String name, typeName;
		for (Module.Service.Externs.Extern extern : externs)
		{
			name = extern.getName();
			typeName = extern.getTypeName();
			_externTypeNameMap.put( name, typeName );
			_externTypeNameMap.put( typeName, typeName );
			Type myType = new Type(typeName);
			_externTypes.add( myType );
			_typeName2TypeMap.put( typeName, myType );
			_structValueImpExpHelperMap.put( myType.getName(), _structValueHelper );
		}
	}
	
	private void parseFields() throws Exception
	{
		parseStructFields();
		parseExceptionFields();
		parseMethodFields();
	}
	
	public LocalTypeImportExportHelper getLocalImportExportHelper( String typeName )
	{
		return _localImpExpHelperMap.get( typeName );
	}
	
	public StructValueImportExportHelper getStructValueImportExportHelper( String typeName )
	{
		return _structValueImpExpHelperMap.get( typeName );
	}
	
	public List<Type> getMethodTypes()
	{
		return _methodTypes;
	}

	public List<Type> getEnumTypes()
	{
		return _enumTypes;
	}

	public List<Type> getStructTypes()
	{
		return _structTypes;
	}

	public List<Type> getExceptionTypes()
	{
		return _exceptionTypes;
	}

	public List<Type> getExternTypes()
	{
		return _externTypes;
	}
	/**
	 * 
	 * @param enums
	 * @return
	 */
	private void parseEnums(List<Module.Service.Enums.Enum> enums)
		throws Exception
	{
		String typeName = null;
		for (Module.Service.Enums.Enum myenum : enums)
		{
			typeName = myenum.getTypeName();
			Type myType = new Type(typeName);
			_enumTypes.add( myType );
			_typeName2TypeMap.put( typeName, myType );
			resolveEnumType(myenum, myType);
		}
	}
	
	private void resolveEnumType(Module.Service.Enums.Enum myenum, Type myType)
		throws Exception
	{
		List<Module.Service.Enums.Enum.Entry> entries = myenum.getEntry();
		List<String> entryValues = new ArrayList<String>(entries.size());
		for (Module.Service.Enums.Enum.Entry entry : entries)
		{
			entryValues.add( entry.getValue() );
			myType.putValidator( new org.apache.etch.bindings.java.msg.Field( entry.getValue()), org.apache.etch.bindings.java.support.Validator_boolean.get( 0 ) );
		}
		try
		{
			LocalEnumImportExportHelper helper = new LocalEnumImportExportHelper(myType, entryValues);
			_LOGGER.log( Level.FINE, "The enum type \"{0}\" is locally defined", myType.getName() );
			_localImpExpHelperMap.put( myType.getName(), helper );
		}
		catch (ClassNotFoundException e)
		{
			_LOGGER.log( Level.FINEST, String.format( "The enum type \"{0}\" is not locally defined: ", myType.getName()), e );
			_structValueImpExpHelperMap.put( myType.getName(), _structValueHelper );
		}	
	}
	
	/**
	 * 
	 * @param structs
	 * @return
	 */
	private void parseStructs(List<Module.Service.Structs.Struct> structs)
		throws Exception
	{
		String typeName = null;
		for (Module.Service.Structs.Struct mystruct : structs)
		{
			typeName = mystruct.getTypeName();
			Type myType = new Type(typeName);
			_structTypes.add( myType );
			_structType2StructMap.put( myType, mystruct );
			_typeName2TypeMap.put( typeName, myType );
			resolveStructType(mystruct, myType);
		}
	}
	
	private void resolveStructType(Module.Service.Structs.Struct mystruct, Type myType)
		throws Exception
	{
		List<org.apache.etch.services.router.xml.Field> fields = mystruct.getField();
		List<String> fieldNames = new ArrayList<String>(fields.size());
		for (org.apache.etch.services.router.xml.Field field : fields)
		{
			fieldNames.add( field.getName() );
		}
		try
		{
			LocalStructImportExportHelper helper = new LocalStructImportExportHelper(myType, fieldNames);
			_LOGGER.log( Level.FINE, "The struct type \"{0}\" is locally defined: ", myType.getName() );
			_localImpExpHelperMap.put( myType.getName(), helper );
		}
		catch (ClassNotFoundException e)
		{
			_LOGGER.log( Level.FINEST, String.format( "The struct type \"%s\" is not locally defined: ", myType.getName()), e );
			_structValueImpExpHelperMap.put( myType.getName(), _structValueHelper );
		}
	}
	
	private void parseStructFields() throws Exception
	{
		for (Type structType : _structTypes)
		{
			Module.Service.Structs.Struct mystruct = _structType2StructMap.get( structType );
			parseStructFields( mystruct, structType);
		}
	}
	
	private void parseStructFields( Module.Service.Structs.Struct mystruct, Type structType )
	{
		List<org.apache.etch.services.router.xml.Field> myFields = mystruct.getField();
		resolveFieldValidators( structType, myFields );
	}
	
	/**
	 * 
	 * @param exceptions
	 * @return
	 */
	private void parseExceptions(List<org.apache.etch.services.router.xml.Exception> exceptions)
		throws Exception
	{
		String typeName = null;
		for (org.apache.etch.services.router.xml.Exception myexp : exceptions)
		{
			typeName = myexp.getTypeName();
			Type myType = new Type(typeName);
			_exceptionTypes.add( myType );
			_excpType2ExcpMap.put( myType, myexp );
			_typeName2TypeMap.put( typeName, myType );
			resolveExceptionType(myexp, myType);
		}
	}

	private void resolveExceptionType(org.apache.etch.services.router.xml.Exception myexp, Type myType)
		throws Exception
	{
		List<org.apache.etch.services.router.xml.Field> fields = myexp.getField();
		List<String> fieldNames = new ArrayList<String>(fields.size());
		for (org.apache.etch.services.router.xml.Field field : fields)
		{
			fieldNames.add( field.getName() );
		}
		try
		{
			LocalStructImportExportHelper helper = new LocalStructImportExportHelper(myType, fieldNames);
			_LOGGER.log( Level.FINE, "The exception type \"{0}\" is locally defined", myType.getName() );
			_localImpExpHelperMap.put( myType.getName(), helper );
		}
		catch (ClassNotFoundException e)
		{
			_LOGGER.log( Level.FINEST, String.format( "The exception type \"%s\" is not locally defined: ", myType.getName()), e );
			_structValueImpExpHelperMap.put( myType.getName(), _structValueHelper );
		}
	}
	
	private void parseExceptionFields()
	{
		for (Type myType : _exceptionTypes)
		{
			org.apache.etch.services.router.xml.Exception myexcp = _excpType2ExcpMap.get( myType );
			parseExceptionFields( myexcp, myType);
		}
	}
	
	private void parseExceptionFields( org.apache.etch.services.router.xml.Exception myexcp, Type excpType)
	{
		List<org.apache.etch.services.router.xml.Field> myFields = myexcp.getField();
		resolveFieldValidators( excpType, myFields );
	}
	
	/**
	 * Note: this must be called after all enum, struct and exception types are parsed, otherwise
	 * the import-export helpers will not be created for the validator setup
	 * 
	 * @param methods
	 * @return
	 */	
	private void parseMethods(List<Module.Service.Methods.Method> methods)
	{
		Map<String, Type> types = new LinkedHashMap<String, Type>(methods.size());
		Map<Type, Module.Service.Methods.Method.Result> type2resultMap = 
			new HashMap<Type, Module.Service.Methods.Method.Result>(methods.size()); 
		String typeName = null;
		for (Module.Service.Methods.Method mymethod : methods)
		{
			typeName = mymethod.getTypeName();
			Type myType = new Type(typeName);
			_methodTypes.add( myType );
			types.put( typeName, myType );
			_methodType2MethodMap.put( myType, mymethod );
			String asyncReceiverMode = mymethod.getAsyncReceiverMode();
			//String isOneway = mymethod.getIsOneway();
			String messageDirection = mymethod.getMessageDirection();
			AsyncMode mode = getAsyncModeEnumByName( asyncReceiverMode );
			if (mode==null)
			{
				_LOGGER.log( Level.SEVERE, "Unknown AsyncReceiverMode attribute \"{0}\" for method \"{1}\" ", new Object[]{ asyncReceiverMode, typeName } );
			}
			else
			{
				myType.setAsyncMode( mode );
			}
			Direction direction = getDirectionEnumByName( messageDirection );
			if (direction==null)
			{
				_LOGGER.log( Level.SEVERE, "Unknown MessageDirection attribute \"{0}\" for method \"{1}\" ", new Object[]{ messageDirection, typeName } );
			}
			else
			{
				myType.setDirection( direction );
			}
			String timeout = mymethod.getTimeout();
			if (timeout!=null)
			{
				try
				{
					myType.setTimeout( Integer.parseInt( timeout ) );
				}
				catch (Exception e)
				{
					_LOGGER.log( Level.SEVERE, "Unknown TimeOut attribute \"{0}\" for method \"{1}\" ", new Object[]{ timeout, typeName } );
				}
			}
			//String responseField = mymethod.getResponseField();
			List<Module.Service.Methods.Method.Result> methResults = mymethod.getResult();
			if (methResults != null && (!methResults.isEmpty()))
			{
				type2resultMap.put( myType, methResults.get( 0 ) );
			}
		}
		Set<Type> keys = type2resultMap.keySet();
		for (Type myType : keys)
		{
			Module.Service.Methods.Method.Result result = type2resultMap.get( myType );
			String fieldName = result.getFieldName();
			if (fieldName != null)
			{
				Type resultType = types.get( fieldName );
				if (resultType==null)
				{
					_LOGGER.log( Level.SEVERE, "The result field \"{0}\" is not defined for method \"{1}\" ", new Object[] { fieldName, myType.getName() } );
				}
				else
				{
					myType.setResult( resultType );
					_resultMethodTypes.add( resultType );
				}
			}
		}
	}
	
	private void parseMethodFields()
	{
		for (Type myType : _methodTypes)
		{
			Module.Service.Methods.Method mymethod = _methodType2MethodMap.get( myType );
			boolean isResultMethod = _resultMethodTypes.contains( myType );
			parseMethodFields( mymethod, myType, isResultMethod);
		}
	}
	
	private void parseMethodFields(Module.Service.Methods.Method mymethod, Type myType, boolean isResultMethod )
	{
		if (isResultMethod)
		{
			myType.putValidator( DefaultValueFactory._mf__inReplyTo, org.apache.etch.bindings.java.support.Validator_long.get( 0 ) );
			myType.putValidator( DefaultValueFactory._mf_result, org.apache.etch.bindings.java.support.Validator_RuntimeException.get() );
		}
		myType.putValidator( DefaultValueFactory._mf__messageId, org.apache.etch.bindings.java.support.Validator_long.get( 0 ) );
		List<org.apache.etch.services.router.xml.Field> myfields = mymethod.getField();
		resolveFieldValidators( myType, myfields);
		if (isResultMethod) return;
		Type resultType = myType.getResult();
		if (resultType==null) return;
		List<org.apache.etch.services.router.xml.Exception> myexps = mymethod.getException();
		if (myexps!=null && myexps.size()>0)
		{
			for (org.apache.etch.services.router.xml.Exception exp : myexps)
			{
				String expTypeName = exp.getType();
				LocalTypeImportExportHelper helper = getLocalImportExportHelper( expTypeName );
				if (helper != null)
				{
					resultType.putValidator( DefaultValueFactory._mf_result, Validator_custom.get( helper.getTypeClass(), 0, true ) );
				}
			}
		}
	}
	
	private void resolveFieldValidators( Type myType, List<org.apache.etch.services.router.xml.Field> myFields)
	{
		for (org.apache.etch.services.router.xml.Field myfield : myFields)
		{
			String fieldName = myfield.getName();
			String isArray = myfield.getIsArray();
			String dimension = myfield.getDimension();
			int nDims = 0;
			if (!"false".equalsIgnoreCase( isArray ))
			{
				try
				{
					nDims = Integer.parseInt( dimension );
				}
				catch (Exception e)
				{
					_LOGGER.log( Level.SEVERE, String.format( "Invalid dimmension attribute of field \"%s\" in Type \"%s\"", fieldName, myType.getName() ), e );
				}
			}
			String isPrimitive = myfield.getIsPrimitiveType();
			String typeName = myfield.getType();
			Validator validator = getWellknownValidator(typeName, nDims);
			org.apache.etch.bindings.java.msg.Field thisField = new org.apache.etch.bindings.java.msg.Field(fieldName);
			if (validator!=null)
			{
				myType.putValidator( thisField, validator );
			}
			else if ("true".equalsIgnoreCase( isPrimitive ))
			{
				_LOGGER.log( Level.SEVERE, "Unknown primitive type \"{0}\" of field \"{1}\" in Type \"{2}\"", new Object[] { typeName, fieldName, myType.getName() } );
			}
			else
			{
				LocalTypeImportExportHelper helper = getLocalImportExportHelper( typeName );
				if (helper != null)
				{
					myType.putValidator( thisField, Validator_custom.get( helper.getTypeClass(), nDims, true ) );
				}
				else
				{
					StructValueImportExportHelper structHelper = getStructValueImportExportHelper( typeName );
					boolean done = false;
					if (structHelper!=null)
					{
						Type fieldType = _typeName2TypeMap.get( typeName );
						if (fieldType!=null)
						{
							myType.putValidator( thisField, Validator_StructValue.get( fieldType, nDims ) );
							done = true;
						}
					}
					else
					{
						String externFullName = _externTypeNameMap.get( typeName );
						if (externFullName != null)
						{
							structHelper = getStructValueImportExportHelper( externFullName );
							Type fieldType = _typeName2TypeMap.get( externFullName );
							if (structHelper!=null && fieldType!=null)
							{
								myType.putValidator( thisField, Validator_StructValue.get( fieldType, nDims ) );
								done = true;
							}
						}
					}
					if (!done)
						_LOGGER.log( Level.SEVERE, "The custom type \"{0}\" of field \"{1}\" in type \"{2}\" is not defined", new Object[] { typeName, fieldName, myType.getName() } );
				}
			}
		}

	}
	
	private AsyncMode getAsyncModeEnumByName(String name)
	{
		AsyncMode[] vals = AsyncMode.values();
		for (AsyncMode val : vals)
		{
			if (val.name().equalsIgnoreCase( name ))
				return val;
		}
		return null;
	}

	private Direction getDirectionEnumByName(String name)
	{
		Direction[] vals = Direction.values();
		for (Direction val : vals)
		{
			if (val.name().equalsIgnoreCase( name ))
				return val;
		}
		return null;	
	}
	
	/**
	 * Get non-customized type's validator by name and dimensions
	 * 
	 * @param typeName
	 * @param nDims
	 * @return
	 */
	public static Validator getWellknownValidator(String typeName, int nDims)
	{
		if ("void".equals( typeName ))
		{
			return Validator_void.get( nDims );
		}
		else if ("boolean".equals( typeName ))
		{
			return Validator_boolean.get(nDims);
		}
		else if ("byte".equals( typeName ))
		{
			return Validator_byte.get(nDims);
		}
		else if ("double".equals( typeName ))
		{
			return Validator_double.get(nDims);
		}
		else if ("float".equals( typeName ))
		{
			return Validator_float.get(nDims);
		}
		else if ("int".equals( typeName ))
		{
			return Validator_int.get(nDims);
		}
		else if ("long".equals( typeName ))
		{
			return Validator_long.get(nDims);
		}
		else if ("object".equals( typeName ))
		{
			return Validator_object.get(nDims);
		}
		else if ("short".equals( typeName ))
		{
			return Validator_short.get(nDims);
		}
		else if ("string".equals( typeName ))
		{
			return Validator_string.get(nDims);
		}
		else if ("Map".equals( typeName ))
		{
			return Validator_custom.get( java.util.Map.class, nDims, true );
		}
		else if ("Datetime".equals( typeName ))
		{
			return Validator_custom.get( java.util.Date.class, nDims, true );
		}
		else if ("List".equals( typeName ))
		{
			return Validator_custom.get( java.util.List.class, nDims, true );
		}
		else if ("Set".equals( typeName ))
		{
			return Validator_custom.get( java.util.Set.class, nDims, true );
		}
		
		return null;
	}
}
