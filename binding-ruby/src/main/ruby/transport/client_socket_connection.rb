require 'socket'
 
 $port = 4001
  
 clientSocket = TCPServer.new( 'localhost', $port )
    
 while( newSession = clientSocket.accept )     
     data = newSession.gets
     newSession.print data            
 end
     