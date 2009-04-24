require 'socket'
require 'etch/bindings/ruby/transport/runner_handler.rb'

class Runner < AbstractStartable
             include RunnerHandler

      attr :handler, :thread
      
      private :handler, :thread
      
      def initialize()
        # do nothing
      end
      
      def setRunnerHandler( handler )
        @handler = handler
      end
      
      def start0()
        @thread = Thread.new( newThread )
        @thread.start()
      end
      
      def stop0()
        t = @thread
        if( t != nil )
            @thread = nil
            t.join()
        end
      end
      
      def run()
            fireStarted()
            first = true
            throw :Exception1 => e unless isStarted()
            while(isStarted())
                throw :Exception1 => e unless (!run0?( first ))
                  if( !run0?(first))
                      break
                  end
                  first = false
            end            
      end
      
      catch :Exception1 => e do
        fireException( "run", e )
      end
      
      ensure fireStopped()
      end
      
      def fireStarted()
          if( @handler != nil )
              @handler.started( this )
      end 
      
      def fireException( what, e )
         if( @handler != nil )
            @handler.exception( r, what, e )
      end
      
      def fireStopped()
          if( @handler != nil )
              @handler.stopped( this )
      end 
      
      def run0()
        raise "subclasser responsibility"
      end

end