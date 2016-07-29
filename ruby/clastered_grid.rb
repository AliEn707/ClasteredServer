class ClasteredGrid

	def initialize(size, offset)
		@size=size.map{|e| e.to_f}
		@scale=1
		@offset=offset
	end
	
	def set_servers(s)
		@server_ids=s.sort
		@servers={}
		size=@server_ids.size
		@counts=[(1..Math.sqrt(size).round).inject{|o,i| 
			p i,o;(i if (size.to_f/i)==(size.to_f/i).to_i)||o
		}]
		@counts<<size/@counts[0]
		p @counts
		p area_size=[@size[0]/@counts[0],@size[1]/@counts[1]]
		#need to find nok
	end
end

grid=ClasteredGrid.new([80,50],3)
grid.set_servers([1,2,3])

