# package etch/bindings/ruby/support

# Base class for call to message translators.
#
require 'etch/bindings/ruby/msg/message'
class RemoteBase

  attr :_svc, true
  attr :_vf, true
  
  def initialize( svc, vf )
    @_svc = svc
    @_vf = vf
  end
  
  def _newMessage( type )
    return Message.new( type, @_vf )
  end
  
  # Shuts down output on this remote object. No more
  # messges may be sent.
  def _shutdownOutput()
    return @_svc.shutdownOutput()
  end
end
