require 'socket'

require 'etch/bindings/ruby/msg/array_value.rb'
require 'etch/bindings/ruby/msg/type.rb'
require 'etch/bindings/ruby/msg/value_factory.rb'

class TaggedData
    attr :vf
    protected :vf
    
    def initialize( vf )
        @vf = vf
    end
    
    def getValueFactory()
        return @vf
    end
    
    def toArrayValue( value )
        c = value.class
        dim = 0
        while( c.is_a?(Array))
          dim += 1
          c = c.getComponentType() # to be found what to do!
        end  
    end
end