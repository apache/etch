def callBlock
  yield
  yield
end

callBlock { puts "Hai Badri" }

a = %w( Badri Narayanan Champakesan )
a.each { | name | print name, " " }

line = gets
#print line
puts line
print
