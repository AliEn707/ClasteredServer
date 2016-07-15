require 'socket'      # Sockets are in standard library
require 'digest'
require "base64"

#puts "hostname"
hostname = "localhost"#gets.chomp
#puts "port"
port = 8000#gets.to_i

s = TCPSocket.open(hostname, port)

#puts connected
s.write [9].pack("v")
s.write [0].pack("c")
s.write [1].pack("c")
s.write [6].pack("c")
s.write [4].pack("v")
s.write "qwer"
#s.write [4232].pack("l")
#s.write [1].pack("l")
p a=s.read(6).unpack("vcccc")
s.write [10].pack("v")
s.write [1].pack("c")
s.write [1].pack("c")
s.write [6].pack("c")
s.write [1].pack("c")
s.write [4].pack("v")
s.write "qwer"
p a=s.read(7).unpack("vcccv")
p b64=s.read(a[-1])
passwd="qwer"
p ans=Base64.encode64(Digest::MD5.digest(Base64.decode64(b64)+Digest::MD5.digest(passwd))).chomp
s.write [ans.size+7].pack("v")
s.write [1].pack("c")
s.write [2].pack("c")
s.write [1].pack("c")
s.write [6].pack("c")
s.write [2].pack("c")
s.write [ans.size].pack("v")
s.write ans
p size=s.read(2).unpack("v")[0]
p s.read(size)
#p s.read(1)
#while line = s.gets   # Read lines from the socket
  #puts line.chop      # And print with platform line terminator
#end

#sleep(5)

s.close               # Close the socket when done


=begin
md5=Digest::MD5.digest("hello")
Base64.encode64(md5)
Base64.decode64(md5)

passwd="hello" #- password
b64 #token from server
Base64.encode64(Digest::MD5.digest(Base64.decode64(b64)+Digest::MD5.digest(passwd)))

=end