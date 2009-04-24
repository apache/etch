# package etch/bindings/ruby/support


# Adapter between remote and message source.
#
module DeliveryService
  
  # Sends the message to the recipient, but does not wait for
  # any response.
  # @param msg the message to send
  # @throws Exception if there is a problem sending
  #
  def send( msg )
    raise "subclasser responsibility"
  end
  
  # Sends the message which begins a call sequence.
  # @param msg the message to send.
  # @return a mailbox which can be used to read response
  #
  def begincall( msg )
    raise "subclasser responsibility"
  end
  
  # @param mb the mailbox returned by begincall( msg )}.
  # @param responseType the type of the expected response.
  # @param responseField the field of the expected response
  # which would contain any result value or thrown exception.
  # @param timeout time in ms to wait for a response.
  # @throws Exception if there is a problem sending or a timeout
  # waiting or if the result value was an exception.
  #
  def endvoidcall( mb, responseType, responseField, timeout )
    raise "subclasser responsibility"
  end
  
  # @param mb
  # @param responseType the type of the expected response.
  # @param responseField the field of the expected response
  # which would contain any result value or thrown exception.
  # @param timeout time in ms to wait for a response.
  # @return the value of the response field if it isn't an exception.
  # @throws Exception if there is a problem sending or a timeout
  # waiting or if the result value was an exception.
  #
  def endcall( mb, responseType, responseField, timeout )
    raise "subclasser responsibility"
  end
  
  # Shuts down output so no more messages may be sent. The
  # other end should see the input message stream close, and
  # would in turn shutdown its output (which we would then see).
  # @throws Exception if there is a problem shutting down out
  # output stream.
  #
  def shutdownOutput
    raise "subclasser responsibility"
  end
end
