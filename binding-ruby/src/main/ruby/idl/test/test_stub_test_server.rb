require 'test/unit'
require 'etch/bindings/ruby/support/delivery_service'
require 'etch/bindings/ruby/idl/test/RemoteTest'
require 'etch/bindings/ruby/idl/test/StubTest'
require 'etch/bindings/ruby/idl/test/ValueFactoryTest'
require 'etch/bindings/ruby/idl/test/fake_test'
require 'etch/bindings/ruby/support/message_source'
require 'etch/bindings/ruby/msg/message'

class TestStubTestServer < Test::Unit::TestCase
  
  
  def setup
    @test = FakeTest.new
    @vf = ValueFactoryTest.new
    @stub = StubTest.new( @test, nil, nil )
    @src = MyMessageSource.new
  end
  
  def test_method_nothing
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_nothing, @vf )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_nothing )
    assert_equal( 0, @src.xreply.size )
  end
  
  def test_method_incr1
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_incr, @vf )
    msg.store( ValueFactoryTest::MF_x, 3 )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_incr )
    assert_equal( 1, @src.xreply.size )
    assert_equal( 4, @src.xreply[ ValueFactoryTest::MF_result ] )
  end
  
  def test_method_incr2
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_incr, @vf )
    # msg.store( ValueFactoryTest::MF_x, 3 )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_incr )
    assert_equal( 1, @src.xreply.size )
    o = @src.xreply[ ValueFactoryTest::MF_result ] 
    assert( o.kind_of?( Exception ) )
  end
  
  def test_method_sub
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_sub, @vf )
    msg.store( ValueFactoryTest::MF_x, 7 )
    msg.store( ValueFactoryTest::MF_y, 3 )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_sub )
    assert_equal( 1, @src.xreply.size )
    assert_equal( 4, @src.xreply[ ValueFactoryTest::MF_result ] )
  end
  
  def test_method_sum
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_sum, @vf )
    msg.store( ValueFactoryTest::MF_x, Array[ 1, 2, 3, 7, 11 ] )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_sum )
    assert_equal( 1, @src.xreply.size )
    assert_equal( 24, @src.xreply[ ValueFactoryTest::MF_result ] )
  end
  
  def test_method_trans1
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_trans, @vf )
    msg.store( ValueFactoryTest::MF_e, Test::E1::A )
    msg.store( ValueFactoryTest::MF_x, 5 )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_trans )
    assert_equal( 1, @src.xreply.size )
    assert_equal( 2, @src.xreply[ ValueFactoryTest::MF_result ] )
  end
  
  def test_method_trans2
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_trans, @vf )
    msg.store( ValueFactoryTest::MF_e, Test::E1::B )
    msg.store( ValueFactoryTest::MF_x, 5 )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_trans )
    assert_equal( 1, @src.xreply.size )
    assert_equal( 10, @src.xreply[ ValueFactoryTest::MF_result ] )
  end
  
  def test_method_trans3
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_trans, @vf )
    msg.store( ValueFactoryTest::MF_e, Test::E1::C )
    msg.store( ValueFactoryTest::MF_x, 5 )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_trans )
    assert_equal( 1, @src.xreply.size )
    assert_equal( 12, @src.xreply[ ValueFactoryTest::MF_result ] )
  end

  def test_method_dist1
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_dist, @vf )
    msg.store( ValueFactoryTest::MF_a, Test::S1.new( 1, 1, 1 ) )
    msg.store( ValueFactoryTest::MF_b, Test::S1.new( 0, 0, 0 ) )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_dist )
    assert_equal( 1, @src.xreply.size )
    assert_equal( Math.sqrt( 3 ), @src.xreply[ ValueFactoryTest::MF_result ] )
  end

  def test_method_dist2
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_dist, @vf )
    msg.store( ValueFactoryTest::MF_a, Test::S1.new( 1, 2, 3 ) )
    msg.store( ValueFactoryTest::MF_b, Test::S1.new( 6, 5, 4 ) )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_dist )
    assert_equal( 1, @src.xreply.size )
    assert_equal( Math.sqrt( 35 ), @src.xreply[ ValueFactoryTest::MF_result ] )
  end

  def test_method_fill
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_fill, @vf )
    msg.store( ValueFactoryTest::MF_n, 4 )
    msg.store( ValueFactoryTest::MF_x, 3 )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_fill )
    assert_equal( 1, @src.xreply.size )
    x = @src.xreply[ ValueFactoryTest::MF_result ]
    assert_not_nil( x )
    assert_equal( 4, x.length )
    x.each { |value| assert_equal( 3, value ) }
  end

  def test_method_blow
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_blow, @vf )
    msg.store( ValueFactoryTest::MF_msg, "foo" )
    msg.store( ValueFactoryTest::MF_code, 23 )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_blow )
    assert_equal( 1, @src.xreply.size )
    e = @src.xreply[ ValueFactoryTest::MF_result ]
    assert_not_nil( e )
    assert_equal( "foo", e.msg )
    assert_equal( 23, e.code )
  end
  
  def test_method_beets1
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_beets, @vf )
    msg.store( ValueFactoryTest::MF_e, Test::E1::A )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_beets )
    assert_equal( 1, @src.xreply.size )
    o = @src.xreply[ ValueFactoryTest::MF_result ]
    assert_equal( 5, o )
  end
  
  def test_method_beets2
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_beets, @vf )
    msg.store( ValueFactoryTest::MF_e, Test::E1::B )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_beets )
    assert_equal( 1, @src.xreply.size )
    o = @src.xreply[ ValueFactoryTest::MF_result ]
    assert_equal( Test::Excp3, o.class )
  end
  
  def test_method_beets3
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_beets, @vf )
    msg.store( ValueFactoryTest::MF_e, Test::E1::C )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_beets )
    assert_equal( 1, @src.xreply.size )
    o = @src.xreply[ ValueFactoryTest::MF_result ]
    assert_equal( Test::Excp4, o.class )
  end
  
  
  def test_method_beets4
    msg = Message.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_beets, @vf )
    msg.store( ValueFactoryTest::MF_e, nil )
    @stub.message( @src, nil, msg )
    @src.xreply.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_beets )
    assert_equal( 1, @src.xreply.size )
    o = @src.xreply[ ValueFactoryTest::MF_result ]
    assert_nil( o )
  end
  
  

  class MyMessageSource 
    include MessageSource
    include Test::Unit::Assertions
    
    attr :xreply, true
    
    def message( recipient, msg )
      assert_nil( recipient)
      assert_nil( @xreply )
      @xreply = msg
    end
  end
end