require 'etch/bindings/ruby/msg/array_value.rb'
require 'etch/bindings/ruby/msg/field.rb'
require 'etch/bindings/ruby/msg/message.rb'
require 'etch/bindings/ruby/msg/struct_value.rb'
require 'etch/bindings/ruby/msg/tagged_data_output.rb'
require 'etch/bindings/ruby/msg/type.rb'
require 'etch/bindings/ruby/msg/value_factory.rb'
require 'etch/bindings/ruby/transport/fmt/type_code.rb'
# require 'socket'

class BinaryTaggedDataOutput < BinaryTaggedData
              include TaggedDataOutput
        
     attr :os
     private :os
     
     def initialize( os, vf )
        super( vf )
        @os = os
     end
     
     def startMessage( msg )
        writeByte( VERSION )
        writeStructType( msg.type() )
     end
     
     def endMessage( msg )
        writeValue( BinaryTaggedData.NONE )
     end
     
     def startStruct( struct )
        writeStructType( struct.type() )
     end
     
     def writeStructElement( key, value )
        key = Field.new
        writeValue( key.getId() )
        writeValue( value )
     end
     
     def endStruct( struct )
          writeValue( BinaryTaggedData.NONE )
     end
     
     def startArray( array )
#    // the caller has already written a type code to indicate an
#    // array is starting.
            array = ArrayValue .new
            byte type = array.typeCode()
            writeType( type )
            if (type == TypeCode.CUSTOM)
              writeStructType( array.customStructType() )
            end
            writeValue( array.dim() )
     end
     
     def writeStructType( type )
          type = Type.new
          writeValue( type.getId() )
     end
     
     def writeArrayElement( value )
          writeValue( value )
     end
     
     def endArray( array )
          writeValue( BinaryTaggedData.NONE )
     end
     
     def close()
        @os.close()
     end
     
     def writeType( type )
        writeByte( type )
     end
     
     def writeValue( value )
         byte type = checkValue( value )
         writeType( type )

         case(type)
            when TypeCode.NULL
            when TypeCode.BOOLEAN_FALSE
            when TypeCode.BOOLEAN_TRUE
            when TypeCode.EMPTY_STRING
            when TypeCode.NONE
              return
      
            when TypeCode.BYTE1
            when TypeCode.SHORT1
            when TypeCode.INT1
            when TypeCode.LONG1
              writeByte( value.byteValue() )
              return
            
            when TypeCode.SHORT2
            when TypeCode.INT2
            when TypeCode.LONG2
              writeShort( value.shortValue() )
              return
      
            when TypeCode.INT4
            when TypeCode.LONG4
              writeInt(value.intValue() )
              return
            
            when TypeCode.LONG8
              writeLong( value.longValue() )
              return
            
            when TypeCode.FLOAT4
              writeFloat(value.floatValue() )
              return
            
            when TypeCode.FLOAT8
              writeDouble( value.doubleValue() )
              return
            
            when TypeCode.BYTES
              writeBytes(value)
              return
            
            when TypeCode.STRING
              writeBytes( value.getBytes( @vf.getStringEncoding() ) )
              return
            
            when TypeCode.STRUCT
              (value.writeStruct( this ))
              return
            
            when TypeCode.ARRAY
              if (value.class == ArrayValue)
                (value.write( this ))
              else
                toArrayValue( value ).write( this )
              end
              return
              
            when TypeCode.CUSTOM
              struct = @vf.exportCustomValue( value )
              
              if (struct == null)
                raise "UnsupportedOperationException"
              end
              
              struct.writeStruct( this )
              return
          end
#        // type is either "small" integer or unused...
          return
       end
       
       def writeByte( value )
            @os.write( value )
       end
       
       def writeShort( value )
          writeByte(value )
          writeByte( (value >> 8) )
       end
       
       def writeInt( value )
          writeShort( value )
          writeShort((value >> 16) )
       end
       
       def writeLong( value )
            writeInt( value );
            writeInt( (value >> 32) )
       end
       
       def writeFloat( value )
           writeInt( Float.floatToIntBits( value ) )
       end
       
       def writeDouble( value )
           writeLong( Double.doubleToLongBits( value ) )
       end
       
       def writeBytes( value = Array.new )
          n = value.length;
          writeValue( n );
          value.each { |value| writeByte( value ) }
       end
       
       def getBytes( msg, vf )
           baos = ByteArrayOutputStream.new();
           tdo = BinaryTaggedDataOutput.new( baos, vf );
           msg.writeMessage( tdo );
           tdo.close();
           return baos.toByteArray()
       end
       
       def setOutputStream( os )
          @os = os
       end
end