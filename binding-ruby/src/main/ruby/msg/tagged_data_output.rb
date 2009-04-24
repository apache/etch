# package 'etch/bindings/ruby/msg'

# A TaggedDataOutputStream writes type tagged data values to 
# an output stream.
#
module TaggedDataOutput
	
  # Writes the beginning of message data. A message is also a struct,
  # and so the type of the struct is written. The fields and
  # values of the struct should be written using 
  # writeStructElement(key, value)
  # @param msg the message being written out.
  #
  def startMessage( msg )
		raise "subclasser responsibility"
	end
	
  # Writes the end of message data.
  # @param msg the message that was written. 
  #
	def endMessage ( msg )
		raise "subclasser responsibility"
	end
	
  # Writes the beginning of struct data. The type of the struct is
  # written. The fields and values of the struct are written
  # using writeStructElement(key, value).
  # @param struct the struct being written out.
  #
	def startStruct( struct )
		raise "subclasser responsibility"
	end
	
  # Writes a struct element with the specified key and value.
  # @param key
  # @param value
  #
	def writeStructElement( key, value )
		raise "subclasser responsibility"
	end
	
  # Writes the end of struct data.
  # @param struct the struct that was written.
  #
	def endStruct( struct )
		raise "subclasser responsibility"
	end
	
  # Writes the beginning of array data. The values of the array
  # should be written using writeArrayElement(value)
  # @param array the array to be written.
  #
	def startArray( array )
		raise "subclasser responsibility"
	end
	
  # Writes an array element with the specified value.
  # @param value
  #
	def writeArrayElement( value )
		raise "subclasser responsibility"
	end
	
  # Writes the end of array data.
  # @param array the array that was written.
  #
	def endArray( array )
		raise "subclasser responsibility"
	end
	
  # Closes and releases any resources.
  #
	def close
		raise "subclasser responsibility"
	end
end
