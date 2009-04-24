# package etch/bindings/ruby/support

class Etch_RuntimeException < Exception
  
  attr :msg, true
  
  # Constructs the EtchRuntimeException.
  # @param msg description of the exception that was caught by the stub
  #
  def initialize( msg )
    @msg = msg
  end
  
  def message()
    return msg
  end
  
end