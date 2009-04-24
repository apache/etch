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
require 'etch/bindings/ruby/support/default_value_factory'

class TestDefaultValueFactory < Test::Unit::TestCase
  
  def setup
    @vf = DefaultValueFactory.new( 3 )
  end

  def test_add_type
    
    #
    # a
    #
    
    a = @vf.addType( Type.new( nil, "a" ) )
    assert_not_nil( a )
    
    x = @vf.addType( @vf.getType( "a" ) )
    assert_not_nil( x )
    assert_same( a, x )
    
    x = @vf.addType( Type.new( nil, "a" ) )
    assert_not_nil( x )
    assert_same( a, x )
    
    #
    # b
    #
    
    b = @vf.addType( Type.new( nil, "b" ) )
    assert_not_nil( b )
    
    y = @vf.addType( @vf.getType( "b" ) )
    assert_not_nil( y )
    assert_same( b, y )
    
    y = @vf.addType( Type.new( nil, "b" ) )
    assert_not_nil( y )
    assert_same( b, y )
    
    #
    # c
    #
    
    c = @vf.addType( Type.new( nil, "c" ) )
    assert_not_nil( c )
    
    z = @vf.addType( @vf.getType( "c" ) )
    assert_not_nil( z )
    assert_same( c, z )
    
    z = @vf.addType( Type.new( nil, "c" ) )
    assert_not_nil( z )
    assert_same( c, z )
    
  end
  
  def test_getType_id_Etch_Runtime_Exception
    x = @vf.getType( DefaultValueFactory::MT__Etch_RuntimeException.xid )
    assert_not_nil( x )
    assert_same( DefaultValueFactory::MT__Etch_RuntimeException, x )
  end
  
  def test_getType_name_Etch_Runtime_Exception
    x = @vf.getType( "_Etch_RuntimeException" )
    assert_not_nil( x )
    assert_same( DefaultValueFactory::MT__Etch_RuntimeException, x )
  end

  def test_just_enough
    @vf.getType( "a" )
    @vf.getType( "b" )
    @vf.getType( "c" )
  end
  
  def test_too_many
  
    @vf.getType( "a" )
    @vf.getType( "b" )
    @vf.getType( "c" )
    
    exceptionCaught = false
    begin 
      @vf.getType( "d" )
    rescue Exception
      exceptionCaught = true
    end
    
    flunk "test should have failed" if ( exceptionCaught == false )
  end

  def test_getTypes
    list = Array.new
    list << @vf.getType( "a" )
    list << @vf.getType( "b" )
    list << @vf.getType( "c" )
    list << DefaultValueFactory::MT__Etch_RuntimeException
    list << DefaultValueFactory::MT__Etch_AuthException
    list << DefaultValueFactory::MT__Etch_Date
    list << DefaultValueFactory::MT__exception
    
    c = @vf.getTypes
    
    # Check for inclusivity. 
    list.each { |element| assert( c.include?( element ) ) }
    c.each{ |element| assert( list.include?( element ) ) }
    
  end
  
  def test_add_field()
    
    #
    # a
    #
    
    a = @vf.addField( Field.new( nil, "a" ) )
    assert_not_nil( a ) 
    
    x = @vf.getField( "a" )
    assert_not_nil( x )
    assert_same( a, x )
    
    #
    # b
    #
    
    b = @vf.addField( Field.new( nil, "b" ) )
    assert_not_nil( b ) 
    
    y = @vf.getField( "b" )
    assert_not_nil( y )
    assert_same( b, y )
    
    #
    # c
    #
    
    c = @vf.addField( Field.new( nil, "c" ) )
    assert_not_nil( c ) 
    
    z = @vf.getField( "c" )
    assert_not_nil( z )
    assert_same( c, z )
    
  end

  def test_get_field_id_messageId
    x = @vf.getField( DefaultValueFactory::MF_messageId.xid )
    assert_not_nil( x ) 
    assert_same( DefaultValueFactory::MF_messageId, x )
  end
  
  def test_get_field_id_inReplyTo
    x = @vf.getField( DefaultValueFactory::MF_inReplyTo.xid )
    assert_not_nil( x ) 
    assert_same( DefaultValueFactory::MF_inReplyTo, x )
  end

  def test_get_field_id_msg
    x = @vf.getField( DefaultValueFactory::MF_msg.xid )
    assert_not_nil( x ) 
    assert_same( DefaultValueFactory::MF_msg, x )
  end
  
  def test_get_field_name_messageId
    x = @vf.getField( "_messageId" )
    assert_not_nil( x ) 
    assert_same( DefaultValueFactory::MF_messageId, x )
  end
  
  def test_get_field_name_inReplyTo
    x = @vf.getField( "_inReplyTo" )
    assert_not_nil( x ) 
    assert_same( DefaultValueFactory::MF_inReplyTo, x )
  end
  
  def test_get_field_name_msg
    x = @vf.getField( "msg" )
    assert_not_nil( x ) 
    assert_same( DefaultValueFactory::MF_msg, x )
  end
  
  def test_get_field_name_ticks
    x = @vf.getField( "ticks" )
    assert_not_nil( x ) 
    assert_same( DefaultValueFactory::MF_ticks, x )
  end
  
  def test_getField_auto_just_enough
    @vf.getField( "a" )
    @vf.getField( "b" )
    @vf.getField( "c" )
  end
  
  def test_getField_auto_too_many
  
    @vf.getField( "a" )
    @vf.getField( "b" )
    @vf.getField( "c" )
  
    exceptionCaught = false
    begin 
      @vf.getField( "d" )
    rescue Exception
      exceptionCaught = true
    end  
    flunk "test should have failed" if ( exceptionCaught == false )
    
  end
  
  def test_getFields
    list = Array.new
    list << @vf.getField( "a" )
    list << @vf.getField( "b" )
    list << @vf.getField( "c" )
    list << DefaultValueFactory::MF_msg
    list << DefaultValueFactory::MF_ticks
    list << DefaultValueFactory::MF_messageId
    list << DefaultValueFactory::MF_inReplyTo
    
    c = @vf.getFields
    
    # Check for inclusivity. 
    list.each { |element| assert( c.include?( element ) ) }
    c.each{ |element| assert( list.include?( element ) ) }
    
  end
  
  def test_get_type_and_fields_are_distinct
    
    # verify that the type and field maps are distinct
    mta = @vf.getType( "a" )
    assert_not_nil( mta )
    mfa = @vf.getField( "a" )
    assert_not_nil( mfa )
    
    assert_not_same( mta, mfa )
  end
  
  # TODO: Test for string encoding
  
  def test_messageId
    
    msg = Message.new( nil, @vf )
    assert_nil( @vf.getMessageId( msg ) )
    
    @vf.setMessageId( msg, 234 )
    msgId = @vf.getMessageId( msg )
    assert_not_nil( msgId ) 
    assert_equal( 234, msgId )
    
    @vf.setMessageId( msg, nil )
    assert_nil( @vf.getMessageId( msg ) ) 
  end
  
  def test_inReplyTo
    
    msg = Message.new( nil, @vf )
    assert_nil( @vf.getInReplyTo( msg ) )
    
    @vf.setInReplyTo( msg, 234 )
    msgId = @vf.getInReplyTo( msg )
    assert_not_nil( msgId )
    assert_equal( msgId, 234 )
    
    @vf.setInReplyTo( msg, nil )
    assert_nil( @vf.getInReplyTo( msg ) )
  end
  
  def test_exportcustomvalue_runtimeexception
    
    value = Exception.new()
    sv = @vf.exportCustomValue( value )
    sv.checkType( DefaultValueFactory::MT__Etch_RuntimeException )
    
    assert_equal( 1, sv.size )
    assert_equal( sv[ DefaultValueFactory::MF_msg ], "Exception" )
  end
  
  def test_exportcustomvalue_runtimeexception_msg
    
    value = Exception.new( "foo!=null" )
    sv = @vf.exportCustomValue( value )
    sv.checkType( DefaultValueFactory::MT__Etch_RuntimeException )
    
    assert_equal( 1, sv.size )
    assert_equal( sv[ DefaultValueFactory::MF_msg ], "foo!=null"  )
  end
  
  def test_exportcustomvalue_date
    
    # the argument gives the no. of seconds since epoch
    value = Time.at(12395)
    sv = @vf.exportCustomValue( value )
    sv.checkType( DefaultValueFactory::MT__Etch_Date )
    assert_equal( 1, sv.size )
    assert_equal( 12395, sv[ DefaultValueFactory::MF_ticks ])
  end

  def test_importcustomvalue_etch_runtimeexception
    sv = StructValue.new( DefaultValueFactory::MT__Etch_RuntimeException )
    e = @vf.importCustomValue( sv )
    assert_not_nil( e )
    assert_nil( e.message() )
  end
  
  def test_importcustomvalue_etch_runtimeexception_msg
    sv = StructValue.new( DefaultValueFactory::MT__Etch_RuntimeException )
    sv.store( DefaultValueFactory::MF_msg, "foo")
    e = @vf.importCustomValue( sv )
    assert_not_nil( e )
    assert_equal( "foo", e.message())
  end
  
  def test_importcustomvalue_etch_date
    sv = StructValue.new( DefaultValueFactory::MT__Etch_Date )
    sv.store( DefaultValueFactory::MF_ticks, 12345)
    e = @vf.importCustomValue( sv )
    assert_not_nil( e )
    assert_equal( 12345, e.to_i)
  end
  
end