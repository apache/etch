require 'test/unit'
require 'etch/bindings/ruby/support/delivery_service'
require 'etch/bindings/ruby/idl/test/RemoteTest'
require 'etch/bindings/ruby/idl/test/fake_test'
require 'etch/bindings/ruby/idl/test/ValueFactoryTest'
require 'etch/bindings/ruby/msg/type'
require 'etch/bindings/ruby/msg/field'

class TestTest < Test::Unit::TestCase

  def setup
    @test = FakeTest.new
  end
  
  def test_enum_E1
    # see implementation of EtchEnum
    assert_equal( 3, Test::E1.constants.length )
    assert_not_nil( Test::E1::A )
    assert_not_nil( Test::E1::B )
    assert_not_nil( Test::E1::C )
  end
  
  def test_struct_S1
    s = Test::S1.new( 1, 2, 3 )
    assert_equal( 1, s.x )
    assert_equal( 2, s.y )
    assert_equal( 3, s.z )
    
    s = Test::S1.new( nil, nil, nil )
    assert_nil( s.x )
    assert_nil( s.y )
    assert_nil( s.z )
    
    s = Test::S1.new()
    assert_nil( s.x )
    assert_nil( s.y )
    assert_nil( s.z )
    
    s.x = 4 
    s.y = 5 
    s.z = 6 
    
    assert_equal( 4, s.x )
    assert_equal( 5, s.y )
    assert_equal( 6, s.z )
    
    s.x = nil
    assert_nil( s.x )
    
    s.x = 7
    assert_equal( 7, s.x )
  end
  
  def test_excp_Excp1
    e = Test::Excp1.new( "foo", 23 )
    assert_equal( "foo", e.msg )
    assert_equal( 23, e.code )
    
    e = Test::Excp1.new( nil, nil )
    assert_nil( e.msg )
    assert_nil( e.code )
    
    e = Test::Excp1.new()
    assert_nil( e.msg )
    assert_nil( e.code )
    
    e.msg = "bar"
    e.code = 24
    assert_equal( "bar", e.msg )
    assert_equal( 24, e.code )
  
    e.msg = nil
    e.code = nil
    assert_nil( e.msg )
    assert_nil( e.code )
    
    assert( e.kind_of?( Exception ) )

  end
  
  def test_excp_Excp3
  
    e = Test::Excp3.new
    assert( e.kind_of?( Exception ) )
  end
  
  def test_excp_Excp4
  
    e = Test::Excp4.new
    assert( e.kind_of?( Exception ) )
  end
  
  def test_method_nothing
    @test.nothing
  end
  
  def test_method_incr1
    assert_equal( 2, @test.incr( 1 ) )
  end
  
  def test_method_incr2
    assert_equal( 3, @test.incr( 2 ) )
  end
  
  def test_method_incr3
    assert_equal( -1, @test.incr( -2 ) )
  end
  
  def test_method_sub1
    assert_equal( 5, @test.sub( 7, 2 ) )
  end
  
  def test_method_sub2
    assert_equal( 8, @test.sub( 23, 15 ) )
  end
  
  def test_method_sub3
    assert_equal( -5, @test.sub( 2, 7 ) )
  end
  
  def test_method_sum
    assert_equal( 24, @test.sum( Array[ 1, 2, 3, 7, 11 ] ) )
  end
  
  def test_method_trans1
    assert_equal( 2, @test.trans( Test::E1::A, 5 ) )
  end
  
  def test_method_trans2
    assert_equal( 10, @test.trans( Test::E1::B, 5 ) )
  end
  
  def test_method_trans3
    assert_equal( 12, @test.trans( Test::E1::C, 5 ) )
  end
  
  def test_method_dist1
    assert_equal( Math.sqrt( 3 ), @test.dist( Test::S1.new( 1, 1, 1 ), Test::S1.new( 0, 0, 0 ) ) )
  end
  
  def test_method_dist2
    assert_equal( Math.sqrt( 35 ), @test.dist( Test::S1.new( 1, 2, 3 ), Test::S1.new( 6, 5, 4 ) ) )
  end
  
  def test_method_dist3
    assert_equal( Math.sqrt( 56 ), @test.dist( Test::S1.new( 1, 2, 3 ), Test::S1.new( -1, -2, -3 ) ) )
  end
  
  def test_method_fill1
    x = @test.fill( 0, 1 )
    assert_equal( 0, x.length )
    x.each { |value| assert_equal( 1, value ) }
  end
  
  def test_method_fill2
    x = @test.fill( 1, 2 )
    assert_equal( 1, x.length )
    x.each { |value| assert_equal( 2, value ) }
  end
  
  def test_method_fill3
    x = @test.fill( 2, 3 )
    assert_equal( 2, x.length )
    x.each { |value| assert_equal( 3, value ) }
  end
  
  def test_method_blow1
    begin
      @test.blow( "foo", 2 )
      flunk( "did not throw" )
    rescue Test::Excp1 => e
      assert_equal( "foo", e.msg )
      assert_equal( 2, e.code )
    end
  end
  
  def test_method_blow2
    begin
      @test.blow( "bar", 3 )
      flunk( "did not throw" )
    rescue Test::Excp1 => e
      assert_equal( "bar", e.msg )
      assert_equal( 3, e.code )
    end
  end
  
  def test_method_beets1
    assert_equal( 5, @test.beets( Test::E1::A ) )
  end
  
  def test_method_beets2
    begin
      @test.beets( Test::E1::B )
      flunk( "did not throw" )
    rescue Test::Excp3 => e
      assert( true )
    end
  end
  
   
  def test_method_beets3
    begin
      @test.beets( Test::E1::C )
      flunk( "did not throw" )
    rescue Test::Excp4 => e
      assert( true )
    end
  end
  
  def test_method_beets4
    assert_nil( @test.beets( nil ) )
  end
  
end