#ifndef CLASTERED_SERVER_SLAVE_ATTRS_MAP_HEADER
#define CLASTERED_SERVER_SLAVE_ATTRS_MAP_HEADER

#include <vector>
#include <map>

namespace clasteredServerSlave {
	class attrs_map{
		public:
			attrs_map(): _size(0) {
			};
			template<class T> 
				void push_back(T& attr){
					attr_size.push_back(sizeof(attr));
					attr_shift.push_back(&attr);
					shift_attr[&attr]=_size;
					_size++;
				};
			void* operator()(int id){
				return attr_shift[id];
			};
			int operator()(void* attr){
				return shift_attr[attr];
			};
			unsigned short size(){
				return _size;
			};
			unsigned short size(int id){
				return attr_size[id];
			};
			unsigned short size(void* attr){
				return attr_size[operator()(attr)];
			};
		private:
			unsigned short _size;
			std::vector<unsigned short> attr_size;
			std::vector<void*> attr_shift;
			std::map<void*, int> shift_attr;	
	};
}

#endif
