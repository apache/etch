require 'thread'
require 'etch/bindings/ruby/support/etch_mutex'
# Map by id and name of IdNames (or subclasses thereof).
#
class IdNameMap

  attr :maxAutoCount, true
  attr :byId, true
  attr :byName, true
  attr :autocount, true
  
  # Constructs the IdNameMap.
  # @param maxAutoCount the maximum number of automatically
  # declared IdNames allowed. Set to 0 if you don't want any,
  # but this may prevent working with a slightly different
  # version of a service.
  #
  def initialize( maxAutoCount )
  
    @mutex = EtchMutex.new 
    
    @maxAutoCount = maxAutoCount
    @byId = Hash.new
    @byName = Hash.new
    @autocount = 0
  end
  
  # Gets the IdName subclass which corresponds to the specified
  # id, or creates it if it isn't found and if autoCount <
  # maxAutoCount. If created, the IdName is given the name which
  # is id.toString().
  # @param id the id of an IdName.
  # @return the IdName subclass found or created.
  #

  def get( id )
    
    if ( id.class == String )
      
      # print "getByName() call"
      getByName( id )
    else
      # Check if the synchronization is correct !
      @mutex.synchronize do
        t = @byId[id]
        
        if (t == nil)
          # print "autocount = ", @autocount, "maxAutocount = ", @maxAutoCount
          if ( @autocount >= @maxAutoCount )
            raise "maxAutoCount would be exceeded"
          end
          
          @autocount += 1
          
          var = add( makeNew( id, id.to_s() ) )
          return var
        end
        
        return t
      end 
    end
  end
  
  # Gets the IdName subclass which corresponds to the specified
  # name, or creates it if it isn't found and if autoCount <
  # maxAutoCount. If created, the IdName is given the id which
  # is IdName.hash( name ).
  # @param name the name of an IdName.
  # @return the IdName subclass found or created.
  #
  def getByName( name )
    @mutex.synchronize do
      
      t = @byName[ name ]
      if (t == nil)
        if (@autocount >= @maxAutoCount)
          raise "maxAutoCount would be exceeded"
        end
        @autocount += 1
        
        return add( makeNew( IdName.hashit( name ), name ) )
      end
      
      return t
      
    end # mutex end
  end
  
  # Adds the IdName subclass to the map. If the entry matches
  # a current entry (both id and name), then return the current
  # entry instead.
  # 
  # @param t the IdName subclass to add.
  # 
  # @return the IdName from the map, either the new one or a current
  # one.
  # 
  # @throws IllegalArgumentException if there is a collision with
  # id or name, or a collision with id and name where they are not
  # associated with the same object.
  #
  def add( t )
    @mutex.synchronize do
    
      # four cases:
      # 1) t.id and t.name do not exist (no collision)
      # 2) t.id exists but t.name does not (collision)
      # 3) t.id does not exist but t.name does (collision)
      # 4) t.id and t.name exist (possible collision)    
      
      sameId = @byId[ t.xid ]
      sameName = @byName[ t.name ]
      if ( sameId!=nil || sameName!=nil )
        # sameId has the same id as t.
        # sameName has the same name as t.
        # the only way this isn't a problem is if sameId == sameName,
        # because that is the only case where there is a single
        # entry which has both the same id and name as t.
        if (sameId != sameName)
          
          if (sameId != nil && sameName != nil)
            raise "id & name collision" 
          else 
            if (sameId != nil)
              raise "id collision"
            end
            if (sameName != nil )
              raise "id collision"
            end
          end
          
        end
        
        return sameId
      end
      
      @byId.store( t.xid, t )
      @byName.store( t.name, t )
      
      return t
    
    end # mutex end
    
  end
  
  def values()
    
    @mutex.synchronize do
      
      return @byId.values()
    
    end # mutex end
    
  end
  
  # Makes a new subclass of IdName to put in this map.
  # @param id the id of the new subclass of IdName.
  # @param name the name of the new subclass of IdName.
  # @return a newly constructed subclass of IdName to put in
  # this map.
  #
  def makeNew( id, name )
    raise "subclasser responsibility"
  end
  
end