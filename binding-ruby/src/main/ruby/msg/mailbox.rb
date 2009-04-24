# package 'etch/bindings/ruby/msg'

module Mailbox
	def getMessageId
		raise "subclasser responsibility"
	end
	
	def deliver( msg )
		raise "subclasser responsibility"
	end
	
	def deliver( msg, maxDelay )
		raise "subclasser responsibility"
	end
	
	def read
		raise "subclasser responsibility"
	end
	
	def read( maxDelay )
		raise "subclasser responsibility"
	end
	
	def close
		raise "subclasser responsibility"
	end
end
