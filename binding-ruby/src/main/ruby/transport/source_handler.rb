require 'socket'

#
 # SourceHandler receives notification of source events.
 # @param <S> event originator
 #
module SourceHandler
	#
	 # Reports the source is up.
	 # @param src event originator
	 # @throws Exception 
	 #
	def up( src ) 
		raise "Subclass responsibility"
	end
	
	#
	 # Reports the source is down.
	 # @param src event originator
	 # @throws Exception 
	 #
	def down( src ) 
		raise "Subclass responsibility"
	end
	
	@Deprecated
	def started( r )
	end
	
	@Deprecated
	def stopped( r )
	end
	
	@Deprecated
	def exception( r, what, e )
	end
end
