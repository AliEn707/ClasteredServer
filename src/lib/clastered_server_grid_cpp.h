#ifndef CLASTERED_SERVER_GRID_CPP_HEADER
#define CLASTERED_SERVER_GRID_CPP_HEADER

class grid {
    public:
        grid(float size[2], float offset);
        int getOwner(float x, float y);
        int* getShares(float x, float y);//return array of int of different size
        void reconfigure(int* a, int id);//take array of int different size
};

#endif
