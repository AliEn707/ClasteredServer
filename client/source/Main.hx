package;

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
		FlxG.scaleMode = new StageSizeScaleMode();
		//read config and setup
	#if desktop
		FlxG.resizeWindow(800, 600);
		FlxG.resizeGame(800, 600);
	#end
	//FlxG.fullscreen = true;
	}
}