# package 'etch/bindings/ruby/msg'

# Interface which defines the value factory which helps
# the idl compiler serialize and deserialize messages,
# convert values, etc.
#
module ValueFactory

  # Adds a type to the set of types.
  # 
  # @param type a type to add.
  # 
  # @return the argument. If there is a collision with
  # an id and name, both associated with the same type,
  # then that type is returned instead of the argument.
  #
  def addType( type )
    raise "subclasser responsibility"
  end

  # Translates a type id into the appropriate Type
  # object.
  # @param id a type id.
  # @return id translated into the appropriate Type.
  #
	def getType( xid )
		raise "subclasser responsibility"
	end
	
  # @return a collection of all the types.
  def getTypes()
    raise "subclasser responsibility"
  end
  
  # Adds a field to the set of fields.
  # 
  # @param field a field to add.
  # 
  # @return the argument. If there is a collision with
  # an id and name, both associated with the same field,
  # then that field is returned instead of the argument. 
  #
  def addField( field )
    raise "subclasser responsibility"
  end
  
  # Translates a field id into the appropriate Field object.
  # @param id a field id.
  #
	def getField( xid )
		raise "subclasser responsibility"
	end
	
  # Get a collection of fields
  def getFields()
    raise "subclasser responsibility"
  end
  
  # @return the encoding to use for strings
	def getStringEncoding
		raise "subclasser responsibility"
	end
	
  # @param msg the message whose well-known message-id field is to be
  # returned.
  # @return the value of the well-known message-id field. This is a
  # unique identifier for this message on a particular transport
  # during a particular session. If there is no well-known message-id
  # field defined, or if the message-id field has not been set, then
  # return null.
	def getMessageId( msg )
		raise "subclasser responsibility"
	end
	
  # Sets the value of the well-known message-id field. This is a
  # unique identifier for this message on a particular transport
  # during a particular session. If there is no well-known message-id
  # field defined then nothing is done. If msgid is null, then the
  # field is cleared.
  # @param msg the message whose well-known message-id field is to
  # be set.
  # @param msgid the value of the well-known message-id field.
  #
	def setMessageId( msg, msgid )
		raise "subclasser responsibility"
	end
	
  # @param msg the message whose well-known in-reply-to field is to
  # be returned.
  # @return the value of the in-reply-to field, or null if there is
  # none or if there is no such field defined.
  #
	def getInReplyTo( msg )
		raise "subclasser responsibility"
	end
	
  # @param msg the message whose well-known in-reply-to field is to
  # be set.
  # @param msgid the value of the well-known in-reply-to field. If
  # there is no well-known in-reply-to field defined then nothing
  # is done. If msgid is null, then the field is cleared.
  #
	def setInReplyTo( msg, msgid )
		raise "subclasser responsibility"
	end
	
  # Converts a value to a struct value representation to be exported
  # to a tagged data output.
  # @param value a custom type defined by a service, or a well-known
  # standard type (e.g., date).
  # @return a struct value representing the value.
  #
	def exportCustomValue( value )
		raise "subclasser responsibility"
	end
	
  # Converts a struct value imported from a tagged data input to
  # a normal type.
  # @param sv a struct value representation of a custom type, or a
  # well known standard type.
  # @return a custom type, or a well known standard type.
  #
	def importCustomValue( sv )
		raise "subclasser responsibility"
	end
	
  # @param c the class of a custom value.
  # @return the struct type of a custom value class.
  #
  def getCustomStructType( type )
    raise "subclasser responsibility"
  end
  
  # @param type the type of a custom value
  # @return the class of a custom struct value type.
  #
  def getCustomType( type )
    raise "subclasser responsibility"
  end
  
end
