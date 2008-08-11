// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;
using System.IO;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Util;
using Etch.Support;

namespace Etch.Transport
{
    /// <summary> A message conversion class, that specializes is converting from JSON to Etch Message format </summary>
    public class JsonInputParser
    {
        #region Type Codes
        private static int boolCode = typeof( bool ).GetHashCode();
        private static int stringCode = typeof( string ).GetHashCode();
        private static int intCode = typeof( int ).GetHashCode();
        private static int longCode = typeof( long ).GetHashCode();
        private static int floatCode = typeof( float ).GetHashCode();
        private static int doubleCode = typeof( double ).GetHashCode();
        private static int decimalCode = typeof( decimal ).GetHashCode();
        private static int dictionaryCode = typeof( Dictionary<string, object> ).GetHashCode();
        private static int listCode = typeof( System.Collections.IList ).GetHashCode();
        private static int mapCode = typeof( System.Collections.IDictionary ).GetHashCode();
        private static int setCode = typeof( System.Collections.IDictionary ).GetHashCode();
        #endregion

        private static XType listXType = null;

        private ValueFactory valueFactory;

        public JsonInputParser(ValueFactory valueFactory)
        {
            this.valueFactory = valueFactory;

            listXType = valueFactory.GetCustomStructType( typeof( System.Collections.ArrayList ) );
        }

        public enum MessageType
        {
            Request,
            Response
        }

        #region Example Messages
        /* Example JSON request message
         * { 
         *      version: 1,                                 // the format of the rest of the JSON object
         *      sessionId: 3,                               // the ID of the client-server session (HTTP can't do this for us as a transport)
         *      transactionId: 2                            // a transactionID used to correlate whether 
         *      method: "com.cisco.service.method",         // the method being invoked if this is a client-originating method. otherwise not present if this is a response
         *      args : { methodArg1 : __, methodArg2 : ___, ... }  // an optionally present object holding the arguments of the method
         */

        /* Example JSON response message
         * { 
         *      version: 1,                                 // the format of the rest of the JSON object
         *      sessionId: 3,                               // the ID of the client-server session (HTTP can't do this for us as a transport)
         *      transactionId: 2                            // a transactionID used to correlate whether 
         *      data : primitive | { ... }                  // an optionally present object or primitive holding the return data of a method response
         */
        #endregion


        /// <summary>
        /// Parses a JSON-formated message, and converts it into a JSON message
        /// </summary>
        /// <returns>A constructed message read from the JSON reader</returns>
        public Message ReadRequestMessage(int version, string methodName, string messageId, Dictionary<string, object> args)
        {
            // Room for possible optimization by avoiding object creation, so we could just pass the JSONReader,
            // but for speed-of-implementation, we'll pass the info pre-parsed from underlying HTTP stack
            if(version != BrowserTransporter.VERSION)
            {
                throw new IOException( "POJSON data version mismatch: got " + version + " expected " + BrowserTransporter.VERSION );
            }
           
            return ParseRequest(messageId, methodName, args);
        }

        /// <summary>
        /// Parses a JSON-formated message, and converts it into a JSON message
        /// </summary>
        /// <returns>A constructed message read from the JSON reader</returns>
        public Message ReadResponseMessage( int version, string messageId, string originalMethodName, object returnData )
        {
            // Room for possible optimization by avoiding object creation, so we could just pass the JSONReader,
            // but for speed-of-implementation, we'll pass the info pre-parsed from underlying HTTP stack
            if( version != BrowserTransporter.VERSION )
            {
                throw new IOException( "POJSON data version mismatch: got " + version + " expected " + BrowserTransporter.VERSION );
            }

            return ParseResponse( messageId, originalMethodName, returnData );
        }

        private Message ParseRequest(string messageIdAsString, string method, Dictionary<string, object> args)
        {
            // messageId must be long-parseable
            long messageId = long.Parse( messageIdAsString );

            // The method should be 1:1 with a real method name found in value factory
            XType methodType = valueFactory.GetType(method);

            // Construct the etch message from the method type
            Message message = new Message(methodType, valueFactory);

            // The client specified message ID is used as the key for the internal Etch message
            message.MessageId = messageId;

            // Begin the process of extracting arguments for this method out of the message 'args' body
            List<Field> list = methodType.GetFields();

            if(list != null)
            {
                // each field should correspond to method arguments
                foreach(Field field in list)
                {
                    Validator fieldValidator = methodType.GetValidator(field);
                    object argument = ParseArgument(field, fieldValidator, args);

                    // Null means not specified and/or a value of null
                    if(argument != null)
                    {
                        message.Add(field, argument);
                    }
                }
            }

            return message;
        }

        /// <summary>
        ///     Parses a given field into a struct
        /// </summary>
        /// <param name="field"></param>
        /// <param name="fieldValidator"></param>
        /// <param name="args"></param>
        /// <returns>Null if the field is not present/null</returns>
        private object ParseArgument(Field field, Validator fieldValidator, Dictionary<string, object> args)
        {
            XType fieldType = valueFactory.GetType(field.Id);
            
            object argumentValue = null;

            if(args != null && args.ContainsKey(field.Name))
            {
                // This is in a JsonReader-specific format.  it must be further processed
                object unprocessedArgumentValue = args[field.Name];

                argumentValue = ProcessValue(fieldValidator, unprocessedArgumentValue, true);
            }

            return argumentValue;
        }

        private StructValue ProcessStruct( Validator structValidator, Dictionary<string, object> data )
        {
            StructValue etchDataWrapper = null;

            // the client should pass in the type as @type
            if(data.ContainsKey("@type"))
            {
                XType structType = valueFactory.GetType( data["@type"] as string );

                if( structType != null )
                {
                    etchDataWrapper = new StructValue(structType, valueFactory, data.Count );

                    // Begin the process of extracting arguments for this method out of the message 'args' body
                    List<Field> list = structType.GetFields();

                    if( list != null )
                    {
                        // each field should correspond to method arguments
                        foreach( Field field in list )
                        {
                            Validator fieldValidator = structType.GetValidator( field );
                            object argument = ParseArgument( field, fieldValidator, data );

                            // Null means not specified and/or a value of null
                            if( argument != null )
                            {
                                etchDataWrapper.Add( field, argument );
                            }
                        }
                    }
                }
            }

            return etchDataWrapper;
        }

        private Object ValidateValue(Validator v, Object value)
        {
            if(v == null)
                throw new Exception("No validator");

            if(value == null)
                return null;

            return v.ValidateValue(value);
        }

        /// <summary> Processes any valid value incoming from JsonReader </summary>
        private object ProcessValue( Validator fieldValidator, object unprocessedArgumentValue, bool initialIteration )
        {
            object argumentValue = null;

            if(unprocessedArgumentValue != null)
            {
                Type type = unprocessedArgumentValue.GetType();

                int typeCode = type.GetHashCode();

                 // notable exceptions not found here:
                    //  byte
                    //  datetime
                    //  short

                if(typeCode == boolCode)
                {
                    argumentValue = ValidateValue(fieldValidator, unprocessedArgumentValue);
                }
                else if(typeCode == stringCode)
                {
                    argumentValue = ValidateValue(fieldValidator, unprocessedArgumentValue);
                }
                else if(typeCode == intCode || typeCode == longCode || typeCode == floatCode || typeCode == doubleCode || typeCode == decimalCode)
                {
                    argumentValue = ValidateValue(fieldValidator, unprocessedArgumentValue);
                }
                else if(typeCode == dictionaryCode)
                {
                    argumentValue = ValidateValue(fieldValidator, valueFactory.ImportCustomValue( ProcessStruct(fieldValidator, (Dictionary<string, object>)unprocessedArgumentValue) ) );
                }
                else if( type.IsArray )
                {
                    bool processedAsNativeListType = false;
                    // is the expected type an [], a List, or a Set?
                    TypeValidator typeValidator = fieldValidator as TypeValidator;
                    if( typeValidator != null)
                    {
                        Type expectedType = typeValidator.GetExpectedClass();
                        if( expectedType != null )
                        {
                            int expectedTypeCode = expectedType.GetHashCode();

                            if( expectedTypeCode == listCode )
                            {
                                argumentValue = valueFactory.ImportCustomValue( ProcessList(fieldValidator, (Array) unprocessedArgumentValue, initialIteration));
                                processedAsNativeListType = true;

                            }
                            else if( expectedTypeCode == mapCode || expectedTypeCode == setCode )
                            {
                                Console.Write( "//TODO SUPPORT MAP && SET" );
                            }
                        }
                    }
                    
                    if(!processedAsNativeListType)
                    {
                        // a type code is not used here, because the JsonFX library can pass up arrays of different derived types.  It likes to cast to a specific array type, if possible.
                        argumentValue = ValidateValue(fieldValidator, ProcessArray(fieldValidator, (Array) unprocessedArgumentValue, initialIteration));
                    }               
                }
                else
                {
                    System.Diagnostics.Debug.Assert( false, "unexpected type in ProcessValue" );
                }
            }

            return argumentValue;
        }

        #region List Parsing Code
        
        /// <summary>
        ///     TODO: Need some guidance on this implementation.   This may work, but it may be done incorrectly
        /// </summary>
        /// <returns></returns>
        private StructValue ProcessList( Validator fieldValidator, Array unprocessedValues, bool initialIteration )
        {
            // TODO: Need to make listValues public on List XType ListSerializer
            Field listValuesField = new Field("listValues");
            Validator objectValidator = listXType.GetValidator(listValuesField).ElementValidator();
            
            StructValue listStruct = new StructValue( listXType, valueFactory );
            
            object[] values = new object[unprocessedValues.Length];
            for(int i = 0; i < unprocessedValues.Length; i++)
            {
                object unprocessedValue = unprocessedValues.GetValue(i);
                values[i] = ProcessValue( objectValidator, unprocessedValue, false );
            }

            listStruct.Add( listValuesField, values );

            return listStruct;
        }

        #endregion

        #region Array Parsing Code

        private Array ProcessArray(Validator arrayValidator, Array unprocessedValues, bool initialIteration )
        {
            Array value = null;

            if(unprocessedValues != null)
            {
                // determine type of array
                Type arrayType = null;
                //int arrayDimension = 0;
                int arrayLength = unprocessedValues.Length;

                if( arrayValidator is TypeValidator )
                {
                    TypeValidator typedValidator = (TypeValidator) arrayValidator;
                    arrayType = typedValidator.GetExpectedClass();
                    //arrayDimension = typedValidator.GetNDims();
                }
                else
                {
                    System.Diagnostics.Debug.Assert( false, "Unexpected validator.  Validator is of type: " + arrayValidator );
                }

                // check to see if the validator
                if( arrayType != null )
                {
                    // we need to back this off by one-dimension, in order for Array.CreateInstance
                    if( arrayType.IsArray)
                    {
                        arrayType = arrayType.GetElementType();
                    }


                    // determine if the incoming array has a more specific type than object[],
                    // if the IDL only specifies an Object in terms of specifity
                    // as the type.  Ex: If a client sends in a string[], we should
                    // allow a string[] to come through on the server side,
                    // if the IDL specified only an object for that parameter
                    if(!initialIteration && arrayType == typeof(object))
                    {
                        Type elementType = unprocessedValues.GetType().GetElementType();
                        if( elementType != typeof( Dictionary<string, object> ) ) // any type passed in by JsonFX is primitive (and therefore OK).. except this
                        {
                            arrayType = elementType;
                        }
                    }

                    value = Array.CreateInstance(arrayType, arrayLength);

                    for( int i = 0; i < arrayLength; i++ )
                    {
                        object unprocessedItem = unprocessedValues.GetValue(i);
                        value.SetValue( ProcessValue( arrayValidator.ElementValidator(), unprocessedItem, false ), i );
                    }
                }
            }

            return value;
        }
            
 
        protected Object FromArrayValue( ArrayValue av )
        {
            return av.GetArray();
        }

        #endregion 

        private Message ParseResponse(string messageIdAsString, string originalMethodName, object returnData)
        {
            // messageId must be long-parseable
            long messageId = long.Parse( messageIdAsString );

            XType methodType = valueFactory.GetType(originalMethodName);

            if( methodType != null )
            {
                Message message = new Message( methodType, valueFactory );
                message = message.Reply();
                message.InReplyTo = messageId;

                // Begin the process of extracting arguments for this method out of the message 'args' body
                XType resultType = methodType.GetResult();
                Field resultField = resultType.GetField(Etch.Support.DefaultValueFactory._mf_result.Id);

                Validator fieldValidator = resultType.GetValidator( resultField );
                object argument = ProcessValue( fieldValidator, returnData, true );

                // Null means not specified and/or a value of null
                //if( argument != null )
                //{
                    message.Add( resultField, argument );
                //}
              
                return message;
            }
            else
            {
                // TOOD: log error about no method type, throw exception too?
                throw new ApplicationException( "unknown method: " + originalMethodName );
            }
        }

        /// <summary>
        ///     Assumes the presence of version in the message, and extracts it. If not present or non-int, an Exception will be thrown
        /// </summary>
        /// <param name="genericObject">The message originating from the client</param>
        /// <returns>The found version</returns>
        private int ExtractVersion(Dictionary<string, object> genericObject)
        {
            int version = 0;
            if(genericObject.ContainsKey("version"))
            {
                try
                {
                    version = (int) genericObject["version"];
                }
                catch
                {
                    // There is something in the version field, but it's not an int!
                    throw new Exception("non-integer version");
                }
            }
            else
            {
                // There is no version specified in the messaging.
                throw new Exception("missing version");
            }

            return version;
        }

        /// <summary>
        ///     Assumes the presence of 'transactionId' in the message, and extracts it. If not present or non-long, an Exception will be thrown
        /// </summary>
        /// <param name="genericObject">The message originating from the client</param>
        /// <returns>The found transactionId</returns>
        private long ExtractTransactionId(Dictionary<string, object> genericObject)
        {
            long transactionId = 0;
            if(genericObject.ContainsKey("transactionId"))
            {
                try
                {
                    transactionId = (long) genericObject["transactionId"];
                }
                catch
                {
                    // There is something in the transacationId field, but it's not an long!
                    throw new Exception("non-long transactionId");
                }
            }
            else
            {
                // There is no transactionId specified in the messaging.
                throw new Exception("missing transactionId");
            }

            return transactionId;
        }

        /// <summary>
        ///     Assumes the presence of 'method' in the message, and extracts it. If not present or non-string, an Exception will be thrown
        /// </summary>
        /// <param name="genericObject">The message originating from the client</param>
        /// <returns>The found method</returns>
        private string ExtractMethod( Dictionary<string, object> genericObject )
        {
            string method = null;
            if( genericObject.ContainsKey( "method" ) )
            {
                try
                {
                    method = (string) genericObject["method"];
                }
                catch
                {
                    // There is something in the method field, but it's not a string!
                    throw new Exception( "method parameter is not a string" );
                }
            }
            else
            {
                // There is no method specified in the messaging.
                throw new Exception( "missing method" );
            }

            return method;
        }

        /// <summary>
        ///     Assumes the presence of 'args' in the message, and extracts it. If not present or non-Dictionary<string, object>, an Exception will be thrown
        /// </summary>
        /// <param name="genericObject">The message originating from the client</param>
        /// <returns>The found arguments</returns>
        private Dictionary<string, object> ExtractArgs( Dictionary<string, object> genericObject )
        {
            Dictionary<string, object> arguments = null;
            if( genericObject.ContainsKey( "args" ) )
            {
                try
                {
                    arguments = (Dictionary<string, object>) genericObject["args"];
                }
                catch
                {
                    // There is something in the args field, but it's not a Dictionary<string, object>
                    throw new Exception( "args parameter is not a Dictionary<string, object>" );
                }
            }
            else
            {
                // There are no args specified in the messaging.
                throw new Exception( "missing args" );
            }

            return arguments;
        }

        /// <summary>
        ///     Assumes the presence of 'data' in the message, and extracts it. Can be not present
        /// </summary>
        /// <param name="genericObject">The message originating from the client</param>
        /// <returns>The found object, otherwise null</returns>
        private object ExtractData( Dictionary<string, object> genericObject )
        {
            object returnData = null;
            if( genericObject.ContainsKey( "data" ) )
            {
                returnData =  genericObject["args"];
            }

            return returnData;
        }
    }
}
