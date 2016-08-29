package clasteredServerClient;

#if cpp
import cpp.vm.Thread;
#elseif neko
import neko.vm.Thread;
#elseif java
import java.vm.Thread;
#end


/**
 * ...
 * @author ...
 */
class Connector{
	#if use_threads
	private var t:Thread;
	#end
	
	private function connector(){
		var main:Thread = Thread.readMessage(true);
		var c:Connection = Thread.readMessage(true);
		while(true){
			
		}
	}
	
	public function new(c:Connection){
		this.t = Thread.create(connector);
		this.t.sendMessage(Thread.current());
		this.t.sendMessage(c);
	}
	
	
}