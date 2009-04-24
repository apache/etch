require 'test/unit'
require 'etch/bindings/ruby/msg/id_name'

class TestIdName < Test::Unit::TestCase
  def testGetId()
    assert_equal( 1, IdName.new( 1, "foo" ).xid )
    assert_equal( 1, IdName.new( 1, "bar" ).xid )
    assert_equal( 2, IdName.new( 2, "foo" ).xid )
    assert_equal( 2, IdName.new( 2, "bar" ).xid )
  end
  
  def testGetName
    assert_equal( "foo", IdName.new( 1, "foo" ).name )
    assert_equal( "bar", IdName.new( 1, "bar" ).name )
    assert_equal( "foo", IdName.new( 2, "foo" ).name )
    assert_equal( "bar", IdName.new( 2, "bar" ).name )
  end
  
  def testToString
    assert_equal( "foo", IdName.new( 1, "foo" ).to_s )
    assert_equal( "bar", IdName.new( 1, "bar" ).to_s )
    assert_equal( "foo", IdName.new( 2, "foo" ).to_s )
    assert_equal( "bar", IdName.new( 2, "bar" ).to_s )
  end
  
  def testEqual
    assert( IdName.new( 1, "foo" ).eql?( IdName.new( 1, "foo" ) ) )
    assert( IdName.new( 1, "foo" ).eql?( IdName.new( 1, "bar" ) ) )
    assert( IdName.new( 2, "foo" ).eql?( IdName.new( 2, "foo" ) ) )
    assert( IdName.new( 2, "foo" ).eql?( IdName.new( 2, "bar" ) ) )
    
    assert( !IdName.new( 1, "foo" ).eql?( IdName.new( 2, "foo" ) ) )
    assert( !IdName.new( 1, "foo" ).eql?( IdName.new( 2, "bar" ) ) )
    assert( !IdName.new( 2, "foo" ).eql?( IdName.new( 1, "foo" ) ) )
    assert( !IdName.new( 2, "foo" ).eql?( IdName.new( 1, "bar" ) ) )
  end
  
  def testHash
    assert_equal( IdName.new( 1, "foo" ).hash, IdName.new( 1, "foo" ).hash );
    assert_equal( IdName.new( 1, "foo" ).hash, IdName.new( 1, "bar" ).hash );
    assert_equal( IdName.new( 2, "foo" ).hash, IdName.new( 2, "foo" ).hash );
    assert_equal( IdName.new( 2, "foo" ).hash, IdName.new( 2, "bar" ).hash );
  end
  
  def testHashIt
    assert_equal( 5381, IdName.hashit( "" ) );
    assert_equal( 352988316, IdName.hashit( "a" ) );
    assert_equal( 1511848646, IdName.hashit( "ab" ) );
    assert_equal( 669497117, IdName.hashit( "abc" ) );
    assert_equal( -1994190713, IdName.hashit( "abcd" ) );
    assert_equal( -802680418, IdName.hashit( "abcde" ) );
    assert_equal( 1266308680, IdName.hashit( "abcdef" ) );
    assert_equal( -379372513, IdName.hashit( "abcdefg" ) );
    assert_equal( -1416967159, IdName.hashit( "abcdefgh" ) );
    assert_equal( 53556896, IdName.hashit( "abcdefghi" ) );
    assert_equal( -4427318, IdName.hashit( "abcdefghij" ) );
  end
end
