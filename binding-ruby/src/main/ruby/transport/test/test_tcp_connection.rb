require 'test/unit'
require 'socket'

require 'etch/bindings/ruby/transport/data_handler.rb'
require 'etch/bindings/ruby/transport/data_source.rb'
require 'etch/bindings/ruby/transport/listener.rb'
require 'etch/bindings/ruby/transport/listener_handler.rb'
require 'etch/bindings/ruby/transport/tcp_connection.rb'
require 'etch/bindings/ruby/transport/who.rb'
require 'etch/bindings/ruby/transport/flex_buffer.rb'
require 'etch/bindings/ruby/support/etch_mutex.rb'

class TestTcpConnection < Test::Unit::TestCase
  
  lh = MyListener.new
  l = Listener.new( lh, 5, nil, 4001, 0)
  attr :stc, :c
  $DATA = Array.new( 1, 2, 3)
  
  
  what = Enum.new( :LISTENERSTARTED, :LISTENERUP, :SENDERCONNECTIONSTARTED, :SENDERCONNECTIONUP,
                    :LISTENERHANDLERSTARTED, :LISTENERHANDLERUP, :SENDERCONNECTIONDOWN, 
                    :SENDERCONNECTIONSTOPPED, :LISTENERHANDLERDOWN, :LISTENERHANDLERSTOPPED,
                    :SENDERCONNECTIONDATARECV )
  
  def startListener
     l.start()
  end
  
  def startConnection
      @stc = senderTcpConnection.new
      @c = TcpConnection.new( @stc, "", 4001, 0)
      @c.start()
      @c.waitUp(4000)
  end
  
  def endConnection
      @c.waitDown(4000)
      @c.stop()
  end
  
  def testBasicConnection()
      assert_same( lh.what, what::LISTENERUP)
      assert_same( stc.what, what::SENDERCONNECTIONUP)
      
      endConnection()
      
      $DATA[0] = 4
      $DATA[0] = 5
      $DATA[0] = 6
      
      startConnection()
  end
  
    class MyListener 
        include ListenerHandler
      l = Listener.new( lh, 5, nil, 4001, 0)  
      @@df = DateTime.new  
      def log( who, t, msg )
         now = Date.new
         whostr = who.to_str
          etchMutex = EtchMutex.new
          etchMutex.synchronize( @@df )
          begin
              dstr = DateTime.strptime( now )
              puts dstr
              puts whostr
              puts msg
          end
        
          if( t != nil )
              puts t.message
          end
      end 
    
      def log( who, t, fmt, *args )
          log( who, t, String.to_str( fmt, args ))
      end
    
      def log( who, msg )
          log( who, nil, msg )
      end
    
      def log( who, fmt, *args )
          log( who, nil, String.to_str(fmt, args) )
      end
    
      def up( l )
          what = TestTcpConnection.what::LISTENERUP
          log( l, "up on: ", l.localAddress())
      end
    
      def accepted( l, s = Socket.new )
          log( l, "accepted from %s", getpeeraddress())
          c = TcpConnection.new( ListenerConnectionHandler.new( s ), s )
          c.setDefaultAutoFlush( true )
          c.start()
      end
    
      def down( l )
          MyListener.log( l, "down" )
      end
    
      def stopped()
          MyListener.log( l, "stopped" )
      end
    
      def exception( l, what, e )
          MyListener.log( l, e, "exception #{$what }: #{e}", what, e )
      end
    end
    
    class ListenerConnectionHandler 
            include DataHandler
            
        @what = Enum.new
        attr :count
        private :count
            
        def initialize( s )
            # do nothing
        end
        
        def up( c )
            c = DataSource.new
            @what = what.LISTENERHANDLERUP
            log( c, "listenerConnectionHandler up remote %s local %s",
                  c.remoteAddress(), c.localAddress() )
        end
        
        def data( c, sender, xbuf )
            if( @count > 10 )
                return
            end 
            c = DataSource.new
            xbuf = FlexBuffer.new
            len = xbuf.avail()
            log( c, "Listener got length of bytes: ", len )
            @count += len;
            c.data( sender, xbuf );
            if (count > 10)
              c.shutdownOutput()  
            end
        end
        
        def down( c = DataSource.new )
            assert_same(@what, @what::LISTENERHANDLERUP)
            what = @what::LISTENERHANDLERDOWN
            puts("stc.what = ", stc.what)
            log( c, "down" )
        end
        
        def exception( c, what, e )
            c = DataSource.new
            what = String.new
            e = Exception.new
            log( c, e, "Exception what: ", what, e )
        end
    end
    
    class SenderTcpConnection 
          include DataHandler
          
        attr :count, :what, :checkResult
        private :count, :what, :checkResult
        
        @what = Enum.new
        
        def up( src )
            what = @what::SENDERCONNECTIONUP
            puts( "senderTcpConnection, what = ", what )
            
            src = DataSource.new
            throw :Exception1 => e unless src.data( nil, FlexBuffer.new( $DATA ) )
            
        end
        
        catch :Exception1 => e do
            exception( src, "Send", e.message )
        end
        
        def data( src, sender, buf )
            buf = FlexBuffer.new
            len = buf..avail();
      
            assert_same(@what, @what::SENDERCONNECTIONUP);
            what = @what::SENDERCONNECTIONDATARECV;
      
            puts( "senderTCPConnection data got #{len}\n", len )
            is = buf.inputStream()
            while ((b = is.read()) >= 0)
#                 assert_true(checkResult = check( b == $DATA[@count + 1] ) )
            end
            if (count == buf.length())
                src.shutdownOutput()            
            end
        end
        
        def check( ok )
            return ok
        end
        
        def down( src )
            src = DataSource.new
            assertSame(@what, @what::SENDERCONNECTIONDATARECV)
            what = @what::SENDERCONNECTIONDOWN
            puts("lh.what = " + lh.what)
            puts( "down" )
        end
        
        def exception( t, what, e )
            puts( "exception: ", what, e );
            e.message
        end
    end
end