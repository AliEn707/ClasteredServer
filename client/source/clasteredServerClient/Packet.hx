package clasteredServerClient;


class Chank{
	public var type:Int;
	public var i:Int;
	public var f:Float;
	public var s:String;
}

class Packet{
	
	public var chanks:Array<Chank>;
	
	public function new(){
		Sys.println("Started");
		trace("started");
	}
	
}