
# Adding Enum capability to ruby

module EtchEnum
  
  def self.enum( *syms )
    
    syms.each { |s| const_set( s, s.to_s ) }
    const_set( :DEFAULT, syms.first ) unless ( syms.nil? || const_defined?( :DEFAULT ) )
  end
  
  def self.enum2( *syms )
    
    order = 0
    syms.each { |s| 
      const_set( s.to_s, order ) 
      order += 1 
    } unless ( syms.nil? || syms.length == 0 )
  end
  
end