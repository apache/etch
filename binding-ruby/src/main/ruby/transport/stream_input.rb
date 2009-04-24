require 'socket'

$port = 4001
class StreamIO
  
  streamSocket = TCPSocket::new( '127.0.0.1', $port )
  streamSocket.send( "Badri\n", 5 )
  streamOutput = streamSocket.recv( 100 )
  puts streamOutput
  streamSocket.close
  
end