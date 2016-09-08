package clasteredServerClient;

import haxe.CallStack;

import flixel.FlxG;
import flixel.FlxCamera;

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
#if flash
	
#else
	private var t:Thread;

	public function new(){
		this.t = Thread.create(connector);
		this.t.sendMessage(Thread.current());
	}
	
	private function connector(){
		var main:Thread = Thread.readMessage(true);
		var game:CSGame = cast FlxG.game;
		trace("connector started");
		if (game.connection != null){
			try{
				while (game.connector){
					var p:Packet = game.connection.recvPacket();
//					trace(p);
					game.l.lock();
						game.packets.push(p);
					game.l.unlock();
//					trace("loop");
				}
			}catch(e:Dynamic){
				trace(e);
				trace(CallStack.toString(CallStack.exceptionStack()));
			}
		}
		game.connection_lost();
		trace("connector exited");
	}
	
#end
	
}