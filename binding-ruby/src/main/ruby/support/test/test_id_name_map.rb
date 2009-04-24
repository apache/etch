require 'test/unit'
require 'etch/bindings/ruby/support/id_name_map'
require 'etch/bindings/ruby/msg/id_name'

class TestIdNameMap < Test::Unit::TestCase
  
  def setup
    
    @map = IdNameMap.new( 3 )
    def @map.makeNew( id, name )
      
      # print "makeNew called with ", id, " & ", name, "\n"
      return IdName.new( id, name )
    end
    
    @a = @map.add( IdName.new( 1, "a" ) )
    @b = @map.add( IdName.new( 2, "b" ) )
  end

  def testget_1
    x = @map.get( 1 )
    assert_not_nil( x )
    assert_same( @a, x )
    assert_equal( 1, x.xid )
    assert_equal( "a", x.name() )
  end

  def testget_a
    x = @map.get( "a" )
    assert_not_nil( x )
    assert_same( @a, x )
    assert_equal( 1, x.xid )
    assert_equal( "a", x.name )
  end

  def testget_2
    x = @map.get( 2 )
    assert_not_nil( x )
    assert_same( @b, x )
    assert_equal( 2, x.xid )
    assert_equal( "b", x.name() )
  end

  def testget_b
    x = @map.get( "b" )
    assert_not_nil( x )
    assert_same( @b, x )
    assert_equal( 2, x.xid )
    assert_equal( "b", x.name )
  end
  
  def testget_3
    x = @map.get( 3 )
    assert_not_nil( x )
    assert_not_same( @a, x )
    assert_not_same( @b, x )
    assert_equal( 3, x.xid )
    assert_equal( "3", x.name )
    
    y = @map.get( 3 )
    assert_same( x, y )
  end
  
  def testget_c
    x = @map.get( "c" )
    assert_not_same( @a, x )
    assert_not_same( @b, x )
    assert_equal( 352988318, x.xid )
    assert_equal( "c", x.name )

    y = @map.get( "c" )
    assert_same( x, y )
  end
  
  def testjustgetenough
    c = @map.get( "c" )
    assert_not_nil( c )
    assert_not_same( @a, c )
    assert_not_same( @b, c )
    
    d = @map.get( "d" )
    assert_not_nil( d )
    assert_not_same( @a, d )
    assert_not_same( @b, d )
    assert_not_same( c, d )
    
    e = @map.get( "e" )
    assert_not_nil( e )
    assert_not_same( @a, e )
    assert_not_same( @b, e )
    assert_not_same( c, e )
    assert_not_same( d, e )  
    
  end
  
  def testgettoomany1
    
    c = @map.get( "c" )
    assert_not_nil( c )
    assert_not_same( @a, c )
    assert_not_same( @b, c )
    
    d = @map.get( "d" )
    assert_not_nil( d )
    assert_not_same( @a, d )
    assert_not_same( @b, d )
    assert_not_same( c, d )
    
    e = @map.get( "e" )
    assert_not_nil( e )
    assert_not_same( @a, e )
    assert_not_same( @b, e )
    assert_not_same( c, e )
    assert_not_same( d, e )  
    
    exceptionCaught = false
    begin 
      @map.get( "f" )
    rescue Exception
      exceptionCaught = true
    end
    
    flunk "test should have failed" if ( exceptionCaught == false )
  end
  
  def testgettoomany2
    
    c = @map.get( 3 )
    assert_not_nil( c )
    assert_not_same( @a, c )
    assert_not_same( @b, c )
    
    d = @map.get( 4 )
    assert_not_nil( d )
    assert_not_same( @a, d )
    assert_not_same( @b, d )
    assert_not_same( c, d )
    
    e = @map.get( 5 )
    assert_not_nil( e )
    assert_not_same( @a, e )
    assert_not_same( @b, e )
    assert_not_same( c, e )
    assert_not_same( d, e )  
    
    exceptionCaught = false
    begin 
      @map.get( 6 )
    rescue Exception
      exceptionCaught = true
    end
    
    flunk "test should have failed" if ( exceptionCaught == false )
  end
  
  def testgettoomany3
    
    c = @map.get( 3 )
    assert_not_nil( c )
    assert_not_same( @a, c )
    assert_not_same( @b, c )
    
    d = @map.get( "d" )
    assert_not_nil( d )
    assert_not_same( @a, d )
    assert_not_same( @b, d )
    assert_not_same( c, d )
    
    e = @map.get( 5 )
    assert_not_nil( e )
    assert_not_same( @a, e )
    assert_not_same( @b, e )
    assert_not_same( c, e )
    assert_not_same( d, e )  
    
    exceptionCaught = false
    begin 
      @map.get( "f" )
    rescue Exception
      exceptionCaught = true
    end
    
    flunk "test should have failed" if ( exceptionCaught == false )
  end
  
  def testadd
    assert_same( @a, @map.add( @a ))
    assert_same( @b, @map.add( @b ))
  end
  
  def test_id_coll
  
    exceptionCaught = false
    begin 
      @map.add( IdName.new( 1, "c" ) )  
    rescue Exception
      exceptionCaught = true
    end  
  end
  
  def test_name_coll
    exceptionCaught = false
    begin 
      @map.add( IdName.new( 3, "a" ) )  
    rescue Exception
      exceptionCaught = true
    end  
  end
  
  def test_id_name_coll
    exceptionCaught = false
    begin 
      @map.add( IdName.new( 2, "a" ) )  
    rescue Exception
      exceptionCaught = true
    end  
  end
  
end