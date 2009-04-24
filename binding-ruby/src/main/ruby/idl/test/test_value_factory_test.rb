require 'test/unit'
require 'etch/bindings/ruby/support/delivery_service'
require 'etch/bindings/ruby/idl/test/RemoteTest'
require 'etch/bindings/ruby/idl/test/ValueFactoryTest'
require 'etch/bindings/ruby/msg/type'
require 'etch/bindings/ruby/msg/field'

class TestValueFactoryTest < Test::Unit::TestCase

  
  def setup
    @vf = ValueFactoryTest.new  
  end
  
  def test_E1
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_E1 )
    
    checkField( ValueFactoryTest::MF_A )
    checkField( ValueFactoryTest::MF_B )
    checkField( ValueFactoryTest::MF_C )
  end
  
  def test_E1_export
    testEnumExport( Test::E1::A, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_E1, 
                    ValueFactoryTest::MF_A )
    testEnumExport( Test::E1::B, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_E1, 
                    ValueFactoryTest::MF_B )
    testEnumExport( Test::E1::C, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_E1, 
                    ValueFactoryTest::MF_C )
  end
  
  def test_E1_import
    testEnumImport( Test::E1::A, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_E1, 
                    ValueFactoryTest::MF_A )
    testEnumImport( Test::E1::B, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_E1, 
                    ValueFactoryTest::MF_B )
    testEnumImport( Test::E1::C, ValueFactoryTest::MT_etch_bindings_ruby_test_Test_E1, 
                    ValueFactoryTest::MF_C )                    
  end
  
  def test_S1
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_S1 )
    
    checkField( ValueFactoryTest::MF_x )
    checkField( ValueFactoryTest::MF_y )
    checkField( ValueFactoryTest::MF_z )  
  end
  
  def test_S1_export
    sv = @vf.exportCustomValue( Test::S1.new( 19, 23, 29 ) )
    sv.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_S1 )
    assert_equal( 3, sv.size )
    assert_equal( 19, sv[ ValueFactoryTest::MF_x ] )
    assert_equal( 23, sv[ ValueFactoryTest::MF_y ] )
    assert_equal( 29, sv[ ValueFactoryTest::MF_z ] )
  end
  
  def test_S1_import

    sv = StructValue.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_S1 )
    sv.store( ValueFactoryTest::MF_x, 101 )
    sv.store( ValueFactoryTest::MF_y, 103 )
    sv.store( ValueFactoryTest::MF_z, 107 )
    s = @vf.importCustomValue( sv )
    assert_equal( 101, s.x )
    assert_equal( 103, s.y )
    assert_equal( 107, s.z )
  end
  
  def test_S2
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_S2 )
    
    checkField( ValueFactoryTest::MF_a )
    checkField( ValueFactoryTest::MF_b )
    checkField( ValueFactoryTest::MF_c )  
  end
  
  def test_S2_export
    
    a = Test::S1.new( 21, 22, 23 )
    b = Test::S1.new( 31, 32, 33 )
    c = Test::E1::A
    
    sv = @vf.exportCustomValue( Test::S2.new( a, b, c ) )
    sv.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_S2 )
    assert_equal( 3, sv.size )
    assert_equal( a, sv[ ValueFactoryTest::MF_a ] )
    assert_equal( b, sv[ ValueFactoryTest::MF_b ] )
    assert_equal( c, sv[ ValueFactoryTest::MF_c ] )
  end
  
  def test_S2_import

    sv = StructValue.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_S2 )
    
    sv.store( ValueFactoryTest::MF_a, Test::S1.new( 21, 22, 23 ) )
    sv.store( ValueFactoryTest::MF_b, Test::S1.new( 31, 32, 33 ) )
    sv.store( ValueFactoryTest::MF_c, Test::E1::A )
    
    s = @vf.importCustomValue( sv )
    assert_equal( 21, s.a.x )
    assert_equal( 22, s.a.y )
    assert_equal( 23, s.a.z )
    assert_equal( 31, s.b.x )
    assert_equal( 32, s.b.y )
    assert_equal( 33, s.b.z )
    assert_equal( Test::E1::A, s.c )
  end 
  
  def test_excps
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_Excp1 )
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_Excp3 )
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_Excp4 )
    
    checkField( ValueFactoryTest::MF_msg )
    checkField( ValueFactoryTest::MF_code )
  end
  
  def test_excps_export
    sv = @vf.exportCustomValue( Test::Excp1.new( "abc", 23 ) )
    sv.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_Excp1 )
    assert_equal( 2, sv.size )
    assert_equal( "abc", sv[ ValueFactoryTest::MF_msg ] )
    assert_equal( 23, sv[ ValueFactoryTest::MF_code ] )
    
    sv = @vf.exportCustomValue( Test::Excp3.new() )
    sv.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_Excp3 )
    assert_equal( 0, sv.size )
    
    sv = @vf.exportCustomValue( Test::Excp4.new() )
    sv.checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_Excp4 )
    assert_equal( 0, sv.size )
    
  end
  
  def test_expcs_import
    sv = StructValue.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_Excp1 )
    sv.store( ValueFactoryTest::MF_msg, "def" )
    sv.store( ValueFactoryTest::MF_code, 29 )
    e1 = @vf.importCustomValue( sv )
    assert_equal( "def", e1.msg )
    assert_equal( 29, e1.code )
    e1 = nil 
    
    sv = StructValue.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_Excp3 )
    e3 = @vf.importCustomValue( sv )
    assert_not_nil( e3 )
    e3 = nil 
    
    sv = StructValue.new( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_Excp4 )
    e4 = @vf.importCustomValue( sv )
    assert_not_nil( e4 )
    e4 = nil 
  end
  
  def test_method_nothing
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_nothing )
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_nothing )
    checkField( ValueFactoryTest::MF_result )
  end
  
  def test_method_incr
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_incr )
    checkField( ValueFactoryTest::MF_x ) 
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_incr )
    checkField( ValueFactoryTest::MF_result )
  end
  
  def test_method_sub
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_sub )
    checkField( ValueFactoryTest::MF_x ) 
    checkField( ValueFactoryTest::MF_y )
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_sub )
    checkField( ValueFactoryTest::MF_result )
  end
  
  def test_method_sum
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_sum )
    checkField( ValueFactoryTest::MF_x ) 
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_sum )
    checkField( ValueFactoryTest::MF_result )
  end
  
  def test_method_trans
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_trans )
    checkField( ValueFactoryTest::MF_e ) 
    checkField( ValueFactoryTest::MF_y )
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_trans )
    checkField( ValueFactoryTest::MF_result )
  end
  
  def test_method_dist
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_dist )
    checkField( ValueFactoryTest::MF_a ) 
    checkField( ValueFactoryTest::MF_b )
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_dist )
    checkField( ValueFactoryTest::MF_result )
  end
  
  def test_method_fill
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_fill )
    checkField( ValueFactoryTest::MF_n ) 
    checkField( ValueFactoryTest::MF_x )
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_fill )
    checkField( ValueFactoryTest::MF_result )
  end
  
  def test_method_blow
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_blow )
    checkField( ValueFactoryTest::MF_msg ) 
    checkField( ValueFactoryTest::MF_code )
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_blow )
    checkField( ValueFactoryTest::MF_result )
  end
  
  def test_method_beets
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test_beets )
    checkField( ValueFactoryTest::MF_e )
    checkType( ValueFactoryTest::MT_etch_bindings_ruby_test_Test__result_beets )
    checkField( ValueFactoryTest::MF_result )
  end
  
  
  
  #
  # utility methods
  # 
  def checkType( t )
    assert_not_nil( t )
    assert_same( Type, t.class )
    assert_same( t, @vf.getType( t.xid ) )
  end
  
  def checkField( f )
    assert_not_nil( f )
    assert_same( Field, f.class )
    assert_same( f, @vf.getField( f.xid ) )
  end
  
  def testEnumExport( e, t, f )
    sv = @vf.exportCustomValue( e )
    sv.checkType( t )
    assert_equal( 1, sv.size )
    assert( sv[f] )
  end
  
  def testEnumImport( e, t, f )
    sv = StructValue.new( t )
    sv.store( f, true )
    a = @vf.importCustomValue( sv )
    assert_same( e, a )
  end
end