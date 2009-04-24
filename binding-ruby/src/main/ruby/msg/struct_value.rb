# package 'etch/bindings/ruby/msg'

require 'etch/bindings/ruby/msg/struct_element'

# A typed map of key/value pairs, where the type is a Type,
# each key is a Field, and each value is of arbitrary type
# chosen from the basic java types boolean, byte, short, int, long,
# float, double, String, an array of those, the extended types
# ArrayValue and StructValue, and specific types supported by
# ValueFactory.
 
# StructValue is not protected against concurrent access.
#
class StructValue < Hash
	
  attr :stype
  
  def initialize( stype )
		@stype = stype
	end
	
  def getType()
    return @stype
  end
  
  # Compares the type of this struct to another type.
  # @param otherType the type to compare this type to.
  # @return true if this struct is of the specified type.
	def stype?( otherStype )
		return @stype.eql?( otherStype )
	end
	
  # Checks a struct for having the expected type.
  # @param expectedType the expected type of this struct.
  # @throws IllegalArgumentException if the type is not as expected.
  #
	def checkType( otherStype )
    if !stype?( otherStype )
      raise "type mismatch (got #{stype} wanted #{otherStype})" 
    else
      return true
    end
	end
	
  # Constructs a new array value and puts it into this struct value
  # with the specified key.
  # @param key the key of the newly constructed array value.
  # @return a newly constructed array value.
  #
	def putArrayValue( key )
		av = ArrayValue.new
		store( key, av )
		return av
	end
	
  # Constructs a new struct value and puts it into this struct value
  # with the specified key.
  # @param key the key of the newly constructed struct value.
  # @param sType the type of the structure.
  # @return a newly constructed struct value.
  #
	def putStructValue( key, type )
		sv = StructValue.new( type )
		store( key, sv )
		return sv
	end
	
  # Reads the struct from the tagged data input.
  # @param tdi the tagged data input to read from.
  # @return a struct value read from the tagged data input.
  # 
	def self.read( tdi )
		sv = tdi.startStruct
		sv.readKeysAndValues( tdi )
		tdi.endStruct( sv )
    return sv
	end
	
  # Writes a struct to the tagged data output.
  # @param tdo the tagged data output to write to.
  #
	def writeStruct( tdo )
		tdo.startStruct( self )
		writeKeysAndValues( tdo )
		tdo.endStruct( self )
	end
	
  # Reads the key / value pairs and puts them in the struct.
  # @param tdi the tagged data input to read from.
  #
	def readKeysAndValues( tdi )
    se = StructElement.new
		while tdi.readStructElement( se )
			store( se.key, se.value )
		end
	end
	
  # Writes the key / value pairs.
  # @param tdo the tagged data output to write to.
  #
	def writeKeysAndValues( tdo )
		each{ |key, value| tdo.writeStructElement( key, value ) }
	end
	
	def to_s
		return "#{@stype}: "+to_a.join( ", " )
	end
end
