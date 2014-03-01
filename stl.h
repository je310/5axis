#ifndef __STL_INCLUDED__   // if x.h hasn't been included yet...
#define __STL_INCLUDED__   //   #define this so the compiler knows it has been included
#include <irrlicht.h>
#include <vector>
#include <fstream>
#include <stdint.h>
using namespace irr;

struct trianginfo{
	core::vector3df v1;
	core::vector3df v2;
	core::vector3df v3;
	core::vector3df normal;
};

void writestl(scene::IMeshBuffer* mesh, std::string path);

core::vector3df getnormal(core::vector3df v1,core::vector3df v2,core::vector3df v3);

#endif