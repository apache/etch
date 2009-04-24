require 'test/unit'
require 'etch/bindings/ruby/support/id_name_map'
require 'etch/bindings/ruby/msg/id_name'
require 'etch/bindings/ruby/support/etch_enum'
require 'etch/bindings/ruby/support/delivery_service'
require 'etch/bindings/ruby/support/default_value_factory'
require 'etch/bindings/ruby/support/remote_base'

class TestRemoteBase < Test::Unit::TestCase
  
  def setup
    @svc = MyDeliveryService.new()
    @vf = DefaultValueFactory.new()
    @rb = RemoteBase.new( @svc, @vf )
    @mt = Type.new( nil, "foo" )
    @rmt = Type.new( nil, "bar" )
    @rmf = Field.new( nil, "baz" )
  end 
  
  def test_new_message
    msg = @rb._newMessage( @mt )
    msg.checkType( @mt )
    assert_same( @vf, msg.vf )
    assert_nil( @svc.what )
    assert_nil( @svc.xmsg )
    assert_nil( @svc.xresponseType )
    assert_nil( @svc.xresponseField )
    assert_nil( @svc.xtimeout )
  end
  
  def test_send
    msg = @rb._newMessage( @mt )
    @rb._svc.send( msg )
    assert_same( @svc.what, :SEND )
    assert_same( @svc.xmsg, msg )
    assert_nil( @svc.xresponseType )
    assert_nil( @svc.xresponseField )
    assert_nil( @svc.xtimeout )
  end
  
  def test_void_call
    msg = @rb._newMessage( @mt )
    @rb._svc.endvoidcall( @rb._svc.begincall( msg ), @rmt, @rmf, 99 )
    
    assert_same( @svc.what, :VOIDCALL )
    assert_same( @svc.xmsg, msg )
    assert_same( @svc.xresponseType, @rmt )
    assert_same( @svc.xresponseField, @rmf )
    assert_equal( @svc.xtimeout, 99 )
  end
  
  def test_call
    msg = @rb._newMessage( @mt )
    result = @rb._svc.endcall( @rb._svc.begincall( msg ), @rmt, @rmf, 98 )
    
    assert_same( @svc.what, :CALL )
    assert_same( @svc.xmsg, msg )
    assert_same( @svc.xresponseType, @rmt )
    assert_same( @svc.xresponseField, @rmf )
    assert_same( @svc.xtimeout, 98 )
    assert_equal( result, 23 )
  end
  
  def test_close
    @rb._shutdownOutput()
    
    assert_same( @svc.what, :SHUTDOWN )
    assert_nil( @svc.xmsg )
    assert_nil( @svc.xresponseType )
    assert_nil( @svc.xresponseField )
    assert_nil( @svc.xtimeout )
  end
  
  class MyDeliveryService 
    include DeliveryService
    include Test::Unit::Assertions
    
    attr :what, true
    attr :xmsg, true
    attr :xresponseType, true
    attr :xresponseField, true
    attr :xtimeout, true
    
    def initialize()
      @what = EtchEnum.enum
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
      @xmb = nil  # replace with PlainMailbox( ... ), look in Java side
      return @xmb
    end
    
    def endvoidcall( mb, responseType, responseField, timeout )
      assert_same( :BEGINCALL, @what )
      assert_same( @xmb, mb ) # probably not
      @what = :VOIDCALL
      @xmb = nil
      @xresponseType = responseType
      @xresponseField = responseField
      @xtimeout = timeout
    end
    
    def endcall( mb, responseType, responseField, timeout )
      assert_same( :BEGINCALL, @what )
      assert_same( @xmb, mb )
      @what = :CALL
      @xmb = nil
      @xresponseType = responseType
      @xresponseField = responseField
      @xtimeout = timeout
      return 23
    end
    
    def shutdownOutput()
      assert_nil( @what )
      @what = :SHUTDOWN
    end
    
  end  
  
end