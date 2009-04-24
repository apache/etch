# package etch/bindings/ruby/support

# Interface which a service implementation may implement which
# is used by the service stub to give notice of an unwanted
# message. This is a message which was not wanted by the service
# stubs (its id did not match any service method).
#
module Unwanted
  
  # Notifies the service implementation that the message
  # is unwanted, that is, its id does not match any defined
  # message.
  # @param src the message source.
  # @param sender the transport defined sender.
  # @param msg the message that was not wanted.
  #
  def _unwanted( src, sender, msg )
    raise "subclasser responsibility"
  end
  
end
