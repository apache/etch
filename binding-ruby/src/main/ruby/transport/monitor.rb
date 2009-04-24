class Monitor
    attr :description, :initialValue, :value
    private :description, :initialValue, :value
    
    def initialize( description, initialValue )
        @description = description
        @initialValue = initialValue
    end
    
    def getDescription()
        return @description
    end
    
    def to_s()
          return "Monitor ", description, ": ", @value
    end
    
    def get()
        return @value
    end
    
    def set( newValue )
      @mutex.synchronize do
          oldValue = @value
          @value = newValue
          notifyAll()
          return oldValue
      end
    end
    
    def waitUntilSet( maxDelay )
      @mutex.synchronize do
          wait( maxDelay )
          return @value
      end
    end
    
    def waitUntilEqAndSet( desiredValue, newValue )
      @mutex.synchronize do
          return waitUntilEqAndSet( desiredValue, 0, newValue )
      end
    end
    
    def waitUntilEqAndSet( desiredValue, maxDelay, newValue )
        waitUntilEq( desiredValue, maxDelay )
        return set( newValue )
    end
    
    def waitUntilEq( desiredValue )
        waitUntilEq( desiredValue, 0 )
    end
    
#   /**
#   * Waits until the value is set to the specified value.
#   * 
#   * @param desiredValue the value we are waiting for.
#   * 
#   * @param maxDelay the max amount of time in ms to wait.
#   * If 0 is specified, we will wait forever.
#   * 
#   * @throws InterruptedException if we waited too long.
#   */
    def waitUntilEq( desiredValue, maxDelay )
      @mutex.synchronize do
            long now = Timer.currentTimeMillis()
            long endTime = maxDelay > 0 ?
              now + maxDelay : Long.MAX_VALUE
            
            while (!eq( @value, desiredValue ) && now < endTime)
                waitUntilSet( endTime - now )
                now = DateTime.usec # returns time in microseconds
            end
            if (!eq( @value, desiredValue ))
              raise "InterruptedException"
            end
      end
    end
    
#      /**
#   * @param undesiredValue
#   * @param newValue
#   * @return the old value
#   * @throws InterruptedException
#   */
    def waitUntilNotEqAndSet( undesiredValue, newValue )
      @mutex.synchronize do
         return waitUntilNotEqAndSet( undesiredValue, 0, newValue )
      end
    end

#  /**
#   * @param undesiredValue
#   * @param maxDelay
#   * @param newValue
#   * @return the old value
#   * @throws InterruptedException
#   */
    def waitUntilNotEqAndSet( undesiredValue, maxDelay, newValue )
        waitUntilNotEq( undesiredValue )
        return set( newValue )
    end
    
#      /**
#   * Waits forever until the value is not the specified value.
#   * 
#   * @param undesiredValue the value we do not want.
#   * 
#   * @return the current value of the monitor.
#   * 
#   * @throws InterruptedException if we waited too long.
#   */
    def  waitUntilNotEq( undesiredValue )
        return waitUntilNotEq( undesiredValue, 0 )
    end
    
#  /**
#   * Waits until the value is not the specified value.
#   * 
#   * @param undesiredValue the value we do not want.
#   * 
#   * @param maxDelay the max amount of time in ms to wait.
#   * If 0 is specified, we will wait forever.
#   * 
#   * @return the current value of the monitor.
#   * 
#   * @throws InterruptedException if we waited too long.
#   */
    def waitUntilNotEq( undesiredValue, maxDelay )
      @mutex.synchronize do
        long now = Timer.currentTimeMillis()
        long endTime = maxDelay > 0 ?
          now + maxDelay : Long.MAX_VALUE
        
        while (eq( @value, undesiredValue ) && now < endTime)
          waitUntilSet( endTime - now )
          now = Timer.usec
        end
        
        if (eq( @value, undesiredValue ))
          raise "timeout"
        end
        return @value
    end
    
#  /**
#   * Compares the specified values.
#     * 
#   * @param v1 a value to compare, which may be null.
#   * 
#   * @param v2 another value to compare, which may be null.
#   * 
#   * @return true if the values are equal, false otherwise. If both
#   * values are null, they are considered equal.
#   */
    def eq( v1, v2 )
        if (v1 != nil && v2 != nil)
          return v1.equals( v2 )
        end 
        return v1 == v2
    end
end