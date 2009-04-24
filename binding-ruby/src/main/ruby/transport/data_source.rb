require 'etch/bindings/ruby/transport/source.rb'

module DataSource 
      include Source
  
  # Interface used to model a stream source to a data recipient, which
  # allows the recipient to send data to the peer.
  #
  #  Delivers data to the peer via the data stream.
  # @param recipient
  # @param buf
  # @throws Exception
  #
  def data( recipient, buf ) 
    raise "Subclass responsibility"
  end
   
end

