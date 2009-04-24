 # $Id$
 #
 # Created by Champakesan, Badri Narayanan on Aug 13, 2007.
 #
 # Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

require 'etch/bindings/ruby/transport/message'
require 'etch/bindings/ruby/transport/stub_base'

# Description of DefaultMessageHandler.
# * @param <T> The type of the stub
 
class DefaultMessageHandler	< MessageHandler
	#Constructs the DefaultMessageHandler.
  
	def DefaultMessageHandler()
    # nothing to do
	end
  
	def up( src )
		Log.report( "messageHandlerUp", "who", this, "src", src )
		stub = newStub( src )
    return src
	end
	
	#@param src
	# @return new stub
  
	def newStub( src ) 
  end
	
	@Override
	def toString() 
  end

	#private T stub;

	def message( src, sender, msg )
		return stub.message( src, sender, msg )
	end
	
	def down( src )
		Log.report( "messageHandlerDown", "who", this, "src", src )
		stub.message( src, null, null )
		stub = null
	end
end