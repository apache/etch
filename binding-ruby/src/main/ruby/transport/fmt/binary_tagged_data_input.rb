require 'etch/bindings/ruby/msg/array_value.rb'
require 'etch/bindings/ruby/msg/message.rb'
require 'etch/bindings/ruby/msg/struct_value.rb'
require 'etch/bindings/ruby/msg/tagged_data_input.rb'
require 'etch/bindings/ruby/msg/type.rb'
require 'etch/bindings/ruby/msg/value_factory.rb'
require 'etch/bindings/ruby/support/message_source.rb'
require 'etch/bindings/ruby/transport/fmt/type_code.rb'
require 'socket'

class BinaryTaggedDataInput < BinaryTaggedData
        include TaggedDataInput
        
      attr :is, :vf
      private :is, :vf
      
      def initialize( vf, is )
          super( vf )
          @is = is
      end
      
      def setInputStream( is )
          @is = is
      end
      
      def startMessage()
          version = readByte()
          if( version != VERSION )
                raise "IOException"
          end
          
          String.to_str( "binary tagged data version mismatch: got %d expected %d",
          version, VERSION )
          msg = Message.new( readStructType(), vf )
          return msg
      end
      
      def endMessage( msg )
#            // nothing to do.
      end
      
      def startStruct()
          strct = StructValue.new( readStructType())
          return strct
      end
      
      def readStructType()
          id = readValue()
          return @vf.getType( id )
      end
      
      def readStructElement( se )
          obj = readValue()
          if (obj == BinaryTaggedData.NONE)
            return false
          end
          
          id = obj
          se.key = vf.getField( id )
          se.value = readValue()
          return true
      end    
      
      def endStruct( struct )
            #    // nothing to do
      end
      
      def startArray()
          type = readType()
          if (type == TypeCode.CUSTOM)
              customStructType = readStructType()
          else
              customStructType = null
              dim = readValue()
#//    System.out.printf( "startArray (input) %d %s %d\n", type, customStructType, dim )
          end
          arr = ArrayValue.new( type, customStructType, dim )
          return arr    
      end
      
      def readArrayElement( ae )
          ae = ArrayValue.new
          ae.value = readValue()
          if (ae.value == BinaryTaggedData.NONE)
            return false
          end  
          return true
      end
      
      def endArray( array )
#    // nothing to do.
      end
  
      def close()
        @is.close()
      end

#    Reads a Message from the data buffer.
#    @param ms the message source.
#    @param vf the value factor for the message.
#    @param buf the data buffer.
#    @return the message read.
#    @throws IOException if there is a problem reading the message.
#   
      def readMessage( vf, ms, buf = Array.new )
          bais = ByteArrayInputStream.new( buf )
          return readMessage( vf, ms, bais )
      end
      
      def readMessage( vf, off, len, ms, buf = Array.new )
          bais = ByteArrayInputStream.new( buf, off, len )
          return readMessage( vf, bais, ms )
      end
      
      def readMessage( vf, is, ms )
          btdi = BinaryTaggedDataInput.new( vf, is )           
          return Message.read( btdi )
      end
      
      def readByte()
         b = is.read()
#            // is.read() returns 0-255 value or -1 if eof
         if (b < 0)
            raise "EOFException"
         end
         return b
      end
      
      def readUByte()
            b = @is.read()
#    // is.read() returns 0-255 value or -1 if eof
            if (b < 0)
                raise "EOFException"
            end
            return b
      end
      
      def readShort()
          a = readUByte()
          b = readUByte()
          return (a | (b << 8))
      end
      
      def readUShort()
          a = readUByte()
          b = readUByte()
          return a | (b << 8)
      end
      
      def readInt()
          a = readUShort()
          b = readUShort()
          return a | (b << 16)
      end
      
      def readUInt()
          a = readUShort()
          b = readUShort()
          return a | (b << 16)
      end
      
      def readLong()
          long a = readUInt()
          long b = readUInt()
          return a | (b << 32)
      end
      
      # readFloat & readDouble to be done!
      
      def readBytes( b = Array.new )
          n = b.length
          for i in (0..n)
            b[i] = readByte()
          end
      end
      
      def readType()
          return readByte()
      end
      
      def readValue()
          type = readType()
          case(type)
            when( TypeCode.NULL)
              return null
            when( TypeCode.BOOLEAN_FALSE)
              return Boolean.FALSE
            when TypeCode.BOOLEAN_TRUE:
              return Boolean.TRUE     
            when TypeCode.BYTE1:
              return readByte()          
            when TypeCode.SHORT1:
              return readByte()          
            when TypeCode.INT1:
              return readByte()          
            when TypeCode.LONG1:
              return readByte()          
            when TypeCode.SHORT2:
              return readShort()          
            when TypeCode.INT2:
              return readShort()          
            when TypeCode.LONG2:
              return readShort()          
            when TypeCode.INT4:
              return readInt()          
            when TypeCode.LONG4:
              return readInt()          
            when TypeCode.LONG8:
              return readLong()          
            when TypeCode.FLOAT4:
              return readFloat()          
            when TypeCode.FLOAT8:
              return readDouble()            
            when TypeCode.BYTES:
              return readBytes()          
            when TypeCode.EMPTY_STRING:
              return ""          
            when TypeCode.STRING:
              return String.new( readBytes(), @vf.getStringEncoding() )          
            when TypeCode.ARRAY:
              return fromArrayValue( ArrayValue.read( this ) )          
            when TypeCode.STRUCT:
              return StructValue.read( this )          
            when TypeCode.CUSTOM:
              return @vf.importCustomValue( StructValue.read( this ) )          
            when TypeCode.NONE:
              return BinaryTaggedData.NONE
            end  
            if ((type & TypeCode.PSMASK) == TypeCode.PSVALUE)
#            // positive small integers
                value = type & TypeCode.PVMASK
                dt = (type >> TypeCode.PDTSHIFT) & TypeCode.DTMASK
                if (dt == TypeCode.BYTE_DT) 
                    return value
                end
                if (dt == TypeCode.SHORT_DT) 
                  return value
                end
                if (dt == TypeCode.INT_DT) 
                    return value 
                end
                return value
            else if ((type & TypeCode.NSMASK) == TypeCode.NSVALUE)
#            // negative small integers
                value = -(type & TypeCode.NVMASK)-1
                dt = (type >> TypeCode.NDTSHIFT) & TypeCode.DTMASK
                if (dt == TypeCode.BYTE_DT) 
                    return value
                end
                if (dt == TypeCode.SHORT_DT) 
                    return value
                end
                if (dt == TypeCode.INT_DT) 
                    return value
                end
                return value
            end
            raise "UnsupportedOperationException #{type} "
       end
    end
    
    def readBytes()
        n = readValue()
        b = Array.new
        readBytes( b )
        return b
    end
end