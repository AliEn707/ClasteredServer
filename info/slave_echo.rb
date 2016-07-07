require 'socket'
server = TCPServer.new(12345)
 
while (connection = server.accept)
  Thread.new(connection) do |conn|
    port, host = conn.peeraddr[1,2]
    client = "#{host}:#{port}"
    puts "#{client} is connected"
    begin
      loop do
#        line = conn.readline
#        puts "#{client} says: #{line}"
#        conn.write(line)
        conn.write(conn.read(1))
      end
    rescue EOFError
      conn.close
      puts "#{client} has disconnected"
    end
  end
end