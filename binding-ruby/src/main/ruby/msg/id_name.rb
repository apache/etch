# package 'etch/bindings/ruby/msg'

# An IdName is a base class for Field or Type. It is used
# to bind together a type or field name with the associated id. The id
# is used for certain operations, such as the key in a Map, comparisons,
# and binary encoding on the wire, while the name is used for display.

class IdName
  attr :xid, true
  attr :name, true
  
  # Constructs the IdName.
  # @param id the id for the name 
  # @param name the name of the item.
	def initialize( xid, name )
    xid = IdName.hashit( name ) if xid.nil?
		@xid = xid
		@name = name
	end
	
  # def initialize( name )
  #  initialize( name, hashit( name ) )
  # end
  
	def to_s
		return @name
	end
	
	def hash
		return @xid.hash
	end
  
  def ==(o)
    
    # check for nil first
    if ( o == nil )
      if ( self.class == NilClass )
        return true
      else 
        return false
      end
      return true
    end
    
    # overload == 
    return @xid == o.xid
  end
  
	def eql?( o )
  
    # check for nil first
    if ( o == nil )
      if ( self.class == NilClass )
        return true
      else 
        return false
      end
      return true
    end
    
		return @xid.eql?( o.xid )
	end
	
  # Computes the hash value of the name to be used as the id when
  # constructing an IdName.
  # @param name the name of the type or field.
  # @return a hash of name in the unicode character encoding which is
  # very likely to be unique over reasonable name spaces. Collisions
  # should be very unlikely as they will force the user to change the
  # name.
	def self.hashit( name )
		r = 5381
		name.each_byte {|c| r = c + (r << 6) + (r << 16) - r }
		r = r % 4294967296
        if r > 2147483647 then
        	r = r - 4294967296
        end
		return r
	end
end
