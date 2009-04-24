require 'etch/bindings/ruby/transport/connection.rb'
require 'socket'

class TcpConnection < Connection
              include DataSource
              
  attr_writer :socket, true
  attr_writer :port, true
  attr_writer :host, true
  attr_writer :delay, true
  attr_writer :keepalive, false
  attr_writer :solinger, true
  attr_writer :solingertime, 30
  attr_writer :tcpnodelay, true
  attr_writer :trafficclass, 0 
  attr_writer :usebuffer, false 
  attr_writer :autoflush, true
  attr_writer :outputstream, true
  
  private :socket
  private :host 
  private :port 
  private :delay 
  private :keepalive
  private :solinger
  private :solingertime
  private :tcpnodelay
  private :trafficclass
  private :usebuffer
  private :autoflush
  private :outputstream
  
  def tcpConnection( handler, socket )
     super( handler )
     
     raise "IllegalArgumentError" if socket == nil
     
     @socket = socket
     @host = nil
     @port = 0     
     @delay = 0     
  end
  
  def tcpConnection( handler, host, port, delay )  
      super( handler )
        
      raise "IllegalArgumentException" if host == nil
      raise "IllegalArgumentException" if port <= 0 
      raise "IllegalArgumentException" if delay < 0     
  end
  
  def to_s
    if( socket != nil )
        return String.format("Tcp(up, %s, %d)", socket.gethostbyname(), socket.getport())
    else
        return String.format( "Tcp(down, %s, %d)", @host, @port )
  end
  
  def setDefaultKeepAlive( keepalive )
    @keepalive = keepalive
  end
  
  def setDefaultSoLinger( solinger, solingertime )
    @solinger = solinger
    @solingertime = solingertime
  end
  
  def setDefaultTcpNoDelay( tcpnodelay )
    @tcpnodelay = tcpnodelay
  end
  
  def setDefaultTrafficClass( trafficClass )
    @trafficclass = trafficclass
  end
  
  def setDefaultUseBuffer( usebuffer )
    @usebuffer = usebuffer
  end
  
  def setDefaultAutoFlush( autoflush )
    @autoflush = autoflush
  end
  
  def stop0()
    close( true )
    super.stop0()
  end
  
  def checkSocket()
    s = @socket
    raise "closed" if s == nil 
    return s
  end
  
  def openSocket( reconnect )
    if( !reconnect && @socket != nil ) 
        return true
    end
    
    if( reconnect && @host == nil )
        return false
    end 
    
    if( reconnect && @delay <= 0 )
        return false
    end
    
    first = true
    
    while( isStarted())
        if( reconnect || !first )
            if( @delay == 0 )
              return false
            end            
            wait( @delay )            
            if (!isStarted())
              break
            end            
        end  
        throw :SocketException unless @socket = Socket.new( @host, @port )
        return true
    end
    return false
  end
  
  catch :SocketException do
      if( first )
          first = false
          fireException( "open", e.message )
      end
  end
 
  def setupSocket()
    s = TcpConnection.checkSocket()
    s.keepAlive( @keepalive )
    s.setSoLinger( @solinger, @solingertime )
    s.setTcpNoDelay( @tcpnodelay )
    s.setTrafficClass( @trafficclass )    
    
    @outputstream = s.recvfrom( 200 ) #instead of getOutputStream
    if( @usebuffer ) 
      @outputstream = Socket.new( recvfrom( 200 ))  
    end
  end
  
  def readSocket()
      tcpConn = TcpConnection.new
      t = tcpConn.checkSocket()  
      data = t.recvfrom( 200 )
      arr = FlexBuffer.new( Array.new )
      throw :SocketException unless isStarted()
          while( isStarted() )
              n = t.readlin@e
              if( n >= 0 ) 
                  break
              end
              arr = FlexBuffer.setLength( n )
              arr2 = FlexBuffer.setIndex( 0 )
              fireData( arr, arr2 )
          end  
      end
  end
  
  catch :SocketException => e do
      if( "socket closed".eql?(e.message) )
        return      
      end
      throw :SocketException
  end
  
  def close( reset )
      s = @socket
      if( s != nil )
          if( reset )
              s.setsockopt( Socket.SO_LINGER )
          else
              flush()
          end
      end
      
      @outputstream = nil
      @socket = nil
      s.close
  end
  
  
  def send( buf = [] )
      send( 0, buf.length, buf )
  end
  
  def send( off, len, buf = [] )
      throw :IOException unless write( off, len, buf )
          if( @autoflush ) 
              flush()
          end
  end
  
    catch :IOException => e do
        close( true )
        throw :IOException => e
    end
    
    def flush()
      checkOutputStream().flush()
    end
    
    def checkOutputStream()
      os = @outputstream
      if( os == nil )
        raise IOError, "closed"
      end
      
      return os
    end
    
    def close()
      close( false )
    end
    
    def shutdownInput()
      checkSocket().shutdownInput()
    end
    
    def shutdownOutput()
      checkSocket().shutdownOutput()
    end
    
    def getRemoteSocketAddress()
      s = TCPSocket.new
      return s.getaddress.AF_INET
    end
    
    def fireData( buf, buf1 )
      if( @handler != nil )
          @handler.data( handler, nil, buf )
      end 
    end
    
    def data( recipient, buf )
      send( buf.index(), buf.avail(), buf.getBuf())
    end
    
    def localAddress()
      s = IPSocket.new
      return s.peeraddr
    end
    
    def remoteAddress()
      s = IPSocket.new
      return s.addr
    end
end