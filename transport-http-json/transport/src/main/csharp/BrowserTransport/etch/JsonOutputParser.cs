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
using Etch.Support;

namespace Etch.Transport
{
    /// <summary> A message conversion class, that specializes is converting from JSON to Etch Message format </summary>
    public class JsonOutputParser
    {
        #region Type Codes
        private static int boolCode = typeof( bool ).GetHashCode();
        private static int stringCode = typeof( string ).GetHashCode();
        private static int shortCode = typeof(short).GetHashCode();
        private static int sbyteCode = typeof( SByte ).GetHashCode();
        private static int intCode = typeof( int ).GetHashCode();
        private static int longCode = typeof( long ).GetHashCode();
        private static int floatCode = typeof( float ).GetHashCode();
        private static int doubleCode = typeof( double ).GetHashCode();
        private static int arrayCode = typeof( Array ).GetHashCode();
        private static int datetimeCode = typeof( DateTime ).GetHashCode();
        private static int listCode = typeof( System.Collections.IList ).GetHashCode();
        private static int mapCode = typeof( System.Collections.IDictionary ).GetHashCode();
        private static int setCode = typeof( System.Collections.IDictionary ).GetHashCode();
        #endregion

        private DefaultValueFactory valueFactory;

        public JsonOutputParser( DefaultValueFactory valueFactory )
        {
            this.valueFactory = valueFactory;
        }

        /// <summary>
        /// Parses an Etch message, and converts it into a Dictionary<string, object> which is parseable by the JSON utilities
        /// This method is responsible for sending either a new message, or a response to an existing message (normal response or exception)
        /// </summary>
        /// <returns>A constructed message read from the JSON reader</returns>
        public Dictionary<string, object> WriteNewMessage(Message etchMessage)
        {
            Dictionary<string, object> jsonMessage = new Dictionary<string, object>();

            // a bit scary to pass longs to Javascript client.  
            // One shouldn't be doing math with this ID anyway--so here we ToString() the MessageId
            jsonMessage["transactionId"] = etchMessage.MessageId.ToString(); 

            XType messageType = etchMessage.GetXType;

            // set the method name
            jsonMessage["method"] = messageType.Name;

            jsonMessage["data"] = ParseMessageData( etchMessage );

            return jsonMessage;
        }

        private Dictionary<string, object> ParseMessageData( StructValue etchMessage)
        {
            Dictionary<string, object> data = new Dictionary<string, object>();

            XType messageType = etchMessage.GetXType;

            foreach( KeyValuePair<Field, Object> valueField in etchMessage )
            {
                Field field = valueField.Key;

                if( field != Etch.Support.DefaultValueFactory._mf__messageId )
                {
                    Validator fieldValidator = messageType.GetValidator( field );

                    data[field.Name] = ParseFieldValue( fieldValidator, valueField.Value);
                }
            }

            return data;
        }

        private object ParseFieldValue( Validator fieldValidator, object fieldValue )
        {
            bool isException;
            return ParseFieldValue(fieldValidator, fieldValue, out isException );
        }
        private object ParseFieldValue(Validator fieldValidator, object fieldValue, out bool isExceptionType)
        {
            object parsedValue = null;
            isExceptionType = false;

            // Quick check against fieldValue for null.  
            if(fieldValue == null)
            {
                return parsedValue;
            }

            Type type = fieldValue.GetType();

            int typeCode = type.GetHashCode();

            if( typeCode == boolCode || typeCode == shortCode || typeCode == sbyteCode ||  
                typeCode == intCode || typeCode == longCode || typeCode == floatCode || 
                typeCode == doubleCode || typeCode == stringCode || typeCode == datetimeCode)
            {
                parsedValue = fieldValidator.ValidateValue(fieldValue);
            }
            else if( IsList(fieldValidator)) // must come before IsArray check below
            {   // TODO: determine if this is the best way to determine that this is a List etch type?
                parsedValue = ParseList( fieldValidator, fieldValue );
            }
            else if( type.IsArray )
            {
                parsedValue = ParseArray( fieldValidator, fieldValue );
            }
            else // is an object with interesting field and type information
            {
                StructValue etchDataWrapper = valueFactory.ExportCustomValue( fieldValue );
                if( etchDataWrapper == null )
                    throw new Exception( "unsupported type " + fieldValue.GetType() );
                
                parsedValue = ParseStruct( etchDataWrapper );

                // also indicate if the type is an exception

                if( fieldValue is StructValue )
                {
                    StructValue structValue = fieldValue as StructValue;
                    Type structValueType = structValue.GetXType.GetComponentType();
                    if( structValueType != null)
                    {
                        isExceptionType = typeof( Exception ).IsAssignableFrom( structValueType );
                    }
                }
                else
                {
                    isExceptionType = typeof( Exception ).IsAssignableFrom( type );
                }
            }
           
            return parsedValue;
        }

        private bool IsExceptionType(Validator fieldValidator)
        {
            bool foundException = false;

            ComboValidator validator = fieldValidator as ComboValidator;
            if( validator != null )
            {
                Console.WriteLine( "IsComboValidator" );
                Validator_custom customValidator = validator.A() as Validator_custom;
                if( customValidator != null )
                {
                    Console.WriteLine( "A() is CustomValidator: " + customValidator.GetExpectedClass() );

                    foundException = typeof( Exception ).IsAssignableFrom( customValidator.GetExpectedClass() );
                    Console.WriteLine( "foundException: " + foundException );
                }

                if( !foundException )
                {

                    customValidator = validator.B() as Validator_custom;
                    if( customValidator != null )
                    {
                        Console.WriteLine( "B() is CustomValidator: " + customValidator.GetExpectedClass() );

                        foundException = typeof( Exception ).IsAssignableFrom( customValidator.GetExpectedClass() );
                        Console.WriteLine( "foundException: " + foundException );

                    }
                }
            }

            if( !foundException )
            {

                Console.WriteLine( "Validator is not comboValidator, checking if is CustomValidator");
                Validator_custom customValidator = fieldValidator as Validator_custom;
                if( customValidator != null )
                {

                    Console.WriteLine( "Field is CustomValidator: " + customValidator.GetExpectedClass() );
                    foundException = typeof( Exception ).IsAssignableFrom( customValidator.GetExpectedClass() );
                    Console.WriteLine( "foundException: " + foundException );

                }
            }

            return foundException;
        }

        /// <summary>
        ///  TODO better way to do this (faster);
        /// </summary>
        /// <param name="fieldValidator"></param>
        /// <returns></returns>
        private bool IsList( Validator fieldValidator )
        {
            bool foundList = false;

            ComboValidator validator = fieldValidator as ComboValidator;
            if(validator != null)
            {
                Validator_custom customValidator = validator.A() as Validator_custom;
                if( customValidator != null )
                {
                    foundList = customValidator.GetExpectedClass().GetHashCode() == listCode;
                }

                if( !foundList )
                {
                    customValidator = validator.B() as Validator_custom;
                    if( customValidator != null )
                    {
                        foundList = customValidator.GetExpectedClass().GetHashCode() == listCode;
                    }
                }
            }

            if( !foundList )
            {
                Validator_custom customValidator = fieldValidator as Validator_custom;
                if( customValidator != null )
                {
                    foundList = customValidator.GetExpectedClass().GetHashCode() == listCode;
                }
            }

            return foundList;
          
        }

        private Dictionary<string, object> ParseStruct( StructValue etchDataWrapper )
        {
            Dictionary<string, object> data = new Dictionary<string, object>();

            XType messageType = etchDataWrapper.GetXType;

            // slip in a type indicator into the struct.
            data["@type"] = messageType.Name;

            if( messageType == valueFactory.Get_mt__Etch_Map())
            {
                //TODO
            }

            foreach( KeyValuePair<Field, Object> valueField in etchDataWrapper )
            {
                Field field = valueField.Key;

                Validator fieldValidator = messageType.GetValidator( field );

                data[field.Name] = ParseFieldValue( fieldValidator, valueField.Value );
            }

            return data;
        }

        private Array ParseList( Validator fieldValidator, object value )
        {
             value = fieldValidator.ValidateValue( value );

            // TODO: the list validator should allow retrieval of this as 0-dimension object validator
             return ParseArray( Validator_object.Get( 1 ), value );
        }

        private Array ParseArray(Validator fieldValidator, object value)
        {
            Type type = value.GetType();
            int dim = 0;

            while( type.IsArray )
            {
                dim++;
                type = type.GetElementType();
            }

            // now we want the type code for c, and if the type code is custom,
            // we'll also want the struct type code.

            int typeCode = type.GetHashCode();

            Array array = value as Array;

            Array newArray = Array.CreateInstance( typeof( object ), array.Length );
            for( int i = 0; i < array.Length; i++ )
            {
                // we have in our hands a C# object. We need to prepare it for JsonFX-ification
                object index = array.GetValue( i );
   
                Validator elementValidator = fieldValidator.ElementValidator();

                // send it back through the parsing logic
                newArray.SetValue( ParseFieldValue( elementValidator, index ), i );
            }

            return newArray;
        }

        public Dictionary<string, object> WriteReplyMessage( Message etchMessage )
        {
            Dictionary<string, object> jsonMessage = new Dictionary<string, object>();

            // a bit scary to pass longs to Javascript client.  
            // One shouldn't be doing math with this ID anyway--so here we ToString() the InReplyTo
            jsonMessage["transactionId"] = etchMessage.InReplyTo.ToString();

            XType messageType = etchMessage.GetXType;

            Validator resultValidator = etchMessage.GetXType.GetValidator(Etch.Support.DefaultValueFactory._mf_result);

            object data = null;

            bool isExceptionType = false;
            if(etchMessage.ContainsKey(Etch.Support.DefaultValueFactory._mf_result))
            {
                data = ParseFieldValue(resultValidator, etchMessage[Etch.Support.DefaultValueFactory._mf_result], out isExceptionType ); 
            }


            if(isExceptionType)
            {
                // errorData indicates exception
                jsonMessage["errorData"] = data;
            }
            else
            {
                // data indicates not-fault
                jsonMessage["data"] = data;
            }

            return jsonMessage;

        }
    }
}
