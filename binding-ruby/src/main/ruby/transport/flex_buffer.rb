class FlexBuffer
    
    attr :buffer, :length, :index, :is, :os
    
    private :is, :os
    
    $MAX_BUFFER_LEN = 4*1024*1024
    $INIT_BUFFER_LEN = 32
    
    def initialize( length, buffer = [] )
        FlexBuffer( length, 0, buffer )
    end
    
    def FlexBuffer( length, index, buffer = [] )
        @length = length
        @index = index
        @buffer = buffer
    end
    
    def getBuf()
        return @buffer
    end
    
    def ensureLength( len )
        n = buffer.length
        if( len <= n )
            return 
        end
        
        k = n
        if( k < 1 )
            k = 1
        end
        
        while( len > k && k < $MAX_BUFFER_LEN )
            k *= 2           
        end
        
        if( len > k )
            raise "Buffer overflow exception"
        end
        
        b = Array.new
        arraycopy( buffer, 0, b, 0, n )
        @buffer = b
    end
    
    def length()
        return @length
    end
    
    def setLength()
        if( length < 0 ) 
            raise "length < 0"
        end
        
        ensureLength( length )
        
        @length = length
        if( @index > length )
            @index = length
        end
        
        return 
    end
    
    def index()
        return @index
    end
    
    def setIndex( index )
        if( index < 0 || index > @length )
            raise "index < 0 || index > length"
        end
        
        @index = index
        
        return 
    end
    
    def avail()
        return @length - @index 
    end
    
    def reset()
        @index = 0
        @length = 0
        return 
    end
    
    def compact()
        if( @index == 0 )
            return 
        end
        
        n = avail()
        if( n == 0 )
            reset()
            return
        end
        
        arraycopy( @buffer, @index, @buffer, 0, n )
        @index = 0
        @length = n
        return
    end
    
    def get()
        if( avail() < 1 )
            raise "IOException"
        end
        
        return @buffer[@index + 1] & 255
    end
    
    def get( buf = Array.new )
        checkBuf( buf )
        len = buf.length
        if( len == 0 )
            return 0
        end
        
        k = avail()
        if( k < 1 )
            raise "IOException"
        end
        n = Math.min( len, k )
        arraycopy( @buffer, @index, buf, 0, n )
        @index += n
        
        return n
    end
    
    def get( off, len, buf = [] )
        checkBuf( off, len, buf )
                if( len == 0 )
            return 0
        end
        
        k = avail()
        if( k < 1 )
            raise "IOException"
        end
        n = Math.min( len, k )
        arraycopy( @buffer, @index, buf, 0, n )
        index += n;
    
        return n;
    end
    
    def getInt()
        if( avail() < 4 )
            raise "EOFException"
        end
        
        b0 = @buffer[@index + 1] & 255
        b1 = @buffer[@index + 1] & 255
        b2 = @buffer[@index + 1] & 255
        b3 = @buffer[@index + 1] & 255  
        
        return b0 + (b1 << 8) + (b2 << 16) + (b3 << 24)
    end
    
    def inputStream()
        if( @is != nil )
            return @is
        end
        
        @is = IO.new
        
        def @is.available()
          return avail()
        end
        
        def @is.read()
           if( avail() < 1 )
                return -1
           end
           return get()
        end
        
        def @is.read( b = Array.new )
          return get( b )
        end
        
        def @is.read( off, len, b = Array.new )
          return get( off, len, b )
        end
        
        return @is
    end
    
    def put( b )
        ensureLength( @index + 1 )
       @buffer[@index + 1] = b
        fixLength()
        
        return
    end
    
    def put( buf = Array.new )
        checkBuf( buf )
        len = buf.length
        if( len == 0 )
            return 
        end
        
        ensureLength( @index + len )
        arraycopy( buf, 0, @buffer, @index, len )
        @index += len 
        return 
    end
    
    def put( off, len, buf = Array.new )
        checkBuf( off, len, buf )
        if( len == 0 )
           return 
        end
        ensureLength( @index + len )
        arraycopy( buf, off, @buffer, @index, len )
        @index += len 
        fixLength()
        return 
    end
    
    def put( buf )
        len = FlexBuffer.avail()
        if( len == 0 )
            return 
        end
        
        ensureLength( index+len );
        arraycopy( FlexBuffer.buffer, FlexBuffer.index, @buffer, @index, len )
        FlexBuffer.index += len;
        @index += len;
        fixLength();    
        return
    end
    
    def put( buf, len )
        if( len > FlexBuffer.avail() )
            raise "IllegalArgumentException (len > FlexBuffer.avail() )"
        end 
        
            ensureLength( @index + len );
            System.arraycopy( @buffer, @index, @buffer, @index, len )
            FlexBuffer.index += len;
            @index += len;
            fixLength();   
            return
    end
    
    def putInt( value )
            ensureLength( @index + 4 )
    
            @buffer[index + 1] =  value
            @buffer[index + 1] = (value >> 8)
            @buffer[index + 1] = (value >> 16)
            @buffer[index + 1] = (value >> 24)   
             fixLength()
             return
    end
    
    def skip( len, put )
       if(len < 0)
         raise "IllegalArgumentException"
       end
       
      if(len == 0)
        return 0
      end
      
      if(put)
          ensureLength( @index + len )      
          @index += len;
          fixLength();      
          return len           
      end
      
      k = avail();
      if (k < 1)
        raise "EOFException()"
      end
      
      n = Math.min( len, k );
      @index += n;
      return n
    end
    
    def outputStream()
        if( @os != nil )
            return @os
        end
        
        @os = IO.new
        def @os.write( b )
            put( b )
        end
        
        def @os.write( b = Array.new )
            put( b )
        end
        
        def @os.write( off, len, buf = Array.new )
            put( off, len, buf )
        end
        
        return @os
    end
    
    def fixLength()
       if( @index > @length )
          @length = @index
       end
    end
    
    def checkBuf( buf = Array.new )
        if( buf == nil )
           raise "NullPointerException ( buf == nil )"
        end
    end
    
    def checkBuf( off, len, buf = Array.new )
        if( buf == nil )
           raise "NullPointerException ( buf == nil )"
        end
 
        if( off < 0 || off > @length )
            raise "IllegalArgumentException"
        end
        
        if( len < 0 )
            raise "IllegalArgumentException( len < 0 )"
        end
        
        if (off+len > buf.length)
            raise "IllegalArgumentException( off+len > buf.length )"
        end
    end
    
    def getAvailByte()
         buf = Array.new 
         get( buf )
         return buf
    end
end
