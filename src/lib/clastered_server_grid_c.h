#ifndef CLASTERED_SERVER_GRID_C_HEADER
#define CLASTERED_SERVER_GRID_C_HEADER

typedef void* clastered_server_grid_t;

clastered_server_grid_t clasteredServerGridInit(float size[2], float offset);
void clasteredServerGridDestroy(clastered_server_grid_t mytype);
int clasteredServerGridGetOwner(clastered_server_grid_t self, float x, float y);
int* clasteredServerGridGetShares(clastered_server_grid_t self, float x, float y, int* size);//?
void clasteredServerGridReconfigure(clastered_server_grid_t self, int* a, short size);

#endif
