package;
import haxe.CallStack;

import clasteredServerClient.Connection;
import clasteredServerClient.Packet;
import flixel.FlxGame;
import openfl.display.Sprite;

import flixel.FlxG;
import flixel.system.scaleModes.*;


class Main extends Sprite
{
	public function new()
	{
		super();
		addChild(new FlxGame(640, 480, PlayState));
		
		FlxG.fixedTimestep = false;
		FlxG.scaleMode = new BorderedStageSizeScaleMode(1024,768);
		//read config and setup
	#if desktop
		FlxG.resizeGame(800, 600);
		FlxG.resizeWindow(800, 600);
//		FlxG.camera.setSize(800, 600);
	#end
		trace("sfdsfsds");
		//FlxG.fullscreen = true;
		try {
			var c:Connection = new Connection("172.16.1.40", 8000);
			trace("sfdsfsds");
			trace(c.auth("qwer","qwer"));
    	} catch( msg : String ) {
			trace("Произошла ошибка: " + msg);
			trace(CallStack.toString(CallStack.exceptionStack()));
		}
	}
}