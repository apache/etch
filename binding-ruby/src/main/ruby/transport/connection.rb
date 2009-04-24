require 'etch/bindings/ruby/transport/monitor.rb'
require 'etch/bindings/ruby/transport/runner.rb'
require 'etch/bindings/ruby/transport/runner_handler.rb'
require 'socket'

class Connection < Runner
      include Source, RunnerHandler
      
    attr_writer :handler, true
    
    protected :handler
    
      def initialize( handler )
        Runner.setRunnerHandler( handler )
        @handler = handler 
      end
      
      def setHandler( handler )
        Runner.setRunnerHandler( handler )
        @handler = handler
      end
      
      def started( runner )
        # do nothing
      end
      
      def stopped( runner )
        # do nothing
      end
      
      def exception( runner, what, e )
        e = Exception.new
        e.message
      end
      
      def run0
        if( !openSocket?(!first) )
            return false
        end
        
        throw :Exception1 unless setUpSocket()
        
        throw :Exception2 unless fireUP()
        throw :Exception2 unless readSocket() 
        return true
     end
     
     catch :Exception1 => e do
       fireException( "setup", e )
       return true
     end
     
     catch :Exception2 =>  e do
       fireException( "run", e )
       close( true )
       return true
     end
     
     ensure 
     begin
        close( false )
        fireDown()
     end
     
     def openSocket( reconnect )
       raise "subclass responsibility"
     end
     
     def setUpSocket()
       raise "subclass responsibility"
     end
     
     def readSocket()
       raise "subclass responsibility"
     end
     
     def close( reset )
       raise "subclass responsibility"
     end
     
     def fireUp()
       notifyUp()
       if( @handler != nil )
          @handler.up( handler )
       end       
     end
     
     def fireDown()
       notifyUp()
       if( @handler != nil )
          @handler.down( handler )
       end       
     end
     
     def notifyUp()
       @status.set( $UP )
     end
     
     def notifyDown()
       @status.set( $DOWN )
     end
     
     def waitUp( maxdelay )
       @status.waitUntilEq( $UP, maxdelay )
     end
     
     def waitDown( maxdelay )
       @status.waitUntilEq( $DOWN, maxdelay )
     end
     
     @status = Monitor.new( @status, $DOWN )
     $DOWN = "down"
     $UP = "up"
     
end
