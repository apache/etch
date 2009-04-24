# package 'etch/bindings/ruby/msg'

# A TaggedDataInputStream reads type tagged values from an input stream.
module TaggedDataInput

  # Starts reading a message from the stream.
  # @return the message that we are reading. 
  #
	def startMessage( ms ) # returns Message
		raise "subclasser responsibility"
	end
	
  # Ends a message that we are reading.
  # @param msg the message that has been read.
  #
	def endMessage( msg )
		raise "subclasser responsibility"
	end
	
  # Starts reading a struct from the stream.
  # @return the struct that we are reading.
  #
	def startStruct # returns StructValue
		raise "subclasser responsibility"
	end
	
  # Reads the next key and value in the sequence of struct
  # elements by stuffing the fields of the passed in StructElement.
  # @param se place to store the read key and value.
  # @return true if a key and value were read, false if there aren't any more
  # key / value pairs.
  #
	def readStructElement( se ) # returns boolean
		raise "subclasser responsibility"
	end
	
  # Ends a struct that we are reading.
  # @param struct the struct that we read.
  #
	def endStruct( struct )
		raise "subclasser responsibility"
	end
	
  # Starts reading an array from the stream.
  # @return the array that we are reading.
  #
	def startArray # returns ArrayValue
		raise "subclasser responsibility"
	end
	
  # Reads the next value in the sequence of array elements by
  # stuffing the fields of the passed in ArrayElement.
  # @param ae place to store the read value.
  # @return true if a value was read, false if there aren't any more.
  #
	def readArrayElement( ae ) # returns boolean
		raise "subclasser responsibility"
	end
	
  # Ends an array that we are reading.
  # @param array the array that we read.
  #
	def endArray( array )
		raise "subclasser responsibility"
	end
	
  # Closes and discards any underlying resources.
	def close()
		raise "subclasser responsibility"
	end
end
