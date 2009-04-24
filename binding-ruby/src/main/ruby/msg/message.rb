# package 'etch/bindings/ruby/msg'

require 'etch/bindings/ruby/msg/struct_value'

# A message is modeled as a command and some argments. 
# The command is an arbitrary integer value, and the 
# arguments are key / value pairs, where the key is an 
# arbitrary integer value and the value is any one of the
# standard java objects, an ArrayValue, a StructValue, 
# or any type which may be serialized by the ValueFactory.

class Message < StructValue
  attr :vf
  
  # Constructs the Message.
  # @param vf the value factory
  # 
  def initialize( stype, vf )
    super( stype )
    @vf = vf
  end
  
  def vf()
    return @vf
  end
  
  # Reads a Message from the tagged data input.
  # @param tdi the tagged data input to read from.
  # @return a Message read from the tagged data input.
  
  def self.read( tdi )
    msg = tdi.startMessage()
    msg.readKeysAndValues( tdi )
    tdi.endMessage( msg )
    return msg
  end
  
  # Writes a message to the tagged data output
  # @param tdo the tagged data output to write to.
  # @throws IOException if there is a problem with the tagged data output.
  
  def writeMessage( tdo )
    tdo.startMessage( self )
    writeKeysAndValues( tdo )
    tdo.endMessage( self )
  end
  
  # Creates a message which is a reply to the current message.
  # The current message's value factory is copied to the new
  # message. The message-id of the current message (if any) is
  # copied into the in-reply-to field of the new message.
  # @param rType the type of the reply.
  # @return a reply message.
  #
  def reply( rType ) # return Message
    rmsg = Message.new( rType, @vf)
    rmsg.setInReplyTo( getMessageId() )
    return rmsg
  end
  
  
  # @return the connection specific unique identifier of this
  # message, or null if there was no such identifier.
  def getMessageId()
    return @vf.getMessageId( self )
  end
  
  # Sets the message-id field of this message.
  # @param msgid the connection specific unique identifier of this
  # message. Null if the message has not been sent yet. 
  # NOTE: the send process overwrites any value the user might set 
  # here. So don't bother trying to set it.
  def setMessageId( msgid )
    vf.setMessageId( self, msgid )
  end
  
  # @return the message-id of the message that this is a response to.
  # Null if this is an original message or if the original message did
  # not have a message-id.
  def getInReplyTo()
    return vf.getInReplyTo( self )
  end
  
  # Sets the in-reply-to field of this message.
  # @param msgid the message-id of the message that this is a response to.
  def setInReplyTo( msgid )
    vf.setInReplyTo( self, msgid )
  end
end
