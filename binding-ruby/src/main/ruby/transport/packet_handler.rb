require 'socket'
require 'etch/bindings/ruby/transport/Who'
require 'etch/bindings/ruby/transport/FlexBuffer'

 #
 # Interface used to deliver packets from a packet source.
 #
module PacketHandler include SourceHandler
	#
	# Delivers a packet from a packet source.
	# @param src
	# @param sender
	# @param buf
	# @throws Exception
	#
	def packet( src, sender, buf ) 
		raise "Subclass responsibility"
	end
end
