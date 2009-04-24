require 'test/unit'
require 'etch/bindings/ruby/msg/array_value'
require 'etch/bindings/ruby/msg/id_name'
require 'etch/bindings/ruby/msg/struct_value'
require 'etch/bindings/ruby/msg/tagged_data_input'
require 'etch/bindings/ruby/msg/tagged_data_output'

class TestStructValue < Test::Unit::TestCase
  def setup
    @mt1 = IdName.new( 1, "one" )
    @mt2 = IdName.new( 2, "two" )
    @mt3 = IdName.new( 3, "three" )
    @mf1 = IdName.new( 4, "four" )
    @mf2 = IdName.new( 5, "five" )
  end
  
  def testStype
    sv = StructValue.new( @mt1 )
    assert_same( @mt1, sv.stype )
    assert_not_same( @mt2, sv.stype )
    
    sv = StructValue.new( @mt2 )
    assert_same( @mt2, sv.stype )
    assert_not_same( @mt1, sv.stype )
  end
  
  def testStype?
    sv = StructValue.new( @mt1 )
    assert( sv.stype?( @mt1 ) )
    assert( !sv.stype?( @mt2 ) )
    
    sv = StructValue.new( @mt2 )
    assert( sv.stype?( @mt2 ) )
    assert( !sv.stype?( @mt1 ) )
  end
  
  
  def testPutArrayValue
    sv = StructValue.new( @mt1 )
    assert( sv.empty? )
    assert_equal( 0, sv.length )
    
    av1 = sv.putArrayValue( @mf1 )
    assert_not_nil( av1 )
    assert_not_same( sv, av1 )
    assert_instance_of( ArrayValue, av1 )
    assert( !sv.empty? )
    assert_equal( 1, sv.length )
    assert_same( av1, sv[@mf1] )
    
    av2 = sv.putArrayValue( @mf2 )
    assert_not_nil( av2 )
    assert_not_same( sv, av2 )
    assert_not_same( av1, av2 )
    assert_instance_of( ArrayValue, av2 )
    assert( !sv.empty? )
    assert_equal( 2, sv.length )
    assert_same( av1, sv[@mf1] )
    assert_same( av2, sv[@mf2] )
  end
  
  def testPutStructValue
    sv = StructValue.new( @mt1 )
    assert( sv.empty? )
    assert_equal( 0, sv.length )
    
    sv1 = sv.putStructValue(@mf1, @mt2)
    assert_not_nil( sv1 )
    assert_not_same( sv, sv1 )
    assert_instance_of( StructValue, sv1 )
    assert( !sv.empty? )
    assert_equal( 1, sv.length )
    assert_same( sv1, sv[@mf1] )
    
    sv2 = sv.putStructValue(@mf2, @mt3)
    assert_not_nil( sv2 )
    assert_not_same( sv, sv2 )
    assert_not_same( sv1, sv2 )
    assert_instance_of( StructValue, sv2 )
    assert( !sv.empty? )
    assert_equal( 2, sv.length )
    assert_same( sv1, sv[@mf1] )
    assert_same( sv2, sv[@mf2] )
  end
  
  def testRead
    readHelper( @mt )
    readHelper( @mt, @mf1, 3 )
    readHelper( @mt, @mf1, 3, @mf2, 4 )
  end
  
  def readHelper( mt, *keyVals )
    tdi = FakeTdi.new( mt, keyVals )
    sv = StructValue.read tdi
    assert_not_nil( sv )
    tdi.close
  end
  
  def testWriteStruct
    sv = StructValue.new @mt1
    writeHelper( sv )
    
    sv.store( @mf1, 123 )
    writeHelper( sv )
    
    sv.store( @mf2, 234 )
    writeHelper( sv )
  end
  
  def writeHelper( sv )
    tdo = FakeTdo.new( sv )
    sv.writeStruct( tdo )
    tdo.close
  end
  
  def testReadKeysAndValues
    assert( true )
  end
  
  def testWriteKeysAndValues
    assert( true )
  end
end

class FakeTdi
  include TaggedDataInput
  include Test::Unit::Assertions
  
  def initialize( mt, keyVals )
    @mt = mt
    @keyVals = keyVals
    @started = false
    @done = false
    @ended = false
  end
  
  def startStruct
    assert( !@started )
    assert( !@done )
    assert( !@ended )
    
    @started = true
    @index = 0
    @xstruct = StructValue.new( @mt )
    
    return @xstruct
  end
  
  def readStructElement( se )
    assert( @started )
    assert( !@done )
    assert( !@ended )
    assert( @index <= @keyVals.length )
    
    if @index < @keyVals.length
      se.key = @keyVals[@index]
      se.value = @keyVals[@index+1]
      @index += 2
      return true
    end
    
    @done = true
    return false
  end
  
  def endStruct( struct )
    assert_same( @xstruct, struct )
    assert( @started )
    assert( @done )
    assert( !@ended )
    
    @ended = true
  end
  
  def close
    assert( @started )
    assert( @done )
    assert( @ended )
    
    assert_same( @mt, @xstruct.stype )
    assert_equal( @keyVals.length/2, @xstruct.length )
    
    index = 0
    while index < @keyVals.length
      key = @keyVals[index]
      value = @keyVals[index+1]
      assert_same( value, @xstruct[key] )
      index += 2
    end
  end
end

class FakeTdo
  include TaggedDataOutput
  include Test::Unit::Assertions
  
  def initialize( struct )
    @xstruct = struct
    @started = false
    @ended = false
  end
  
  def startStruct( struct )
    assert_same( @xstruct, struct )
    assert( !@started )
    assert( !@ended )
    
    @started = true
    @items = {}
  end
  
  def writeStructElement( key, value )
    assert( @started )
    assert( !@ended )
    @items[key] = value
  end
  
  def endStruct ( struct )
    assert_same( @xstruct, struct )
    assert( @started )
    assert( !@ended )
    
    @ended = true
  end
  
  def close
    assert( @started )
    assert( @ended )
    
    assert_equal( @xstruct.length, @items.length )
    assert_equal( @xstruct, @items )
  end
end

