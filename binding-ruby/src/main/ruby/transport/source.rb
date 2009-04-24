require 'socket'

#
# Common interface to sources of data, packets, or messages. Models
# the various handler's view of a communication channel.
# @param <H> The handler type of this source.
#
module Source
	#
	# @return the local socket address of the source if there
	# is one. Generally this is where the source receives
	# data, packets, or messages.
	# @throws Exception
	#
	def localAddress()
			raise "Subclass responsibility"
	end 
  
	#
	# @return the remote socket address of the source if
	# there is one. Generally this is where the source
	# sends data, packets, or messages.
	# @throws Exception
	 #
	def remoteAddress() 
     raise "Subclass responsibility"
  end
  
	#
	# Shuts down reception of data, packets, or messages. This
	# has the most local effect, and does not apply to shared
	# resources such as web servers.
	# @throws Exception
	#
	def shutdownInput() 
    raise "Subclass responsibility"
  end
  
	#
	# Shuts down sending of data, packets, or messages. This
	# has the most local effect, and does not apply to shared
	# resources such as web servers.
	# @throws Exception
	#
	def shutdownOutput() 
      raise "Subclass responsibility"
  end
  
	#
	 # Shuts down the entire communication channel and releases
	 # all associated resources. Certain persistent channels will
	 # start up again after a short delay.
	 # @param reset if true means do not shut down nicely, but
	 # instead close the channel immediately.
	 # @throws Exception
	 #
	def close( reset ) 
      raise "Subclass responsibility"
  end 
  
	#
	 # Shuts down the entire communication channel and releases
	 # all associated resources. Certain persistent channels will
	 # which would start up again after a short delay are prevented
	 # from doing so.
	 # @throws Exception
	 #
	def stop() 
    raise "Subclass responsibility"
  end
	
	#
	 # @param handler 
	 #
	def setHandler( handler )
    raise "Subclass responsibility" 
  end
end