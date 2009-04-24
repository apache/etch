require 'etch/bindings/ruby/idl/test/Test'

# silly implementation of test used for testing
class FakeTest
  include Test
  
  def beets( e )
    
    if ( e == nil )
      return nil
    end
    
    case ( e )
      when Test::E1::A then return 5
      when Test::E1::B then raise Test::Excp3.new
      when Test::E1::C then raise Test::Excp4.new
      else return nil
    end
  end
  
  def blow( msg, code )
    raise Test::Excp1.new( msg, code )
  end
  
  def dist( a, b )
    c = Test::S1.new( a.x - b.x, a.y - b.y, a.z - b.z )
    return Math.sqrt( c.x * c.x + c.y * c.y + c.z * c.z )
  end
  
  def incr( x )
    return ( x+1 )
  end
  
  def nothing()
    # nothing
  end
  
  def sub( x, y )
    return ( x - y )
  end
  
  def sum( x )
    sumVar = 0 
    x.each { |value| sumVar += value }
    return sumVar
  end
  
  def trans( e, x )
    case ( e )
      when Test::E1::A then return ( x/2 )
      when Test::E1::B then return ( x*2 )
      when Test::E1::C then return ( x+7 )
    end
  end
  
  def fill( n, x )
    y = Array.new( n )
    for i in 0...n
      y[i] = x
    end
    return y
  end

  
  
end