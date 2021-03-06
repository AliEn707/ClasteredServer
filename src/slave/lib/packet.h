#ifndef CLASTERED_SERVER_PACKET_HEADER
#define CLASTERED_SERVER_PACKET_HEADER

#include <vector>
#include <string>

extern "C"{
	
}

namespace clasteredServer {
	
	class packet_chank{
		public:
			char type;
			union{
				char c;
				short s;
				int i;
				float f;
				double d;
			}value;
			std::string str;
		
			int size();
			void* data();
	};
	
	class packet {
		public:
			std::vector<packet_chank> chanks;
			struct{
				char type;
				int id;
			} dest;

			packet();
//			~packet();
			void setType(char);
			char type();
			int size();
			void* data();
			bool init();
			bool init(void*, short);
			bool add(char);
			bool add(short);
			bool add(int);
			bool add(float);
			bool add(double);
			bool add(std::string);
			bool add(void*, short size);
		private: 
			std::vector<char> buf;
			
			void parse();
	};
}


#endif
