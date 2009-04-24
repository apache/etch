require 'test/unit'
require 'etch/bindings/ruby/msg/array_element'
require 'etch/bindings/ruby/msg/array_value'
require 'etch/bindings/ruby/msg/id_name'
require 'etch/bindings/ruby/msg/struct_value'
require 'etch/bindings/ruby/msg/tagged_data_input'
require 'etch/bindings/ruby/msg/tagged_data_output'

class TestArrayValue < Test::Unit::TestCase
  def testAddArrayValue
    av = ArrayValue.new
    assert( av.empty? )
    assert_equal( 0, av.length )
    
    av1 = av.addArrayValue
    assert_not_nil( av1 )
    assert_not_same( av, av1 )
    assert_instance_of( ArrayValue, av1 )
    assert( !av.empty? )
    assert_equal( 1, av.length )
    assert_same( av1, av[0] )
    
    av2 = av.addArrayValue
    assert_not_nil( av2 )
    assert_not_same( av, av2 )
    assert_not_same( av1, av2 )
    assert_instance_of( ArrayValue, av2 )
    assert( !av.empty? )
    assert_equal( 2, av.length )
    assert_same( av1, av[0] )
    assert_same( av2, av[1] )
  end
  
  def testAddStructValue
    av = ArrayValue.new
    assert_equal( 0, av.length )
    
    mt1 = IdName.new( 1, "foo" )
    mt2 = IdName.new( 2, "bar" )
    
    sv1 = av.addStructValue( mt1 )
    assert_not_nil( sv1 )
    assert_not_same( av, sv1 )
    assert_instance_of( StructValue, sv1 )
    assert_same( mt1, sv1.stype )
    assert( !av.empty? )
    assert_equal( 1, av.length )
    assert_same( sv1, av[0] )
    
    sv2 = av.addStructValue( mt2 )
    assert_not_nil( sv2 )
    assert_not_same( av, sv2 )
    assert_not_same( sv1, sv2 )
    assert_instance_of( StructValue, sv2 )
    assert_same( mt2, sv2.stype )
    assert( !av.empty? )
    assert_equal( 2, av.length )
    assert_same( sv1, av[0] )
    assert_same( sv2, av[1] )
  end
  
  def testRead
    readHelper
    readHelper( 1 )
    readHelper( 2, 3.3 )
    readHelper( 4, true, 6 )
    readHelper( "a", "b", "c", "d" )
  end
  
  def testWrite
    writeHelper
    writeHelper( 1 )
    writeHelper( 2, 3.3 )
    writeHelper( 4, true, 6 )
    writeHelper( "a", "b", "c", "d" )
  end
  
  def readHelper( *vals )
    tdi = FakeTdi.new( vals )
    av = ArrayValue.read( tdi )
    assert_instance_of( ArrayValue, av )
    tdi.close
  end
  
  def writeHelper( *vals )
    av = ArrayValue.new
    vals.each {|v| av << v}
    tdo = FakeTdo.new( vals )
    av.write( tdo )
    tdo.close()
  end
end

class FakeTdo
  include TaggedDataOutput
  include Test::Unit::Assertions
  
  def initialize( vals )
    @vals = vals
    @started = false
    @ended = false
  end
  
  def startArray( array )
    assert( !@started )
    assert( !@ended )
    assert_not_nil( array )
    
    @started = true
    @xarray = array
    @list = Array.new
  end
  
  def writeArrayElement( value )
    assert( @started )
    assert( !@ended )
    
    @list << value
  end
  
  def endArray( array )
    assert( @started )
    assert( !@ended )
    assert_same( @xarray, array )
    
    @ended = true
  end
  
  def close
    assert( @started )
    assert( @ended )
    assert_equal( @vals.length, @list.length )
    assert_equal( @vals, @list )
  end
end

class FakeTdi
  include TaggedDataInput
  include Test::Unit::Assertions
  
  def initialize( vals )
    @vals = vals
    @started = false
    @done = false
    @ended = false
  end
  
  def startArray
    assert( !@started )
    assert( !@done )
    assert( !@ended )
    
    @started = true
    @xarray = ArrayValue.new
    @index = 0
    
    return @xarray
  end
  
  def readArrayElement( ae )
    assert( @started )
    assert( !@done )
    assert( !@ended )
    assert( @index <= @vals.length )
    
    if @index < @vals.length
      ae.value = @vals[@index]
      @index += 1
      return true
    end
    
    @done = true
    return false
  end
  
  def endArray( array )
    assert( @started )
    assert( @done )
    assert( !@ended )
    assert_same( @xarray, array )
    
    @ended = true
  end
  
  def close
    assert( @started )
    assert( @done )
    assert( @ended )
    
    assert_equal( @vals.length, @xarray.length )
    assert_equal( @vals, @xarray )
  end
end
