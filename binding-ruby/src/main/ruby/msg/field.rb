# package 'etch/bindings/ruby/msg'
require 'etch/bindings/ruby/msg/id_name'

# Field is an IdName which denotes a field of a struct
# or message (i.e., a key for a value).

class Field < IdName
  
  # Constructs the Field.
  # @param id the id of the field.
  # @param name the name of the field.
  
  def initialize(id, name)
    super(id, name)
  end
  
  # Constructs the Field, computing the appropriate value
  # for the id.
  # @param name the name of the field.
  
  # def initialize(name)
  #  super(name)
  # end
  
end