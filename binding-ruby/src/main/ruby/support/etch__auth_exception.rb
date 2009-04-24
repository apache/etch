# package 'etch/bindings/ruby/support'
require 'etch/bindings/ruby/support/etch__runtime_exception'

class Etch_AuthException < Etch_RuntimeException
  
  def initialize( msg )
    super( msg )
  end
  
end