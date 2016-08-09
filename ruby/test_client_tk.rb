require_relative "clastered_server_drawer"
require_relative "clastered_server_packet"

include ClasteredServer

class World
	attr_reader :drawer, :size
	attr_accessor :npcs
	def initialize(x,y, cell)
		@scale=600.0/x
		@size=[x,y]
		@drawer=Drawer.new(600,y*@scale,0,0, scale: 1/@scale, translate:{x: 0,y: 0})
		@npcs={}
		@cell=cell
		@objects=[]
	end
	def remove(x)
		@drawer.remove(x)
	end
	
	def bind(*args)
		@drawer.bind(*args)
	end
	
end

class Npc
	attr_accessor :bot, :keys
	attr_reader :id, :obj, :position
	def initialize(world, id)
		@id=id
		@world=world
		@obj=@world.drawer.c_rpoly(20, 70, 5, 5, state: 'hidden', fill:"white", outline: "black")
		@dir=[0,0]
		@goal=[0,0]
		@position=[0,0]
		@vel=1
		@keys=[0,0,0,0]
	end
	
	def move(x,y)
		@position[0]+=x
		@position[1]+=y
		@obj.move(x,y)
		if @bot
			if (@position[0]-@goal[0])**2+(@position[1]-@goal[1])**2<=@vel**2
				@goal=[rand*@world.size[0], rand*@world.size[1]]
				set_dir(@goal[0]-@position[0], @goal[1]-@position[1])
			end
		end
	end
	
	def walk
		move(@dir[0]*@vel, @dir[1]*@vel)
	end
	
	def update_position(x,y)
		move(x-@position[0],y-@position[1])
	end
	
	def set_dir(x=nil,y=nil)
		@dir[0]=x||@keys[3]-@keys[2]
		@dir[1]=y||@keys[1]-@keys[0]
		normalize_dir
	end
	
	def hide!
		@obj.state="hidden"
	end
	
	def show!
		@obj.state="normal"
	end
	
	def bind(*args)
		@obj.bind(*args)
	end
	private
	
	def normalize_dir
		if (l=Math.sqrt(@dir[0]*@dir[0]+@dir[1]*@dir[1]))!=0
			@dir[0]/=l
			@dir[1]/=l
		end
	end
end

def time_diff(start, finish)
   (finish - start)
end

#########

world=World.new(600,600,0)
drawer=world.drawer

#mouse event listener


=begin
drawer.c_rect(10,  5,    55,  50, 
                         'width' => 1)
drawer.c_rect(10,  65,  55, 110, 
                         'width' => 5) 
drawer.c_rect(10,  125, 55, 170, 
                         'width' => 1, 'fill'  => "red") 
drawer.c_line(0, 5, 100, 5)
drawer.c_line(0, 15, 100, 15, 'width' => 2)
drawer.c_line(0, 25, 100, 25, 'width' => 3)
drawer.c_line(0, 35, 100, 35, 'width' => 4)
=end
n=Npc.new(world,-1)
n.show!

events={
	"w"=>0,
	"s"=>1,
	'a'=>2,
	'd'=>3
}

world.bind("KeyPress", 
	proc{|e| 
		if events[e]
			if n.keys[events[e]]!=1
				n.keys[events[e]]=1
				n.set_dir
			end
		end
	},
"%K")
world.bind("KeyRelease", 
	proc{|e| 
		if events[e]
			if n.keys[events[e]]!=0
				n.keys[events[e]]=0
				n.set_dir
			end
		end
	},
"%K")

world.npcs[n.id]=n

100.times{|i|
	npc=Npc.new(world,i)
	npc.show!
	npc.bot=true
	npc.obj.fill="green"
	world.npcs[npc.id]=npc
}

lat=1.0/30

Thread.new{
	packet=ClasteredServerPacket.new
	loop{
		t1=Time.now
		world.npcs.each{|k,v|
		begin
			packet.init
			packet.recv(conn, false)
			p a=packet.parse
			case a[0]
				when 2 #client connected
					#do some stuff
				when 40
					cl=npc[a[2]]
					if cl
						cl.update_position(a[3],a[4])
						#do some stuff
					end
				else
					puts "unknown packet"
			end
		rescue Exeption=> e
			puts e
			puts backtrace
		end
		}
		dif=time_diff(t1,Time.now)
		sleep(lat-dif) if (lat-dif>0)
	}
}

Drawer.loop
