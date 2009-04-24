# package etch/bindings/ruby/support
require 'etch/bindings/ruby/support/message_handler'

# An interface used to deliver responses to a message. Support for
# the mechanism is somewhat dependent upon properties of the transport
# and message format. 
#
module Mailbox 
  include MessageHandler
  
  def getMessageId()
    raise "subclasser responsibility"
  end
  
  # @param maxDelay the maximum amount of time in milliseconds to
  # wait to read a message from an empty mailbox. 0 means wait
  # forever, -1 means don't wait at all.
  # @return the next message to be read from the mailbox, or null if
  # the mailbox is empty and closed. Wait forever for such a message
  # to be delivered.
  #
  def read( maxDelay )
    raise "subclasser responsibility"
  end
  
  # Closes the mailbox so that no more messages can be delivered.
  # Queued messages remain to be read. Reading an empty closed
  # mailbox returns null.
  def closeDelivery()
    raise "subclasser responsibility"
  end
  
  # Closes the mailbox so that no more messages will be delivered or
  # read. Any remaining queued messages are delivered to a default
  # handler.
  def closeRead()
    raise "subclasser responsibility"
  end
  
end