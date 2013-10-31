#ifndef TYPES_INCLUDED__
#define TYPES_INCLUDED__

#include <irrlicht.h>
//#include "linkedlist.h"

#define BuildSize 150
#define Boxsize		1
#define drawat		150
#define numbox  BuildSize/Boxsize
#define SMALL_NUM   0.00000001



using namespace irr;
struct Tline{
	core::vector3df start;
	core::vector3df end;
};

struct Triangle{
	core::vector3df V0;
	core::vector3df V1;
	core::vector3df V2;
};

struct instruction {
  int G;
  double X,Y,Z,A,B,E;
};



#endif