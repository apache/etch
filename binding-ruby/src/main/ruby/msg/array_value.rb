# package 'etch/bindings/ruby/msg'

class ArrayValue < Array
	
  # Adds many values to the array value
  # param -> values = set of values as varargs
  
  def addMany( *values )
    values.each { |value| self << value }
  end
  
  # Constructs a new array value and adds it to the end of 
  # this array value.
  # returns -> newly constructed array value
  
  def addArrayValue
		av = ArrayValue.new
		self << av
		return av
	end
	
  # Constructs a new struct value and adds it to the end of 
  # this array value.
  # param -> type = the type of the structure
  # returns -> a newly constructed structvalue
  
	def addStructValue( type )
		sv = StructValue.new( type )
		self << sv
		return sv
	end
	
  # Reads an array and all of its values.
  # param -> tdi = the tagged data input to read from.
  # return -> an array value read from the tagged data input.
  
	def self.read( tdi )
		array = tdi.startArray
		array.readValues( tdi )
		tdi.endArray( array )
		return array
	end
	
  # Writes an array and all of its values.
  # param -> tdo the tagged data output to write to.
  
	def write( tdo )
		tdo.startArray( self )
		writeValues( tdo )
		tdo.endArray( self )
	end
	
  # Reads the values of the array. Does not clear the array first.
  # param -> tdi = the tagged data input to read from.
  
	def readValues( tdi )
		ae = ArrayElement.new
		while tdi.readArrayElement( ae )
			self << ae.value
		end
	end
	
  # Writes the values of the array.
  # param -> tdo = the tagged data output to write to.
	def writeValues( tdo )
		each {|value| tdo.writeArrayElement( value )}
	end
end
