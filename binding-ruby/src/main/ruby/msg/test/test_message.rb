require 'test/unit'
require 'etch/bindings/ruby/msg/id_name'
require 'etch/bindings/ruby/msg/field'
require 'etch/bindings/ruby/msg/type'
require 'etch/bindings/ruby/msg/message'
require 'etch/bindings/ruby/msg/mailbox'
require 'etch/bindings/ruby/msg/tagged_data_input'
require 'etch/bindings/ruby/msg/tagged_data_output'
require 'etch/bindings/ruby/msg/value_factory'

class TestMessage < Test::Unit::TestCase
  
  attr :vf, true
  attr :mt1, true
  attr :rmt, true
  attr :mf1, true
  attr :mf2, true

  def setup
    @vf = MyValueFactory.new()
    @mt1 = Type.new(1, "mt")
    @rmt = Type.new(2, "rmt")
    @mf1 = Field.new(1, "x")
    @mf2 = Field.new(2, "y")
  end

  # Tests
  #

  def testgetVf
    msg = Message.new(@mt1, @vf)
    assert_equal(@vf, msg.vf())
  end

  def testread
    readHelper(@mt1)
    readHelper(@mt1, @mf1, 3)
    readHelper(@mt1, @mf1, 3, @mf2, 4)
  end

  def readHelper(mt, *keyVals)
    tdi = FakeTdi.new(@vf, mt, keyVals)
    #puts tdi
    msg = Message.read(tdi)
    assert_not_nil(msg)
    tdi.close()
  end
  
  def testwriteMessage
    msg = Message.new(@mt1, @vf)
    writeHelper(msg)
    
    msg.store(@mf1, 123)
    writeHelper(msg)
    
    msg.store(@mf2, 234)
    writeHelper(msg)
  end
  
  def writeHelper(msg)
    tdo = FakeTdo.new(msg)
    msg.writeMessage(tdo)
    tdo.close()
  end
  
  def testreply
    msg = Message.new(@mt1, @vf)
    msg.setMessageId(12345)
    
    rmsg = msg.reply(@rmt)
    assert_equal(@rmt, rmsg.getType())
    assert_equal(@vf, rmsg.vf())
    assert_equal(12345, rmsg.getInReplyTo())
  end
  
  def testgetMessageId
    msg = Message.new(@mt1, @vf)
    assert_nil(msg.getMessageId())
    msg.setMessageId(234)
    assert_equal(234, msg.getMessageId)
    msg.setMessageId(345)
    assert_equal(345, msg.getMessageId)
  end
  
  def testgetInReplyTo
    msg = Message.new(@mt1, @vf)
    assert_nil(msg.getInReplyTo)
    msg.setInReplyTo(234)
    assert_equal(234, msg.getInReplyTo)
  end


  # Helper (mostly dummy) classes
  #
  class MyValueFactory 
    include ValueFactory
    
    attr :mf_messageId, true
    attr :mf_inReplyTo, true
    def initialize()
      @mf_messageId = Field.new(nil, "_messageId")
      @mf_inReplyTo = Field.new(nil, "_inReplyTo")
    end
    
    # Return message id of message.
    def getMessageId(msg)
      return msg[@mf_messageId]
    end
    
    # Set message Id. of the message
    def setMessageId(msg, msgId)
      msg.store(@mf_messageId, msgId)
    end
    
    def getInReplyTo(msg)
      return msg[@mf_inReplyTo]
    end
    
    def setInReplyTo(msg, msgId)
      msg.store(@mf_inReplyTo, msgId)
    end
  end
  
  class FakeTdo 
    include TaggedDataOutput
    include Test::Unit::Assertions
    
    attr :xmsg, true
    attr :started, true
    attr :ended, true
    attr :items, true
    
    def initialize(msg)
      @xmsg = msg
      @started = false
      @ended = false
      @items = Hash.new()
    end
    
    def startMessage(msg)
      assert_equal(@xmsg, msg)
      assert_same(@xmsg, msg)
      assert(!@started) 
      assert(!@ended)
      
      @started = true
    end
    
    def writeStructElement(key, value)
      assert(@started)
      assert(!@ended)
      @items.store(key, value)
    end
    
    def endMessage(msg)
      assert_equal(@xmsg, msg)
      assert_same(@xmsg, msg)  # testing ...
      assert(!@ended)
      @ended = true
    end
    
    def close
      assert(@started)
      assert(@ended)
      assert_equal(@xmsg.size(), @items.size())
      assert_equal(@xmsg, @items)
       
    end
  end
  
  class FakeTdi 
    include TaggedDataInput
    include Test::Unit::Assertions
    
    attr :vf, true
    attr :mt, true
    attr :fieldsandvalues, true
    attr :started, true
    attr :ended, true
    attr :done, true
    attr :xmsg, true
    
    def initialize(vf, mt, *fieldsandvalues)
      @vf = vf
      @mt = mt
      @fieldsandvalues = fieldsandvalues
    end
    
    def startMessage()
      
      assert(!@started)
      assert(!@ended)
      assert(!@done)
    
      @started = true
      @xmsg = Message.new(@mt, @vf)
      @index = 0
      
      return @xmsg
    end
    
    def readStructElement(se)
      assert(@started)
      assert(!@done)
      assert(!@ended)
      
      index = 0
      while (@keyVals != nil) and (index < @keyVals.length)
        key = @keyVals[index]
        value = @keyVals[index+1]
        assert_same( value, @xmsg[key] )
        index += 2
      end
      
      @done = true
      return false
    end

    def endMessage(msg)
      assert_equal(@xmsg, msg)
      assert(@started)
      assert(@done)
      assert( !@ended )
      @ended = true
    end

    def close()
     assert(@started)
     assert(@done)
     assert(@ended)
      
      assert_equal(@mt, @xmsg.getType())
      assert_equal(@fieldsandvalues.length/2, @xmsg.length/2)
      
      index = 0
      while index < @fieldsandvalues.length
        key = @fieldsandvalues[index]
        value = @fieldsandvalues[index+1]
        assert_equal( value, @xmsg[key] )
        index += 2
      end
    end

  end

end














