# package etch/bindings/ruby/support

# Interface which a service implementation may implement which
# is used by the service stub to give notice of a closed message
# source.

module Close

  # Notifies the service implementation that the message
  # source is closed.
  # @param src the message source that is closed.
  #
  def _close( src )
    raise "subclasser responsibility"
  end
  
end
