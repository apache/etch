# package etch/bindings/ruby/support

module MessageHandler
  
  # Delivers a message from a message source.
  # @param src the message source
  # @param sender the message sender (meaning depends upon the message
  # source).
  # @param msg the message from the message source.
  # @return true if the message was processed.
  # 
  def message( src, sender, msg )
    raise "subclasser responsibility"
  end
end
