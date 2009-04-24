require 'socket'
require 'etch/bindings/ruby/transport/who.rb'
require 'etch/bindings/ruby/transport/flex_buffer.rb'
require 'etch/bindings/ruby/transport/data_type_values.rb'

class Packetizer 
      include DataHandler, PacketSource, DataTypeValues
      
    attr :handler, :maxPktSize, :dataSrc
    private :handler, :maxPktSize, :dataSrc
    
    @savedBuf = FlexBuffer.new
      
    $DEFAULT_MAX_PKT_SIZE = 10240
    $USE_DEFAULT_MAX_PKT_SIZE = -1
    $USE_UNLIMITED_MAX_PKT_SIZE = 0
    $HEADER_SIZE = 8
    $SIG = 0xdeadbeef
    
    def initialize( handler, maxPktSize )
        @handler = handler
        if ( maxPktSize == $USE_DEFAULT_MAX_PKT_SIZE )
            maxPktSize = $DEFAULT_MAX_PKT_SIZE
        else if ( maxPktSize == $USE_UNLIMITED_MAX_PKT_SIZE )
                maxPktSize = $INTEGER_MAX_VALUE
        @maxPktSize = maxPktSize
        end
    end
    
    def Packetizer( handler )
         initialize( handler, $USE_DEFAULT_MAX_PKT_SIZE )
    end
    
   def Packetizer( maxPktSize )
        initialize( nil, maxPktSize )
   end
   
   def Packetizer()
        initialize( nil, $DEFAULT_MAX_PKT_SIZE )
   end
    
   def setHandler( handler )
        @handler = handler
   end
    
   def to_s
        str.to_str( "Packetizer/%s", dataSrc )
   end
    
   def setDataSource( src )
       if( src == nil || @dataSrc == nil )
            @dataSrc = src
       else if( src != @dataSrc )
            raise "IllegalArgumentException"
       end
   end
    
   def data( src, sender, buf = FlexBuffer.new )
        setDataSource( src )
        
        while( FlexBuffer.avail() > 0 )
            if( @wantHeader )
                if( @savedBuf.length() + buf.avail() >= HEADER_SIZE )
                    if( @savedBuf.length() == 0 )
                        sig = buf.getint()
                        if( sig != $SIG )
                            raise "IOException"
                        end
                        
                        len = buf.getInt()
                        if( len == 0 )
                           continue
                        end
                        
                        if( len < 0 || len > @maxPktSize )
                            raise "IOException (len < 0 || len > maxPktSize) "
                        end
                        
                        @bodyLen = len
                        @wantHeader = false
                    else 
                        needFromBuf = $HEADER_SIZE - @savedBuf.length()
                        @savedBuf.put( buf, needFromBuf )
                        @savedBuf.setIndex( 0 )
                        
                        sig = @savedBuf.getInt()
                        if( sig != $SIG )
                           raise "IOException bad SIG"
                        end
                        
                        len = @savedBuf.length()
                        @savedBuf.reset()
                        if( len == 0 )
                            continue
                        end
                        
                        if (len < 0 || len > maxPktSize)
                             raise " IOException len < 0 || len > maxPktSize"
                        end
                        
                        @bodyLen = len
                        @wantHeader = false
                    end
                else
                    @savedBuf.setIndex( @savedBuf.length() )
                    @savedBuf.put( buf )
                end
            else if( @savedBuf.length() + buf.avail() >= bodyLen)
                    assert savedBuf.length() < bodyLen
                    if( @savedBuf.length() == 0 )
                         length = buf.length();
                         index = buf.index();
                         buf.setLength( index + @bodyLen );
                         @handler.packet( this, sender, buf );
                         buf.setLength( length );
                         buf.setIndex( index + @bodyLen );          
                         @wantHeader = true;
                    else
                        needFromBuf = @bodyLen - @savedBuf.length()
                        @savedBuf.put( buf, needFromBuf )
                        @savedBuf.setIndex( 0 )
          
                        @handler.packet( this, sender, @savedBuf )
          
                        @savedBuf.reset()
                        @wantHeader = true
                    end
                 else
                      @savedBuf.put( buf )
                 end
            end
          end
        end
      end
    end
    
    def firePacket( sender, buf )
        @handler.packet( this, sender, buf )
    end
    
    def packet( recipient = who.new, buf = FlexBuffer.new )
        len = buf.avail()
        if (len < $HEADER_SIZE)
            raise "IllegalArgumentException( \"len < HEADER_SIZE\" )"
        end
        
        index = buf.index()
        buf.putInt( $SIG )
        buf.putInt( len - $HEADER_SIZE )
        buf.setIndex( index )
        @dataSrc.data( recipient, buf )
    end
    
    def headerSize()
        return $HEADER_SIZE 
    end
    
    def up( src )
        setDataSource( src )
        @handler.down( this ) # this?
    end
    
    def close( reset )
        @dataSrc.close( reset )
    end
    
    def localAddress()
        return @dataSrc.localAddress()
    end
    
    def remoteAddress()
        return @dataSrc.remoteAddress()
    end
    
    def shutdownInput()
        @dataSrc.shutdownInput()
    end
    
    def shutdownOutput()
        @dataSrc.shutdownOutput()
    end
    
    def stop()
        @dataSrc.stop()
    end

end