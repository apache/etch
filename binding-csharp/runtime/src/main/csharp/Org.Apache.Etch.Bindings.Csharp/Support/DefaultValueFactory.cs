// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{

    /// <summary>Default implementation of ValueFactory which provides some dynamic type and field support, as well 
    /// as standard value conversions and import and export.</summary>
    public class DefaultValueFactory : ValueFactory
    {
        private const String ETCH_RUNTIME_EXCEPTION_TYPE_NAME = "_Etch_RuntimeException";

        private const String ETCH_LIST_TYPE_NAME = "_Etch_List";

	    private const String ETCH_MAP_TYPE_NAME = "_Etch_Map";

	    private const String ETCH_SET_TYPE_NAME = "_Etch_Set";

	    private const String ETCH_DATETIME_TYPE_NAME = "_Etch_Datetime";

	    private const String ETCH_AUTH_EXCEPTION_TYPE_NAME = "_Etch_AuthException";

	    private const String ETCH_EXCEPTION_MESSAGE_NAME = "_exception";

	    private const String MSG_FIELD_NAME = "msg";

	    private const String MESSAGE_ID_FIELD_NAME = "_messageId";

	    private const String IN_REPLY_TO_FIELD_NAME = "_inReplyTo";

        private const String RESULT_FIELD_NAME = "result";
     

        public static void Init( TypeMap types, Class2TypeMap class2type )
        {
            RuntimeExceptionSerializer.Init(
             types.Get(ETCH_RUNTIME_EXCEPTION_TYPE_NAME), class2type);

            ListSerializer.Init(
                types.Get(ETCH_LIST_TYPE_NAME), class2type);

            MapSerializer.Init(
                types.Get(ETCH_MAP_TYPE_NAME), class2type);

        /*    SetSerializer.Init(
                types.Get(ETCH_SET_TYPE_NAME), class2type); */

            DateSerializer.Init(
                types.Get(ETCH_DATETIME_TYPE_NAME), class2type);

            AuthExceptionSerializer.Init(
                types.Get(ETCH_AUTH_EXCEPTION_TYPE_NAME), class2type);
         
            XType t3 = types.Get(ETCH_EXCEPTION_MESSAGE_NAME);
            t3.PutValidator(_mf_result, Validator_RuntimeException.Get());
            t3.PutValidator(_mf__messageId, Validator_long.Get(0));
            t3.PutValidator(_mf__inReplyTo, Validator_long.Get(0));
        }

        /// <summary>The msg field of the standard unchecked exception.</summary>
        public readonly static Field _mf_msg = new Field(MSG_FIELD_NAME);

        /// <summary>The well-known _messageId field.</summary>
        public readonly static Field _mf__messageId = new Field(MESSAGE_ID_FIELD_NAME);

        /// <summary>The well-known _inReplyTo field. </summary>
        public readonly static Field _mf__inReplyTo = new Field(IN_REPLY_TO_FIELD_NAME);

        /// <summary> The well-known result field </summary>
        public readonly static Field _mf_result = new Field(RESULT_FIELD_NAME);

        private readonly TypeMap types;

        private readonly TypeMap dynamicTypes = new TypeMap();

	    private readonly Class2TypeMap class2type;
	
	    private readonly XType _mt__Etch_RuntimeException;
	
	    private readonly XType _mt__Etch_AuthException;
	
	    private readonly XType _mt__exception;

        private readonly XType _mt__Etch_List;
	
	    private readonly XType _mt__Etch_Map;
	
	    private readonly XType _mt__Etch_Set;
	
	    private readonly XType _mt__Etch_Datetime;
	

       /// <summary>
        /// Constructs the DefaultValueFactory.
       /// </summary>
       /// <param name="uri"></param>
       /// <param name="types"></param>
       /// <param name="class2type"></param>
        public DefaultValueFactory(String uri, TypeMap types, Class2TypeMap class2type)
        {
            URL u = new URL(uri);
            
            String s = u.GetTerm("DefaultValueFactory.level", "FULL");
            level = (Validator.Level) Enum.Parse(typeof(Validator.Level), s);

            this.types = types;
            this.class2type = class2type;

            _mt__Etch_RuntimeException = types.Get(ETCH_RUNTIME_EXCEPTION_TYPE_NAME);
            _mt__Etch_AuthException = types.Get(ETCH_AUTH_EXCEPTION_TYPE_NAME);
            _mt__exception = types.Get(ETCH_EXCEPTION_MESSAGE_NAME);
            _mt__Etch_List = types.Get(ETCH_LIST_TYPE_NAME);
            _mt__Etch_Map = types.Get(ETCH_MAP_TYPE_NAME);
            _mt__Etch_Set = types.Get(ETCH_SET_TYPE_NAME);
            _mt__Etch_Datetime = types.Get(ETCH_DATETIME_TYPE_NAME);
        }

        public XType get_mt__Etch_RuntimeException()
	    {
		    return _mt__Etch_RuntimeException;
	    }
	
	    public XType get_mt__Etch_AuthException()
	    {
		    return _mt__Etch_AuthException;
	    }
	
	    public  XType get_mt__exception()
	    {
		    return _mt__exception;
	    }

        public XType Get_mt__Etch_List()
	    {
		    return _mt__Etch_List;
	    }
	
	    public XType Get_mt__Etch_Map()
	    {
		    return _mt__Etch_Map;
	    }
	
	    public XType Get_mt__Etch_Set()
	    {
		    return _mt__Etch_Set;
	    }
	
	    public XType Get_mt__Etch_Datetime()
	    {
	    	return _mt__Etch_Datetime;
	    }

        protected void addMixin(ValueFactory vf)
        {
            ValueFactory[] newMixins = new ValueFactory[mixins.Length + 1];
            System.Array.Copy(mixins,newMixins,mixins.Length);
            newMixins[mixins.Length] = vf;
            mixins = newMixins;
        }

        private ValueFactory[] mixins = { };

     

        /////////////////////
        // STRING ENCODING //
        /////////////////////

        public Encoding GetStringEncoding()
        {
            return Encoding.UTF8;
        }

        ////////////////
        // MESSAGE ID //
        ////////////////

        public long? GetMessageId(Message msg)
        {
            return (long?)(msg.Get(_mf__messageId));
        }

        public void SetMessageId(Message msg, long? msgid)
        {
            msg[_mf__messageId] = msgid;
        }

        public Field Get_mf__messageId()
        {
            return _mf__messageId;
        }

        /////////////////
        // IN REPLY TO //
        /////////////////

        public long? GetInReplyTo(Message msg)
        {
            return (long?)(msg.Get(_mf__inReplyTo));
        }

        public void SetInReplyTo(Message msg, long? msgid)
        {
            msg[_mf__inReplyTo] = msgid;
        }

        public Field Get_mf__inReplyTo()
        {
            return _mf__inReplyTo;
        }

        //////////////////////
        // VALUE CONVERSION //
        //////////////////////

       

        public Object ImportCustomValue( StructValue sv )
        {
            ImportExportHelper helper = sv.GetXType.GetImportExportHelper();

            if ( helper == null )
                return null;

            return helper.ImportValue( sv );
        }

       

        #region ValueFactory members

        public StructValue ExportCustomValue( Object value )
        {
            Type clss = value.GetType();
            XType type;
            try
            {
                type = GetCustomStructType(value.GetType());
            }
            catch (KeyNotFoundException)
            {
                type = null;
            }

            if (type == null && clss == typeof(StructValue) )
		    {
			    StructValue struct1 = (StructValue) value;
		        type = struct1.GetXType;
		    }

            if (type == null)
                //       if (value is Exception)
                if (typeof(Exception).IsAssignableFrom(clss))
                    type = _mt__Etch_RuntimeException;
                else if (typeof(IList).IsAssignableFrom(clss))
                    type = _mt__Etch_List;
                else if (typeof(IDictionary).IsAssignableFrom(clss))
                    type = _mt__Etch_Map;
                // Need to add set, but in 2.0 there is no set so just use keys of IDictionary, 
                else
                    return null;
		
		    ImportExportHelper helper = type.GetImportExportHelper();
		
		    if (helper == null)
			    return null;
		
		    return helper.ExportValue( this, value );
        }

        public XType GetCustomStructType( Type t )
        {
            XType type = class2type.Get( t );
		    if (type != null)
			    return type;
		
		    foreach (ValueFactory vf in mixins)
			    if ((type = vf.GetCustomStructType( t )) != null)
				    return type;
		
		    return null;
           
        }

        public XType GetType( int id )
        {
            XType type = types.Get(id);
            if (type != null)
                return type;

            foreach (ValueFactory vf in mixins)
                if ((type = vf.GetType(id)) != null)
                    return type;

            lock (dynamicTypes)
            {
                return dynamicTypes.Get(id);
            }
        }

        public XType GetType( string name )
        {
            XType type = types.Get(name);
            if (type != null)
                return type;

            foreach (ValueFactory vf in mixins)
                if ((type = vf.GetType(name)) != null)
                    return type;

            lock (dynamicTypes)
            {
                return dynamicTypes.Get(name);
            }
        }

        public void AddType(XType type)
        {
            lock (dynamicTypes)
            {
                dynamicTypes.Add(type);
            }
        }

        public  ICollection<XType> GetTypes()
        {
            ICollection<XType> values = types.Values();

            foreach (ValueFactory vf in mixins)
                foreach (XType x in vf.GetTypes())
                    if (!values.Contains(x))
                        values.Add(x);

            lock (dynamicTypes)
            {
                foreach (XType x in dynamicTypes.Values())
                    if (!values.Contains(x))
                        values.Add(x);
            }

            return values;
        }

        #endregion


        public void LockDynamicTypes()
        {
            dynamicTypes.Lock();
        }

        public void UnlockDynamicTypes()
        {
            // TODO dynamicTypes.Unlock();
            throw new NotSupportedException("The method or operation is not implemented.");
        }

        public Validator.Level GetLevel()
        {
            return level;
        }

        public Validator.Level SetLevel(Validator.Level level)
        {
            Validator.Level oldLevel = this.level;
            this.level = level;
            return oldLevel;
        }

        private Validator.Level level = Validator.Level.FULL;
    }
}