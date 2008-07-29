using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using System.IO;
using Etch.Support;
using Etch.Msg;
using System.Reflection;

namespace Etch.Xml
{
    /// <summary>
    /// ValueFactory extracted from the XML file of an Etch file
    /// </summary>
    public class DynamicValueFactory : DefaultValueFactory
    {
        static module binding;
        public static string XMLPATH = "../../Test.xml";

        static DynamicValueFactory()
        {
            DefaultValueFactory.Init(types, fields, class2type );

            LoadXml( XMLPATH );
            CreateMapForValidators();

            LoadMethods();
            LoadEnums();
            LoadStructs();
            LoadExceptions();

        }

        // Define the type and field maps
        public readonly static TypeMap types = new TypeMap();
        public readonly static FieldMap fields = new FieldMap();
        private readonly static Class2TypeMap class2type = 
                new Class2TypeMap();

        public static void LoadXml( string path )
        {
            binding = new module();
            // load the xml binding
            XmlSerializer seri = new XmlSerializer( typeof( module ) );
            using ( XmlTextReader reader = new XmlTextReader( File.OpenRead( path ) ) )
            {
                binding = seri.Deserialize( reader ) as module;
            }
        }

        /// <summary>
        /// Map tracking < data type => 'which validator to add / validator code to run' >
        /// </summary>
        public static Dictionary<string, ValidatorDelegate> typeValidatorMap = new Dictionary<string, ValidatorDelegate>();

        /// <summary>
        /// Delegate needed to be run depending upon the type
        /// </summary>
        /// <param name="currentType">type needing the validator</param>
        /// <param name="currentField">field needing the validator</param>
        /// <param name="dims">greater than 0, if array type</param>
        public delegate void ValidatorDelegate( XType currentType, Field currentField, int dims );

        /// <summary>
        /// Populate the map of <type (string), validator code to be invoked>
        /// </summary>
        /// <param name="currentType">type under consideration</param>
        /// <param name="currentField">field under consideration</param>
        /// <param name="dims">number of dimensions of field</param>
        public static void CreateMapForValidators()
        {
            typeValidatorMap.Add( "byte",
                delegate( XType currentType, Field currentField, int dims )
                {
                    currentType.PutValidator( currentField, Validator_byte.Get( dims ) );
                }
            );

            typeValidatorMap.Add( "int",
                delegate( XType currentType, Field currentField, int dims )
                {
                                currentType.PutValidator( currentField, Validator_int.Get( dims ) );
                }
            );
            typeValidatorMap.Add( "short",
                delegate( XType currentType, Field currentField, int dims )
                {
                    currentType.PutValidator( currentField, Validator_short.Get( dims ) );
                }
            );
            typeValidatorMap.Add( "float",
                delegate( XType currentType, Field currentField, int dims )
                {
                    currentType.PutValidator( currentField, Validator_float.Get( dims ) );
                }
            );
            typeValidatorMap.Add( "double",
                delegate( XType currentType, Field currentField, int dims )
                {
                    currentType.PutValidator( currentField, Validator_double.Get( dims ) );
                }
            );
            typeValidatorMap.Add( "string",
                delegate( XType currentType, Field currentField, int dims )
                {
                    currentType.PutValidator( currentField, Validator_string.Get( dims ) );
                }
            );
            typeValidatorMap.Add( "long",
                delegate( XType currentType, Field currentField, int dims )
                {
                    currentType.PutValidator( currentField, Validator_long.Get( dims ) );
                }
            );
            typeValidatorMap.Add( "boolean",
                delegate( XType currentType, Field currentField, int dims )
                {
                    currentType.PutValidator( currentField, Validator_boolean.Get( dims ) );
                }
            );
            typeValidatorMap.Add( "void",
                            delegate( XType currentType, Field currentField, int dims )
                            {
                                currentType.PutValidator( currentField, Validator_void.Get( dims ) );
                            }
                        );
            typeValidatorMap.Add( "object",
                delegate( XType currentType, Field currentField, int dims )
                {
                    currentType.PutValidator( currentField, Validator_object.Get( dims ) );
                }
            );
        }

        /// <summary>
        /// Loads all the methods into the valuefactory
        /// </summary>
        public static void LoadMethods()
        {
            for ( int serviceCount = 0;
                serviceCount < binding.service.Length; serviceCount++ )
            {
                moduleService myService = binding.service[ serviceCount ];

                for ( int i = 0; i < myService.methods.Length; i++ )
                {
                    moduleServiceMethodsMethod method = myService.methods[ i ];

                    // First add methodname
                    XType currentType = types.Get( method.typeName );
                    XType resultType = null;

                    // Console.WriteLine( "adding method = " + method.typeName );

                    bool isOneWay = Convert.ToBoolean( method.isOneway );
                    // Then add it's result
                    if ( !isOneWay )
                        resultType = types.Get( method.result[ 0 ].fieldName );

                    // Set result type
                    if ( !isOneWay )
                        currentType.SetResult( resultType );

                    // Add validators

                    currentType.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                    if ( method.field != null )
                    {
                        // iterate through fields
                        for ( int j = 0; j < method.field.Length; j++ )
                        {
                            field field = method.field[ j ];
                            Field currentField = fields.GetByName( field.fieldName );

                            // Check if field already doesn't exist
                            if ( currentField == null )
                            {
                                // add to fields ... 
                                currentField = fields.Get( field.fieldName );
                                // Console.WriteLine( "adding field = " + field.fieldName );
                            }

                            // Validators

                            int dims = Convert.ToInt32( field.dimension );

                            try
                            {
                                typeValidatorMap[ field.type ]( currentType, currentField, dims );
                            }
                            catch ( KeyNotFoundException )
                            {
                                Console.WriteLine( "custom type serialization not supported" );
                            }

                            // TODO: Validators for exceptions

                        }
                    }
                    // Validators for the return message
                    if ( !isOneWay )
                    {
                        // return type
                        int dims = Convert.ToInt32( method.result[ 0 ].dimension );
                        Field currentField = fields.GetByName( "result" );

                        typeValidatorMap[ method.result[ 0 ].type ]( resultType, currentField, dims );

                        resultType.PutValidator( fields.GetByName( "_messageId" ), Validator_long.Get( 0 ) );
                        resultType.PutValidator( fields.GetByName( "result" ), Validator_RuntimeException.Get() );
                        resultType.PutValidator( fields.GetByName( "_inReplyTo" ), Validator_long.Get( 0 ) );
                    }
                }
            }
        }

        
        /// <summary>
        /// Load all enums in the service
        /// </summary>
        public static void LoadEnums()
        {
            for ( int serviceCount = 0;
                serviceCount < binding.service.Length; serviceCount++ )
            {
                moduleService myService = binding.service[ serviceCount ];

                for ( int i = 0; i < myService.enums.Length; i++ )
                {
                    moduleServiceEnumsEnum myEnum = myService.enums[ i ];

                    // First add methodname
                    XType currentType = types.Get( myEnum.typeName );

                    //Console.WriteLine( "adding enum = " + myEnum.typeName );

                    // Add validators

                    if ( myEnum.entry != null )
                    {
                        // iterate through fields
                        for ( int j = 0; j < myEnum.entry.Length; j++ )
                        {
                            moduleServiceEnumsEnumEntry enumEntry = myEnum.entry[ j ];
                            Field currentField = fields.GetByName( enumEntry.value );

                            // Check if field already doesn't exist
                            if ( currentField == null )
                            {
                                // add to fields ... 
                                currentField = fields.Get( enumEntry.value );
                                //Console.WriteLine( "adding enumEntry = " + enumEntry.value );
                            }

                            // Validators
                            currentType.PutValidator( currentField, Validator_boolean.Get( 0 ) );

                        }
                    }
                }
            }
        }

        /// <summary>
        /// Load all the structs in the service
        /// </summary>
        public static void LoadStructs()
        {
            for ( int serviceCount = 0;
                serviceCount < binding.service.Length; serviceCount++ )
            {
                moduleService myService = binding.service[ serviceCount ];

                for ( int i = 0; i < myService.structs.Length; i++ )
                {
                    moduleServiceStructsStruct myStruct = myService.structs[ i ];

                    // First add methodname
                    XType currentType = types.Get( myStruct.typeName );

                    //Console.WriteLine( "adding enum = " + myStruct.typeName );

                    // Add validators

                    if ( myStruct.field != null )
                    {
                        // iterate through fields
                        for ( int j = 0; j < myStruct.field.Length; j++ )
                        {
                            field structField = myStruct.field[ j ];
                            Field currentField = fields.GetByName( structField.fieldName );

                            // Check if field already doesn't exist
                            if ( currentField == null )
                            {
                                // add to fields ... 
                                currentField = fields.Get( structField.fieldName );
                                //Console.WriteLine( "adding structEntry = " + structField.fieldName );
                            }

                            // Validators

                            int dims = Convert.ToInt32( structField.dimension );

                            try
                            {
                                typeValidatorMap[ structField.type ]( currentType, currentField, dims );
                            }
                            catch ( KeyNotFoundException )
                            {
                                // TODO
                                Console.WriteLine( "custom type serialization not supported as yet" );
                            }
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Load all the exceptions in the service
        /// </summary>
        public static void LoadExceptions()
        {
            for ( int serviceCount = 0;
                serviceCount < binding.service.Length; serviceCount++ )
            {
                moduleService myService = binding.service[ serviceCount ];

                for ( int i = 0; i < myService.exceptions.Length; i++ )
                {
                    exception myException = myService.exceptions[ i ];

                    // First add methodname
                    XType currentType = types.Get( myException.typeName );

                    //Console.WriteLine( "adding enum = " + myException.typeName );

                    // Add validators

                    if ( myException.field != null )
                    {
                        // iterate through fields
                        for ( int j = 0; j < myException.field.Length; j++ )
                        {
                            field exceptionField = myException.field[ j ];
                            Field currentField = fields.GetByName( exceptionField.fieldName );

                            // Check if field already doesn't exist
                            if ( currentField == null )
                            {
                                // add to fields ... 
                                currentField = fields.Get( exceptionField.fieldName );
                                //Console.WriteLine( "adding exceptionEntry = " + exceptionField.fieldName );
                            }

                            // Validators

                            int dims = Convert.ToInt32( exceptionField.dimension );

                            try
                            {
                                typeValidatorMap[ exceptionField.type ]( currentType, currentField, dims );
                            }
                            catch ( KeyNotFoundException )
                            {
                                // TODO
                                Console.WriteLine( "custom type serialization not supported as yet" );
                            }

                        }
                    }
                }
            }
        }

        public override StructValue ExportCustomValue( object value )
        {
            return ExportCustomValue( this, class2type, value );
        }

        public override XType GetCustomStructType( Type t )
        {
            return class2type.Get( t );
        }

        public override Field GetField( string name )
        {
            return fields.Get( name );
        }

        public override Field GetField( int? id )
        {
            return fields.Get( id );
        }

        public override ICollection<Field> GetFields()
        {
            return fields.Values();
        }

        public override XType GetType( string name )
        {
            return types.Get( name );
        }

        public override XType GetType( int? id )
        {
            return types.Get( id );
        }

        public override ICollection<XType> GetTypes()
        {
            return types.Values();
        }
    }
}
