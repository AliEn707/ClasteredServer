
#include "clastered_server_grid_cpp.h"

//cpp class and code
class grid{
	
}

//c bindings
#ifdef __cplusplus
extern "C" {
#endif
#include "clastered_server_grid_c.h"
#ifdef __cplusplus
}
#endif

clastered_server_grid_t clasteredServerGridInit(float size[2], float offset) {
	return new grid(size, offset);
}

void clasteredServerGridDestroy(clastered_server_grid_t untyped_ptr) {
	grid* typed_ptr = static_cast<grid*>(untyped_ptr);
	delete typed_ptr;
}

int clasteredServerGridGetOwner(clastered_server_grid_t untyped_self, float x, float y) {
	grid* typed_self = static_cast<grid*>(untyped_self);
	return typed_self->getOwner(x, y);
}

int* clasteredServerGridGetShares(clastered_server_grid_t untyped_self, float x, float y,int* size) {
	grid* typed_self = static_cast<grid*>(untyped_self);
	return typed_self->getShares(x, y);//need to correct
}

void clasteredServerGridReconfigure(clastered_server_grid_t untyped_self, int* a, short size, int id) {
	grid* typed_self = static_cast<grid*>(untyped_self);
	return typed_self->reconfigure(a, size, id);//??
}

