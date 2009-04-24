require 'thread'

# Check whether the mutex is  
# being held by the current thread itself,
# in which case, don't try to lock again. 
# @param threadId holds the information 
# regarding "which" thread holds the lock

class EtchMutex < Mutex
  
  def synchronize
    
    # do checking here
    if (@threadId == nil)
      # no thread holds lock
      @threadId = Thread.current
      super
    else
      if (@threadId == Thread.current)
        # don't call synchronize again
        # because this thread already holds lock
        # just yield !
        yield
      else
        # another thread wants the lock
        # proceed normal locking operation
        super
      end
    end
  end
  
  # When unlock is called, threadId should be set back
  # nil. 
  def unlock
    @threadId = nil
    super
  end
end