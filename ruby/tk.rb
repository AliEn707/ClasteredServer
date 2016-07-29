require 'tk'

root = TkRoot.new
root.title = "Window"

canvas = TkCanvas.new(root) do
   place('height' => 170, 'width' => 100, 
          'x' => 10, 'y' => 10)
end

root.bind('Button',
             proc{|b,x,y,root_x,root_y|
               puts b,x,y,root_x,root_y
             }, "%b %x %y %X %Y")

TkcRectangle.new(canvas, 10,  5,    55,  50, 
                         'width' => 1)
TkcRectangle.new(canvas, 10,  65,  55, 110, 
                         'width' => 5) 
TkcRectangle.new(canvas, 10,  125, 55, 170, 
                         'width' => 1, 'fill'  => "red") 
TkcLine.new(canvas, 0, 5, 100, 5)
TkcLine.new(canvas, 0, 15, 100, 15, 'width' => 2)
TkcLine.new(canvas, 0, 25, 100, 25, 'width' => 3)
TkcLine.new(canvas, 0, 35, 100, 35, 'width' => 4)

Thread.new{
	i=0
	loop{
		TkcRectangle.new(canvas, 10+i,  5,    55-i,  50, 
                         'width' => 1)
		i+=1
		sleep(1)
		canvas.delete("all")
	}
}

Tk.mainloop