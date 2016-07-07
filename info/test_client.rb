require 'socket'      # Sockets are in standard library

#puts "hostname"
hostname = "localhost"#gets.chomp
#puts "port"
port = 8000#gets.to_i

s = TCPSocket.open(hostname, port)

#puts connected
s.write [0].pack("v")
s.write [0].pack("c")
s.write [1].pack("c")
s.write [6].pack("c")
s.write [4].pack("v")
s.write "qwer"
#s.write [4232].pack("l")
#s.write [1].pack("l")
p a=s.read(6).unpack("vcccc")
#p s.read(1)
#while line = s.gets   # Read lines from the socket
  #puts line.chop      # And print with platform line terminator
#end

#sleep(5)

s.close               # Close the socket when done
