require 'test/unit'
require 'etch/bindings/ruby/transport/who.rb'
require 'etch/bindings/ruby/transport/data_handler.rb'
require 'etch/bindings/ruby/transport/data_source'
require 'etch/bindings/ruby/transport/packet_handler.rb'
require 'etch/bindings/ruby/transport/packet_source.rb'
require 'etch/bindings/ruby/transport/packetizer.rb'
require 'etch/bindings/ruby/transport/flex_buffer.rb'

class TestPacketizer
    
      @@mph = MyPacketHandler.new
      @@p  = Packetizer.new( mph, Packetizer.USE_DEFAULT_MAX_PKT_SIZE )
      
    def setUp() 
        p.setDataSource( @@mph )
    end
    
    def sendPacket()
        buf = FlexBuffer.new( Array.new( 0,0,0,0,0,0,0,0 ))
        result = Array.new{Array.new( -17, -66, -83, -34, 0, 0, 0, 0 )}
        @@p.packet(nil, buf)
        assert_same( @@mph.what, what::DATA )
#        assert_true(mph.check(result))
        assert_nil(@@mph.xsender)
        assert_nil(@@mph.xsrc)
        assert_nil(@@mph.xbuf)
    end
    
    def sendPacket1()
        buf = FlexBuffer.new(Array.new( 0, 0, 0, 0, 0, 0, 0, 0, 1 ))
        result = Array.new{Array.new( -17, -66, -83, -34, 1, 0, 0, 0, 1 )}
        @@p.packet(nil, buf)
        assert_same( @@mph.what, what.DATA )
#        assertTrue(mph.check(result))
        assert_nil(@@mph.xsender)
        assert_nil(@@mph.xsrc)
        assert_nil(@@mph.xbuf)
    end
    
      def sendPacket2()
#       Create packet to send
         buf = FlexBuffer.new(Array.new( 0, 0, 0, 0, 0, 0, 0, 0, 2, 3 ))
         result = Array.new{Array.new( -17, -66, -83, -34, 2, 0, 0, 0, 2, 3 )}
    
         @@p.packet(nil, buf)
         assert_same( @@mph.what, what::DATA )
#        assertTrue(@@mph.check(result))
         assert_nil(@@mph.xsender)
         assert_nil(@@mph.xsrc)
         assert_nil(@@mph.xbuf)
      end
      
      def  sendSingleSingleData0()
#    // Create data to send
          buf = FlexBuffer.new(Array.new(-17, -66, -83, -34, 0, 0, 0, 0 ))
          result = Array.new{Array.new}
    
          @@p.data(@@mph, nil, buf)
          assertTrue(@@mph.check(result))
    
          assert_nil(@@mph.what)
          assert_nil(@@mph.xsender)
          assert_nil(@@mph.xsrc)
          assert_nil(@@mph.xbuf)
      end
      
        def sendSingleSingleData1()
#           length = 1
            buf = FlexBuffer.new(Array.new( -17, -66, -83, -34, 1, 0, 0, 0, 1 ))
            result = Array.new{Array.new( 1 ) }
            
            @@p.data(@@mph, nil, buf)
#            assertTrue(@@mph.check(result))
            
            assert_same(@@mph.what, what::PACKET)
            assert_nil(@@mph.xsender)
            assert_nil(@@mph.xsrc)
            assert_nil(@@mph.xbuf)
        end
        
        def sendSingleSingleData2()
#            // length = 2
            buf = FlexBuffer.new(Array.new( -17, -66, -83, -34, 2, 0, 0, 0, 3, 4 ))
            result = Array.new{Array.new( 3, 4 )}
            
            @@p.data(@@mph, nil, buf)
#            assertTrue(mph.check(result))
            
            assert_same(@@mph.what, what::PACKET)
            assert_nil(@@mph.xsender)
            assert_nil(@@mph.xsrc)
            assert_nil(@@mph.xbuf)
        end
        
        def sendDoubleSingleData0()
#              // length = 0
              buf = FlexBuffer.new(Array.new( -17, -66, -83, -34, 0, 0, 0, 0, -17, -66, -83, -34, 0, 0, 0, 0 ))
              result = Array.new{Array.new}
              
              @@p.data(@@mph, nil, buf)
              
#              assertTrue(@@mph.check(result))
              assert_nil(@@mph.what)
              assert_nil(@@mph.xbuf)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xsender)
        end
        
        def sendDoubleSingleData1()
#              // length = 1
              buf = FlexBuffer.new(Array.new(-17, -66, -83, -34, 1, 0, 0, 0, 1, -17, -66, -83, -34, 1, 0, 0, 0, 2 ))
              result = Array.new{Array.new((1), (2))}
              
              @@p.data(@@mph, nil, buf)
              
#              assertTrue(@@mph.check(result))
              assert_same(@@mph.what, what::PACKET)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
        end
        
        def sendDoubleSingleData2()
#          // length = 2
              buf = FlexBuffer.new(Array.new( -17, -66, -83, -34, 2, 0, 0, 0, 3, 4, -17, -66, -83, -34, 2, 0, 0, 0, 5, 6 ))
              result = Array.new{Array.new({3, 4}, {5, 6})} 
              
              @@p.data(@@mph, nil, buf)
              
#              assertTrue(@@mph.check(result))
              assert_same(@@mph.what, what::PACKET)
              assert_nil(@@mph.xbuf)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xsender)
        end
        
        def sendDoubleSingle_HeaderSplit_Data0()
#              // length = 0
              buf = FlexBuffer.new(Array.new( -17, -66, -83, -34, 0, 0 ))
              result = Array.new{Array.new} 
              
              @@p.data(@@mph, nil, buf)
              
#              assertTrue(@@mph.check(result))
              assert_nil(@@mph.what)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
              
              buf2 = FlexBuffer.new(Array.new( 0, 0, -17, -66, -83, -34, 0, 0, 0, 0))
              result2 = Array.new{Array.new}
              
              @@p.data(@@mph, nil, buf2)
              
#              assertTrue(mph.check(result2))
              assert_nil(@@mph.what)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
        end
        
        def sendDoubleSingle_HeaderSplit_Data1()
#              // length = 1
              buf = FlexBuffer.new(Array.new( -17, -66, -83, -34, 1, 0 ))
              result = Array.new{Array.new} 
              
              @@p.data(@@mph, nil, buf)
              
#              assertTrue(@@mph.check(result))
              assert_nil(@@mph.what)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
              
              buf2 = FlexBuffer.new(Array.new( 0, 0, 1, -17, -66, -83, -34, 1, 0, 0, 0, 2 ))
              result2 = Array.new{Array.new(( 1 ), ( 2 ))}
              
              @@p.data(@@mph, nil, buf2)
              
#              assertTrue(mph.check(result2))
              assert_same(@@mph.what, what::PACKET)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
        end
        
        def sendDoubleSingle_HeaderSplit_Data2()
#              // length = 2
              buf = FlexBuffer.new(Array.new( -17, -66, -83, -34, 2, 0 ))
              result = Array.new{Array.new}
              
              @@p.data(@@mph, nil, buf)
              
#              assertTrue(mph.check(result))
              assert_nil(@@mph.what)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
              
              buf2 = FlexBuffer.new(Array.new(0, 0, 3, 4, -17, -66, -83, -34, 2, 0, 0, 0, 5, 6 ))
              result2 = Array.new{Array.new({3, 4}, {5, 6}) }
              
              @@p.data(@@mph, nil, buf2)
              
#              assertTrue(mph.check(result2))
              assert_same(@@mph.what, what::PACKET)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
        end
        
        def sendDoubleSingle_BodySplit_Data2()
#              // length = 2
              buf = FlexBuffer.new(Array.new( -17, -66, -83, -34, 2, 0, 0, 0, 1 ))
              result = Array.new{Array.new} 
              
              @@p.data(@@mph, nil, buf)
              
#              assertTrue(@@mph.check(result))
              assert_nil(@@mph.what)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
              
              buf2 = FlexBuffer.new(Array.new( 2, -17, -66, -83, -34, 2, 0, 0, 0, 3, 4  ))
              result2 = Array.new{ Array.new({ 1, 2 }, { 3, 4 })}
              
              @@p.data(@@mph, nil, buf2)
              
#              assertTrue(mph.check(result2))
              assert_same(@@mph.what, what::PACKET)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
        end
        
        def sendDoubleSingle_BodySplit_Data3()
#                  // length = 3
             buf = FlexBuffer.new(Array.new( -17, -66, -83, -34, 3, 0, 0, 0, 5, 6 ))
             result = Array.new{Array.new} 
                  
             @@p.data(@@mph, nil, buf)
                  
#              assertTrue(mph.check(result))
              assert_nil(@@mph.what)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
                  
              buf2 = FlexBuffer.new(Array.new( 7, -17, -66, -83, -34, 3, 0, 0, 0, 8, 9, 10 ))
              result2 = Array.new{Array.new({ 5, 6, 7, 0 }, { 8, 9, 10, 0 })} # odd number list for hash
                  
              @@p.data(@@mph, nil, buf2)
                  
#              assertTrue(mph.check(result2))
              assert_same(@@mph.what, what::PACKET)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
        end
        
        def up()        
           @@p.up(@@mph)              
           assert_same(@@mph.what, what::UP)
           assert_nil(@@mph.xbuf)
           assert_nil(@@mph.xsrc)
           assert_nil(@@mph.xsender)
        end
        
        def down()
            @@p.down(@@mph)
            assert_same(@@mph.what, what::DOWN)
            assert_nil(@@mph.xbuf)
            assert_nil(@@mph.xsrc)
            assert_nil(@@mph.xsender)
        end
        
        def close()
            @@p.close(true)            
            assert_same(@@mph.what, what::RESET)
            assert_nil(@@mph.xsender)
            assert_nil(@@mph.xsrc)
            assert_nil(@@mph.xbuf)
        end
        
        def localAddress()
              @@p.localAddress()
              assert_same(@@mph.what, what::LOCALADDRESS)
              assert_nil(@@mph.xsender)
              assert_nil(@@mph.xsrc)
              assert_nil(@@mph.xbuf)
        end
        
        def remoteAddress()
            @@p.remoteAddress()
            assert_same(@@mph.what, what::REMOTEADDRESS)
            assert_nil(@@mph.xsender)
            assert_nil(@@mph.xsrc)
            assert_nil(@@mph.xbuf)
        end
        
        def shutdownInput()
            @@p.shutdownInput()
            
            assert_same(@@mph.what, what::SHUTDOWNINPUT)
            assert_nil(@@mph.xsender)
            assert_nil(@@mph.xsrc)
            assert_nil(@@mph.xbuf)
        end
        
        def shutdownOutput()
            @@p.shutdownOutput() 
            assert_same(@@mph.what, what::SHUTDOWNOUTPUT)
            assert_nil(@@mph.xsender)
            assert_nil(@@mph.xsrc)
            assert_nil(@@mph.xbuf)
        end
        
        def stop()
            @@p.stop()            
            assert_same(@@mph.what, what::STOP)
            assert_nil(@@mph.xsender)
            assert_nil(@@mph.xsrc)
            assert_nil(@@mph.xbuf)
        end
        
        $what = Enum.new(:DATA, :PACKET, :UP, :DOWN, :RESET, :LOCALADDRESS, :REMOTEADDRESS, 
                          :SHUTDOWNINPUT, :SHUTDOWNOUTPUT, :STOP, :STARTED, :STOPPED, :EXCEPTION)
                          
        class MyPacketHandler 
              include DataSource, PacketHandler
            
            attr :what
#             public Enum what
             public @@xsrc = PacketSource.new 
             public @@xsender = Who.new
             public @@xbuf = FlexBuffer.new
             @@list = Array.new
              
             def clear()
                return @list.clear()
             end
             
             def check(result = Array.new{Array.new})
                    flag = check( @@list.size() == result.length )
                    if (flag == false)
                      return flag
                    end
                    
                    for i in ( 0..list.size ) 
                      flag = check( @@list.get( i ), result[i] )
                      if (flag == false)
                        return flag
                      end
                    end
                    return true
             end
             
             def check( a = Array.new, b = Array.new )
                 flag = check( a.length == b.length )
                  if (flag == false)
                    return flag
                  end
                  
                  for i in (0..a.length) 
                    flag = check( a[i] == b[i] )
                    if (flag == false)
                      return flag
                    end
                  end
                  return true
             end  
             
             def check(ok )
                return ok
             end
             
             def packet( src, sender, buf )
                  @what = What::PACKET; 
                  buf = FlexBuffer.new
                  @@list.add( buf.getAvailBytes() );
             end
             
             def up( s )
                @what = $what::UP
             end
             
             def down(s)
                @what = $what::DOWN
             end

             def started( r )
                @what = $what::STARTED
             end
             
            def stopped( r )
                @what = $what::STOPPED
            end
            
            def exception( r, string, e )
                @what = $what::EXCEPTION
            end
            
            def data( recipient, buf )
                @what = $what::DATA
                buf = FlexBuffer.new
                @@list.add( buf.getAvailBytes() )
            end
            
            def localAddress()
                @what = $what::LOCALADDRESS;
                return nil
            end
            
            def remoteAddress()
                @what = $what::REMOTEADDRESS;
                return nil
            end
            
            def shutdownInput()
                @what = $what::SHUTDOWNINPUT
            end
            
            def shutdownOutput()
                @what = $what::SHUTDOWNOUTPUT
            end
            
            def close( reset )
#                assertTrue(reset);
                @what = $what::RESET
            end
            
            def stop()
                @what = $what::STOP
            end
            
            def setHandler(handler)
                # do nothing 
            end
        end
end 