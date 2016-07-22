require 'socket'
require_relative "clastered_server_packet"
server = TCPServer.new(12345)
 
packet=ClasteredServerPacket.new

 
while (connection = server.accept)
  Thread.new(connection) do |conn|
    port, host = conn.peeraddr[1,2]
    client = "#{host}:#{port}"
    puts "#{client} is connected"
    begin
      loop do
		  packet.init
		  packet.recv(conn, true)
		  p packet.parse
#        line = conn.readline
#        line = conn.read
#        puts "#{client} says: #{line}"
#        conn.write(line)
#        conn.write(conn.read(1))
      end
    rescue EOFError => e
		p e
      conn.close
      puts "#{client} has disconnected"
    end
  end
end