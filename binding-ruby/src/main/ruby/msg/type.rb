# package 'etch/bindings/ruby/msg'

class Type < IdName

  
  # Constructs the Type.
  # @param id the id of the field.
  # @param name the name of the field.
  
  def initialize(id, name)
    super(id, name)
  end
  
  # Constructs the Type, computing the appropriate value
  # for the id.
  # @param name the name of the type.
=begin  
  def initialize(name)
    super(name)
  end
=end
end