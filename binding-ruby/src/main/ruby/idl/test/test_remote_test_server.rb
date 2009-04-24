require 'test/unit'
require 'etch/bindings/ruby/support/delivery_service'
require 'etch/bindings/ruby/idl/test/RemoteTest'
require 'etch/bindings/ruby/idl/test/ValueFactoryTest'

class TestRemoteTestServer < Test::Unit::TestCase
  
  attr :svc, true
  attr :test, true
  
  def setup
    @svc = MyDeliveryService.new
    @test = RemoteTest.new( svc )
  end
  
  def test_method_nothing
    @test.nothing()  
    check( :VOIDCALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_nothing, 
            Array.new, Array.new, ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_nothing, 
            4000 )
  end
  
  def test_method_incr1
    @svc.xresult = 2
    assert_equal( 2, @test.incr( 1 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_incr, 
            Array[ ValueFactoryTest::MF_x ], Array[ 1 ], 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_incr, 
            4000 )
  end
  
  def test_method_incr2
    @svc.xresult = 3
    assert_equal( 3, @test.incr( 2 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_incr, 
            Array[ ValueFactoryTest::MF_x ], Array[ 2 ], 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_incr, 
            4000 )
  end
  
  def test_method_incr3
    @svc.xresult = -1
    assert_equal( -1, @test.incr( -2 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_incr, 
            Array[ ValueFactoryTest::MF_x ], Array[ -2 ], 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_incr, 
            4000 )
  end
  
  def test_method_sub1
    @svc.xresult = 5
    assert_equal( 5, @test.sub( 7, 2 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_sub, 
            Array[ ValueFactoryTest::MF_x, ValueFactoryTest::MF_y ], 
            Array[ 7, 2 ], 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_sub, 
            4000 )
  end
  
  def test_method_sub2
    @svc.xresult = 8
    assert_equal( 8, @test.sub( 23, 15 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_sub, 
            Array[ ValueFactoryTest::MF_x, ValueFactoryTest::MF_y ], 
            Array[ 23, 15 ], 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_sub, 
            4000 )
  end
  
  def test_method_sub3
    @svc.xresult = -5
    assert_equal( -5, @test.sub( 2, 7 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_sub, 
            Array[ ValueFactoryTest::MF_x, ValueFactoryTest::MF_y ], 
            Array[ 2, 7 ], 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_sub, 
            4000 )
  end
  
  def test_method_sum
    @svc.xresult = 24
    args = Array[ 1, 2, 3, 7, 11 ]
    assert_equal( 24, @test.sum( args ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_sum, 
            Array[ ValueFactoryTest::MF_x ], 
            Array[ args ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_sum, 
            4000 )
  end
  
  def test_method_trans1
    @svc.xresult = 2
    assert_equal( 2, @test.trans( Test::E1::A, 5 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_trans, 
            Array[ ValueFactoryTest::MF_e, ValueFactoryTest::MF_x ], 
            Array[ Test::E1::A, 5 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_trans, 
            4000 )
  end
  
  def test_method_trans2
    @svc.xresult = 10
    assert_equal( 10, @test.trans( Test::E1::B, 5 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_trans, 
            Array[ ValueFactoryTest::MF_e, ValueFactoryTest::MF_x ], 
            Array[ Test::E1::B, 5 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_trans, 
            4000 )
  end
  
  
  def test_method_trans3
    @svc.xresult = 12
    assert_equal( 12, @test.trans( Test::E1::C, 5 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_trans, 
            Array[ ValueFactoryTest::MF_e, ValueFactoryTest::MF_x ], 
            Array[ Test::E1::C, 5 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_trans, 
            4000 )
  end
  
  def test_method_dist1
    @svc.xresult = Math.sqrt( 3 )
    arg1 = Test::S1.new( 1, 1, 1 )
    arg2 = Test::S1.new( 0, 0, 0 )
    assert_equal( Math.sqrt( 3 ), @test.dist( arg1, arg2 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_dist, 
            Array[ ValueFactoryTest::MF_a, ValueFactoryTest::MF_b ], 
            Array[ arg1, arg2 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_dist, 
            4000 )
  end
  
  def test_method_dist2
    @svc.xresult = Math.sqrt( 35 )
    arg1 = Test::S1.new( 1, 2, 3 )
    arg2 = Test::S1.new( 6, 5, 4 )
    assert_equal( Math.sqrt( 35 ), @test.dist( arg1, arg2 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_dist, 
            Array[ ValueFactoryTest::MF_a, ValueFactoryTest::MF_b ], 
            Array[ arg1, arg2 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_dist, 
            4000 )
  end
  
  def test_method_dist3
    @svc.xresult = Math.sqrt( 56 )
    arg1 = Test::S1.new( 1, 2, 3 )
    arg2 = Test::S1.new( -1, -2, -3 )
    assert_equal( Math.sqrt( 56 ), @test.dist( arg1, arg2 ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_dist, 
            Array[ ValueFactoryTest::MF_a, ValueFactoryTest::MF_b ], 
            Array[ arg1, arg2 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_dist, 
            4000 )
  end
  
  def test_method_fill1
    @svc.xresult = Array.new
    x = @test.fill( 0, 1 )
    assert_equal( 0, x.length )
    
    x.each { |value| assert_equal( 1, value ) }
    
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_fill, 
            Array[ ValueFactoryTest::MF_n, ValueFactoryTest::MF_x ], 
            Array[ 0, 1 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_fill, 
            4000 )
  end
  
  def test_method_fill2
    @svc.xresult = Array[ 2 ]
    x = @test.fill( 1, 2 )
    assert_equal( 1, x.length )
    x.each { |value| assert_equal( 2, value ) }
    
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_fill, 
            Array[ ValueFactoryTest::MF_n, ValueFactoryTest::MF_x ], 
            Array[ 1, 2 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_fill, 
            4000 )
  end
  
  def test_method_fill3
    @svc.xresult = Array[ 3, 3 ] 
    x = @test.fill( 2, 3 )
    assert_equal( 2, x.length )
    x.each { |value| assert_equal( 3, value ) }
    
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_fill, 
            Array[ ValueFactoryTest::MF_n, ValueFactoryTest::MF_x ], 
            Array[ 2, 3 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_fill, 
            4000 )
  end
  
  def test_method_blow1
    begin
      @svc.xresult = Test::Excp1.new( "foo", 2 )
      @test.blow( "foo", 2 )
      flunk( "did not throw ! " )
    rescue Test::Excp1 => e
      assert_equal( "foo", e.msg )
      assert_equal( 2, e.code )
    end
    check( :VOIDCALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_blow, 
            Array[ ValueFactoryTest::MF_msg, ValueFactoryTest::MF_code ], 
            Array[ "foo", 2 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_blow, 
            4000 )
  end
  
  def test_method_blow2
    begin
      @svc.xresult = Test::Excp1.new( "bar", 3 )
      @test.blow( "bar", 3 )
      flunk( "did not throw ! " )
    rescue Test::Excp1 => e
      assert_equal( "bar", e.msg )
      assert_equal( 3, e.code )
    end
    check( :VOIDCALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_blow, 
            Array[ ValueFactoryTest::MF_msg, ValueFactoryTest::MF_code ], 
            Array[ "bar", 3 ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_blow, 
            4000 )
  end
  
  def test_method_beets1
    @svc.xresult = 5
    assert_equal( 5, @test.beets( Test::E1::A ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_beets, 
            Array[ ValueFactoryTest::MF_e ], 
            Array[ Test::E1::A ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_beets, 
            4000 )
  end
  
  def test_method_beets2
    
    begin
      
      @svc.xresult = Test::Excp3.new
      @test.beets( Test::E1::B )
      flunk( "beets did not throw" )
    
    rescue Test::Excp3 => e
      assert( true )
    end
    
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_beets, 
            Array[ ValueFactoryTest::MF_e ], 
            Array[ Test::E1::B ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_beets, 
            4000 )
  end
  
  def test_method_beets3
    
    begin
      
      @svc.xresult = Test::Excp4.new
      @test.beets( Test::E1::C )
      flunk( "beets did not throw" )
    
    rescue Test::Excp4 => e
      assert( true )
    end
    
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_beets, 
            Array[ ValueFactoryTest::MF_e ], 
            Array[ Test::E1::C ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_beets, 
            4000 )
  end
  
  def test_method_beets4
    
    @svc.xresult = nil
    assert_nil( @test.beets( nil ) )
    check( :CALL, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_beets, 
            Array[ ValueFactoryTest::MF_e ], 
            Array[ nil ] , 
            ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_beets, 
            4000 )
  end
  
  
  
  def check( what, type, fields, objects, resultType, timeout )
    assert_same( what, @svc.what )
    assert_same( type, @svc.xmsg.getType() )
    
    n = fields.length
    assert_equal( n, objects.length )
    assert_equal( n, @svc.xmsg.size )
    
    for i in 0...n 
      f = fields[i]
      assert_equal( objects[i], @svc.xmsg[f] )
    end
    
    assert_same( resultType, @svc.xresponseType )
    assert_same( ValueFactoryTest::MF_result, @svc.xresponseField )
    assert_equal( timeout, @svc.xtimeout )
  end
  
  class MyDeliveryService 
    include DeliveryService
    include Test::Unit::Assertions
    
    attr :what, true 
    attr :xmsg, true 
    attr :xresponseType, true 
    attr :xresponseField, true 
    attr :xtimeout, true 
    attr :xresult, true 
    attr :xmb, true
    
    def initialize()
      @what = EtchEnum.enum2
    end
    
    def clear()
      
    end
    
    def send( msg )
      assert_nil( @what )
      @what = :SEND
      @xmsg = msg
    end
    
    def begincall( msg )
    
      assert_nil( @what )
      @what = :BEGINCALL
      @xmsg = msg
      # TODO: Uncomment this when PlainMailbox is done !
      # @xmb = PlainMailbox.new( nil, 0, 0, 0, 1)
      return @xmb
    end
    
    def endvoidcall( mb, responseType, responseField, timeout )
      
      assert_same( :BEGINCALL, @what )
      assert_same( mb, @xmb )
      @what = :VOIDCALL
      @xmb = nil
      @xresponseType = responseType
      @xresponseField = responseField
      @xtimeout = timeout
      if ( @xresult != nil && @xresult.kind_of?( Exception ) )
        raise @xresult
      end
      assert_nil( @xresult )
    end
    
    def endcall( mb, responseType, responseField, timeout )

      assert_same( :BEGINCALL, @what )
      assert_same( mb, @xmb )
      @what = :CALL
      @xmb = nil
      @xresponseType = responseType
      @xresponseField = responseField
      @xtimeout = timeout
      if ( @xresult.kind_of?( Exception ) )
        raise @xresult 
      end
      return @xresult
    end
    
    def shutdownOutput()
      assert_nil( @what )
      what = :SHUTDOWNOUTPUT
    end
    
  end
  
end