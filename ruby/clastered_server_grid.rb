class ClasteredServer
	class Grid

		def initialize(size, offset)
			@size=size.map{|e| e.to_f}
			@offset=offset
			@cell=[0,0]
			@data=[]
			@data_size=[0,0]
			@id=0
		end
		
		def get_owner(x,y)
			return (@data[to_grid(x/@cell[0],y/@cell[1])]||{})[:owner]
		end
		
		def get_shares(x,y)
			return (@data[to_grid(x/@cell[0],y/@cell[1])]||{})[:shares]
		end
		
		def reconfigure(s, id=0)
			@id=id
			@server_ids=s.sort
			servers={}
			size=@server_ids.size
			counts=[(1..Math.sqrt(size).round).inject{|o,i| 
				(i if (size.to_f/i)==(size.to_f/i).to_i)||o
			}]
			counts<<size/counts[0]
			counts=[counts[1],counts[0]] if (@size[0]>@size[1])#int 
			#p counts
			#p [@size[0]/counts[0],@size[1]/counts[1]]
			@cell=[]
			2.times{|i|
				l=@size[i]/counts[i]
				d=@offset
				1.step(1000000, 1){|n|#TODO: change
					a=l/n
					m=(d/a).ceil
					if (d<=m*a && 1.1*d>=m*a)
						@cell<<a
						break
					end
				}
			}
			#p @cell
			#p (@offset/@cell[1]).ceil
			@data_size=[(@size[0]/@cell[0]).to_i,(@size[1]/@cell[1]).to_i]
			#need to find nok
			@server_ids.each_with_index{|id, i|
				servers[id]={
					id: id, 
					index: i, 
					area:{
						l: @size[0]/@cell[0]/counts[0]*(i% counts[0]),
						t: @size[1]/@cell[1]/counts[1]*(i/ counts[0]),
						r: @size[0]/@cell[0]/counts[0]*(1+(i% counts[0])), 
						b: @size[1]/@cell[1]/counts[1]*(1+(i/ counts[0]))
					}
				}
			}
			cells=[]
			@data=Array.new(@data_size[0]*@data_size[1])
			(@data_size[1]).times{|y|
				(@data_size[0]).times{|x|
					s=servers[@server_ids[x/(@data_size[0]/counts[0])+y/(@data_size[1]/counts[1])*counts[0]]]
					o={}
					o[:owner]=s[:id]
					o[:shares]=[]
					if (@id==0 || s[:id]==@id)
						ix,iy=s[:index]% counts[0],s[:index]/ counts[0]
						offset=[(@offset/@cell[0]).ceil,(@offset/@cell[1]).ceil]
						l=s[:area][:l]<=x && x<s[:area][:l]+offset[0]
						t= s[:area][:t]<=y && y<s[:area][:t]+offset[1]
						r=s[:area][:r]>x && x>=s[:area][:r]-offset[0]
						b=s[:area][:b]>y && y>=s[:area][:b]-offset[1]
						if l
							x1=ix-1
							y1=iy
							o[:shares]<<@server_ids[(y1)*counts[0]+x1] if x1>=0 && x1<counts[0] && y1>=0 && y1<counts[1]
							if t
								x1=x1
								y1=y1-1
								o[:shares]<<@server_ids[(y1)*counts[0]+x1] if x1>=0 && x1<counts[0] && y1>=0 && y1<counts[1]
							end
						end
						if t
							x1=ix
							y1=iy-1
							o[:shares]<<@server_ids[(y1)*counts[0]+x1] if x1>=0 && x1<counts[0] && y1>=0 && y1<counts[1]
							if r
								x1=x1+1
								y1=y1
								o[:shares]<<@server_ids[(y1)*counts[0]+x1] if x1>=0 && x1<counts[0] && y1>=0 && y1<counts[1]
							end
						end
						if r
							x1=ix+1
							y1=iy
							o[:shares]<<@server_ids[(y1)*counts[0]+x1] if x1>=0 && x1<counts[0] && y1>=0 && y1<counts[1]
							if b
								x1=x1
								y1=y1+1
								o[:shares]<<@server_ids[(y1)*counts[0]+x1] if x1>=0 && x1<counts[0] && y1>=0 && y1<counts[1]
							end
						end
						if b
							x1=ix
							y1=iy+1
							o[:shares]<<@server_ids[(y1)*counts[0]+x1] if x1>=0 && x1<counts[0] && y1>=0 && y1<counts[1]
							if l
								x1=x1-1
								y1=y1
								o[:shares]<<@server_ids[(y1)*counts[0]+x1] if x1>=0 && x1<counts[0] && y1>=0 && y1<counts[1]
							end
						end
					end
					@data[to_grid(x,y)]= (i=cells.index(o)) && cells[i] || (cells<<o) && o
				}
			}
			#p @data
			return self
		end
		
		private 

		def to_grid(x,y)
			#puts "data #{@data_size[0]} (#{x},#{y}) on (#{x/@cell[0]},#{y/@cell[1]})"
			index=(y.to_i*@data_size[0]).to_i+(x).to_i
			index>0 ? index : 0
		end
	end
end

#data=ClasteredServer::Grid.new([6,8],2)
#data.reconfigure([1,2,3,4])
data=ClasteredServer::Grid.new([32000,32000],20)
data.reconfigure([1,24,-39,45,15,36], 24)

#0.step(8,0.5){|x|
#	0.step(8,0.5){|y|
#		puts "(#{x},#{y}) #{data.get_owner(x,y)}|#{data.get_shares(x,y)}"
#	}
#}
