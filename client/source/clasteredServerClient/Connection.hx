package clasteredServerClient;


import haxe.io.Bytes;
import sys.net.Socket;
import sys.net.Host;


class Connection{
	private var sock:Socket=new Socket();
	
	public function new(host:String, port:Int){
		sock.connect(new Host(host), port);
		sock.input.bigEndian=false;
		sock.output.bigEndian=false;
//		sock.setFastSend=true;
	}
	
	public function recvChar():Int{
		return sock.input.readInt8();
	}

	public function recvShort():Int{
		return sock.input.readInt16();
	}

	public function recvInt():Int{
		return sock.input.readInt32();
	}

	public function recvFloat():Float{
		return sock.input.readFloat();
	}

	public function recvDouble():Float{
		return sock.input.readDouble();
	}

	public function recvBytes(?size:Null<Int>):Bytes{
		if (size==null)
			size=recvShort();
		return sock.input.read(size);
	}

	public function recvString(?size:Null<Int>):String{
		if (size==null)
			size=recvShort();
		return sock.input.readString(size);
	}

	public function sendChar(a:Int):Void{
		sock.output.writeInt8(a);
	}

	public function sendShort(a:Int):Void{
		sock.output.writeInt16(a);
	}

	public function sendInt(a:Int):Void{
		sock.output.writeInt32(a);
	}

	public function sendFloat(a:Float):Void{
		sock.output.writeFloat(a);
	}

	public function sendDouble(a:Float):Void{
		sock.output.writeDouble(a);
	}

	public function sendBytes(s:Bytes):Void{
		sendShort(s.length);
		sock.output.write(s);
	}

	public function sendString(s:String):Void{
		sendShort(s.length);
		sock.output.writeString(s);
	}

}