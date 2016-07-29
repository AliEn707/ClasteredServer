require 'socket'
require 'io/wait'
require_relative "clastered_server_packet"

class Area
	def initialize(c, size, index, scale=1)
		p @counts=c
		@size=size
		@scale=scale
		@index=index
		@l,@t=@size/@counts[0]*(index% @counts[0]),@size/@counts[1]*((index) / @counts[0]) #@r>@l
		@r,@b=@l+@size/@counts[0],@t+@size/@counts[1] #@b>@t
		p "#{@index},( #{@l},#{@t}), #{@r},#{@b}"
	end
	
	def check?(x,y)
		@l<=x && @r>x && @t<=y && @b>y
	end
	
	def shares(x,y)
		o=[]
		ix,iy=@index% @counts[0],@index/ @counts[0]
		shift=1*@scale
		if check?(x,y) 
			if @l<=x && x<=@l+shift
				x1=ix-1
				y1=iy
				o<<((x1)*@counts[0]+y1) if x1>=0 && x1<@counts[0] && y1>=0 && y1<@counts[1]
			end
			if @r>x && x>=@r-shift 
				x1=ix+1
				y1=iy
				o<<((x1)*@counts[0]+y1) if x1>=0 && x1<@counts[0] && y1>=0 && y1<@counts[1]
			end
			if @t<=y && y<=@t+shift 
				x1=ix
				y1=iy-1
				o<<((x1)*@counts[0]+y1) if x1>=0 && x1<@counts[0] && y1>=0 && y1<@counts[1]
			end
			if @b>y && y>=@b-shift
				x1=ix
				y1=iy+1
				o<<((x1)*@counts[0]+y1) if x1>=0 && x1<@counts[0] && y1>=0 && y1<@counts[1]
			end
		end
		return o
	end
end

class Grid
	def initialize(size, scale=1)
		@size=size
		@scale=scale
		@grid=[]
		@server_ids=[]
	end
	
	def set_servers(a)
		@server_ids=a.uniq.sort
		@servers=@server_ids.each_with_index.inject({}){|o, z|
			id, i=z
			o.merge!(id => {id: id, index: i})
		}
		set_parting
		return self
	end
	
	def check_owner(x,y)
		@grid[to_grid(x,y)][:owner]
	end

	def check_shares(x,y)
		@grid[to_grid(x,y)][:shares]
	end

	private
	def to_grid(x,y)
		x.to_i.round*@size+y.to_i
	end

	def set_parting
		@counts=[(0..Math.sqrt(@server_ids.size).round).inject{|o,i| (i if @server_ids.size.to_f/i==(@server_ids.size/i).to_i)||o}]
		@counts<<@server_ids.size/@counts[0]
		@servers.each{|k,v|
			p v[:area]=Area.new(@counts, @size, v[:index])
		}
		@grid=[]
		@size.times{|x|
			@size.times{|y|
				index=to_grid(x,y)
				@grid[index]={}
				@servers.each{|k, s|
					if s[:area].check?(x,y)
						@grid[index][:owner]=s[:id]
						@grid[index][:shares]=s[:area].shares(x,y).map!{|e| @server_ids[e]}
					end
				}
			}
		}
	end	
end

grid =Grid.new(10)
grid.set_servers([1,2,3,4,5])

5.times{|x|
	y=x
	p "#{x},#{y},#{grid.check(x,y)}"
}
qq

server = TCPServer.new(12345)
 
packet=ClasteredServerPacket.new

while (connection = server.accept)
	Thread.new(connection) do |conn|
		port, host = conn.peeraddr[1,2]
		client = "#{host}:#{port}"
		puts "#{client} is connected"
		begin
			loop do
				if (conn.ready?)
					packet.init
					packet.recv(conn, true)
					p packet.parse
				end
				if(Time.now.to_i%2==0 && false)
					packet.init.set_type(1).add_int(2).add_string("test")
				end#        line = conn.readline
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