require 'test/unit'
require 'etch/bindings/ruby/support/id_name_map'
require 'etch/bindings/ruby/msg/id_name'
require 'etch/bindings/ruby/support/etch_enum'
require 'etch/bindings/ruby/support/delivery_service'
require 'etch/bindings/ruby/support/default_value_factory'
require 'etch/bindings/ruby/support/remote_base'
require 'etch/bindings/ruby/support/close'
require 'etch/bindings/ruby/support/unwanted'
require 'etch/bindings/ruby/support/message_source'
require 'etch/bindings/ruby/support/stub_base'
require 'etch/bindings/ruby/support/who'

class TestStubBase < Test::Unit::TestCase
  
  def test_close1
    obj = MyUnwantedClose.new()
    sb = StubBase.new( obj, nil, nil )
    src = MyMessageSource.new()
    
    sb.message( src, nil, nil )
    
    assert_equal( :CLOSE, obj.what )
    assert_same( src, obj.src )
    assert_nil( obj.sender )
    assert_nil( obj.msg )
  end
  
  def test_close2
    obj = MyClose.new()
    sb = StubBase.new( obj, nil, nil )
    src = MyMessageSource.new()

    sb.message( src, nil, nil )
    
    assert_equal( :CLOSE, obj.what )
    assert_same( src, obj.src )
  end
  
  def test_close3
    obj = MyUnwanted.new()
    sb = StubBase.new( obj, nil, nil )
    src = MyMessageSource.new()
    
    sb.message( src, nil, nil )
    
    assert_nil( obj.what )
    assert_nil( obj.src )
    assert_nil( obj.sender )
    assert_nil( obj.msg )
  end
  
  
  class MyMessageSource 
    include MessageSource 
    include Test::Unit::Assertions
    
    def message( recipient, msg )
      flunk( "not needed" )
    end
    
    def vf()
      flunk( "not needed" )
      return nil
    end
  end
  
  class MyUnwantedClose 
    include Close
    include Unwanted
    include Test::Unit::Assertions
    
    attr :what, true
    attr :src, true
    attr :sender, true
    attr :msg, true
    
    def initialize()
      @what = EtchEnum.enum
    end
    
    def _unwanted( src, sender, msg )
      assert_nil( @what )
      @what = :UNWANTED
      @src = src
      @sender = sender
      @msg = msg
    end
    
    def _close( src )
      assert_nil( @what )
      @what = :CLOSE
      @src = src
      @sender = nil
      @msg = nil
    end
  end
  
  class MyClose 
    include Close
    include Test::Unit::Assertions
    
    attr :what, true
    attr :src, true
    
    def initialize()
      @what = EtchEnum.enum
    end
    
    def _close( src )
      assert_nil( @what )
      @what = :CLOSE
      @src = src
    end
  end
  
  class MyUnwanted
    include Unwanted
    include Test::Unit::Assertions
    
    attr :what, true
    attr :src, true
    attr :sender, true
    attr :msg, true
    
    def initialize()
      @what = EtchEnum.enum
    end
    
    def _unwanted( src, sender, msg )
      assert_nil( @what )
      @what = :UNWANTED
      @src = src
      @sender = sender
      @msg = msg
    end
  end
  
end