#ifndef __TRI_INCLUDED__   // if x.h hasn't been included yet...
#define __TRI_INCLUDED__   //   #define this so the compiler knows it has been included
#include <irrlicht.h>

using namespace irr;

struct Triangle{
	core::vector3df V0;
	core::vector3df V1;
	core::vector3df V2;
};

struct dirnode{
	scene::IAnimatedMeshSceneNode* node;
	int direction;
};

struct Tline{
	core::vector3df start;
	core::vector3df end;
};


#endif