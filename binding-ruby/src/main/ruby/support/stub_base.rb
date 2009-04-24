# package etch/bindings/ruby/support
require 'etch/bindings/ruby/support/message_handler'


# Base class of stub implementations.
#
class StubBase
  include MessageHandler
  
  
  attr :_obj
  attr :_free
  attr :_pool
  
  def initialize( obj, pool, free )
    @_obj = obj
    @_free = free
    @_pool = pool
  end
  
  
  def message( src, sender, msg )
    
    if (msg == nil)
      if ( @_obj.class.method_defined? :_close )
        @_obj._close( src )
      end
      return true
    end
    return false
  end
end
