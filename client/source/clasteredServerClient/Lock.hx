package clasteredServerClient;

#if cpp
import cpp.vm.Mutex;
#elseif neko
import neko.vm.Mutex;
#elseif java
import java.vm.Mutex;
#end


/**
 * ...
 * @author ...
 */
class Lock{
	#if use_locks
	private var m:Mutex=new Mutex();
	#end
	
	public function new(){
		
	}
	
	public function lock():Void{
		#if use_locks
			m.acquire();
		#end
	}
	
	public function unlock():Void{
		#if use_locks
			m.release();
		#end
	}
	
}