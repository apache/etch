require 'socket'
 #
 # Interface used to report listener events.
 #
module ListenerHandler include
			SourceHandler
	#
	# Reports that the listener has accepted a connection.
	# @param t event originator
	# @param s the socket of the connection.
	# @throws Exception 
	#
	def accepted( t, s ) 
		raise "Subclass responsibility"
	end
end
