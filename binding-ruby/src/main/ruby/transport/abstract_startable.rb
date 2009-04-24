require 'socket'

class AbstractStartable 
		include Startable
    
    attr :started, true
    
    private :started
		
	def start()
      if( isStarted() )
          raise "isStarted"
      end
      @started = true
      throw :Exception1 => e unless start0()
  end
  
  catch :Exception1 => e do
      started = false
      throw e
  end
  
  def stop()
      if( !isStarted() )
          raise "!isStarted"
      end
      @started = false
      stop0()
  end
  
  def isStarted()
    return @started
  end
  
  def start0()
     raise "Subclasser responsibility"
  end
  
  def stop0()
     raise "Subclasser responsibility"
  end   
end
		