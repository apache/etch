require 'socket'
require 'etch/bindings/ruby/transport/runner.rb'

class Listener
  
    attr :backlog
    attr :host 
    attr :port
    attr :delay
    attr :serverSocket
    private :backlog, :host, :port, :delay, :serverSocket


	def initialize( handler, backlog, host, port, delay )
		super( handler )
    
    if( port < 0 || port > 65536 )
        raise "port < 0 || port > 65535" 
    end 
    
    if( delay < 0 )
        raise "IllegalArgumentError"
    end
    
    @backlog = backlog
    @host = host
    @port = port
    @delay = delay
  end
    
    def stop0()
      close( true )
      super.stop0()
    end
    
    def checkSocket()
      ss = @serverSocket
      if( ss == nil ) 
          raise "closed" 
      end
      return ss
    end
    
    def to_s
      if( @serverSocket == nil )
          return String.to_str( "Listener(down, %s, %d)", host, port )
      end
      return String.to_str( "Listener(up, %s, %d)",
      @serverSocket.gethostbyaddr(), @serverSocket.getaddrinfo() )
    end
    
    def openSocket( reconnect ) # to be synchronized
        first  = true
        
        while(isStarted())
            if( reconnect || !first )
                if( delay == 0 )
                    return false
                end
                
                wait( delay )
                
                if(!isStarted())
                  break
                end
            end
            
            throw :Exception1 => e unless @serverSocket = ServerSocket.new( @port, @delay, 
                                           @host != nil ? gethostbyname( @host ): nil )
            return true   
        end
        return false
    end
    
    catch :Exception1 do
      first = true
      if( first )
          first  = false
          Runner.fireException( "open", :Exception )
      end
    end
    
    def setupSocket()
      # do nothing
    end
    
    def readSocket()
      ss = checkSocket()
      
      while( isStarted())
          s = Socket.accept()
          throw :Exception1 => e unless fireException( s )
      end
    end
    
    catch :Exception1 => e do
        s.close
        fireException( "accepted", e.message )
    end
    
    def fireAccepted( s )
        @handler.accepted( handler, s )
    end
    
    def close( reset )
      ss = @serverSocket
      if( ss != nil )
          ss.close
      end
    end
    
    def localAddress()
      return IPSocket.getaddress
    end
    
    def remoteAddress()
      return null
    end
    
    def shutdownInput()
      # do nothing
    end
    
    def shutdownOutput()
      # do nothing
    end   
end