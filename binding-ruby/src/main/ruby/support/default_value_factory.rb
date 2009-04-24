# package etch/bindings/ruby/support
require 'etch/bindings/ruby/msg/value_factory'
require 'etch/bindings/ruby/msg/id_name'
require 'etch/bindings/ruby/support/id_name_map'
require 'etch/bindings/ruby/msg/type'
require 'etch/bindings/ruby/msg/field'
require 'etch/bindings/ruby/support/etch__auth_exception'

# Default implementation of ValueFactory which provides some
# dynamic type and field support, as well as standard
# value conversions and import and export.
#
class DefaultValueFactory
  include ValueFactory
  
  attr :types, true
  attr :fields, true
    
  MT__Etch_RuntimeException = Type.new( nil, "_Etch_RuntimeException" )
  MT__Etch_AuthException = Type.new( nil, "_Etch_AuthException" )
  MT__Etch_Date = Type.new( nil, "_Etch_Date" )
  MT__exception = Type.new( nil, "_exception" )

  MF_msg = Field.new( nil, "msg"  )
  MF_ticks = Field.new( nil, "ticks" )
  MF_messageId = Field.new( nil, "_messageId" )
  MF_inReplyTo = Field.new( nil, "_inReplyTo" )

  def initialize( *params )
    
        
    # Call parametrized constructor
    temp = 100
    temp = params.first unless params.nil? 
    paramInitialize( temp )
    
  end
  
  def paramInitialize( maxAutoCount )
    
    @types = IdNameMap.new( maxAutoCount )
    
    # verify anonymous class technique
    def @types.makeNew( id, name )
      return Type.new( id, name )
    end
    
    # verify anonymous class technique
    @fields = IdNameMap.new( maxAutoCount )
    def @fields.makeNew( id, name )
      return Field.new(id, name )
    end
    
    addType( MT__Etch_RuntimeException );
    addType( MT__Etch_AuthException );
    addType( MT__Etch_Date );
    addType( MT__exception );
    
    addField( MF_msg );
    addField( MF_ticks );
    addField( MF_messageId );
    addField( MF_inReplyTo );
    
  end
  
  # 
  # Types
  #
  
  def addType( type )
    return @types.add( type )
  end
  
  def getType( xid )
    
    if ( xid.class != Fixnum )
      if ( xid.class == String )
        getTypeByName( xid )
      end
    end
    
    case ( xid )
      when MT__Etch_RuntimeException.xid then return MT__Etch_RuntimeException
      when MT__Etch_AuthException.xid then return MT__Etch_AuthException
      when MT__Etch_Date.xid then return MT__Etch_Date
      when MT__exception.xid then return MT__exception
    end
    
    return @types.get( xid )
  end

  def getTypeByName( name )
    @types.get( name )
  end
  
  def getTypes()
    return @types.values()
  end
  
  #
  # Fields
  #
  
  def addField( mf )
    return @fields.add( mf )
  end
  
  def getField( xid )
    
    if ( xid.class == String )
      getFieldByName( xid ) 
    else
      case ( xid )
        when MF_msg.xid then return MF_msg
        when MF_ticks.xid then return MF_ticks
        when MF_messageId.xid then return MF_messageId
        when MF_inReplyTo.xid then return MF_inReplyTo
      end
      return @fields.get( xid )
      
    end
  end
  
  def getFieldByName( name )
    return @fields.get( name )
  end
  
  def getFields()
    return @fields.values()
  end
  
  # Figure out the encoding implementation
  def getStringEncoding()
    raise "not implemented as yet"
  end
  
  #
  # MessageID
  #
  
  def getMessageId( msg )
    return msg[MF_messageId]
  end
  
  def setMessageId( msg, msgid )
    
    if (msgid != nil)
      msg[MF_messageId] = msgid
    else
      msg.delete(MF_messageId)
    end
    
  end
  
  #
  # InReplyTo
  #
  def getInReplyTo( msg )
    return msg[MF_inReplyTo]
  end
  
  def setInReplyTo( msg, msgid )
    
    if (msgid != nil)
      msg[MF_inReplyTo] = msgid
    else
      msg.delete(MF_inReplyTo)
    end
  end
  
  # 
  # Value Conversion
  #
  def exportCustomValue( value )
    
    c = value.class
    
    if ( c == Time )
      sv = StructValue.new( MT__Etch_Date )
      sv.store( MF_ticks, value.to_i )
      return sv
    end
    
    if ( c == Etch_AuthException )
      sv = StructValue.new( MT__Etch_AuthException )
      sv.store( MF_msg, v.to_s() )
      return sv
    end
    
    # catch any exception which wasn't otherwise
    # handled and pass it through.
    #
    if ( c == Exception )
      sv = StructValue.new( MT__Etch_RuntimeException )
      sv.store( MF_msg, value.to_s )
      return sv
    end
    
    return nil
  end
  
  def importCustomValue( sv )
    
    type = sv.getType()

    case ( type.xid )
      when MT__Etch_RuntimeException.xid then return Etch_RuntimeException.new( sv[ MF_msg ] )
      when MT__Etch_AuthException.xid then return Etch_AuthException.new( sv[ MF_msg ])
      when MT__Etch_Date.xid then return Time.at( sv[ MF_ticks ] )
    end
    
    return nil
  end
  
  def getCustomStructType( c )
    if (c == DateTime )
      return MT__Etch_Date
    end
    return nil
  end
  
  def getCustomType( type )
    
    case ( type.xid )
      when MT__Etch_Date then return DateTime
    end
    
    return nil
  end
  
end
