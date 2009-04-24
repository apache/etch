# package etch/bindings/ruby/support

class Enum
  include Comparable
  
  def initialize( name, ord )
    @name = name
    @ord = ord
  end
  
  def <=>( other )
    return @ord<=>( other.ord )
  end
  
  def to_s
    return @name.to_s
  end
  
  attr :name
  attr :ord
  
  alias :to_int :ord
  alias :to_i :ord
  
  protected :initialize
end

# Usage:
#class PrimaryColor < Enum
#  RED = PrimaryColor.new( :RED, 0 )
#  GREEN = PrimaryColor.new( :GREEN, 1 )
#  BLUE = PrimaryColor.new( :BLUE, 2 )
#end
