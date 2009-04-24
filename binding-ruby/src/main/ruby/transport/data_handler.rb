module DataHandler
  
  #Interface used to deliver data from a stream source.
  #
  # Delivers data from a stream source.
  # @param src
  # @param sender
  # @param buf
  # @throws Exception
  #
   
  def data( src, sender, buf )
    raise "subclass responsibility"
  end
  
end