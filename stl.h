#ifndef __STL_INCLUDED__   // if x.h hasn't been included yet...
#define __STL_INCLUDED__   //   #define this so the compiler knows it has been included
#include <irrlicht.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <stdint.h>
#include "collision.h"
#include "triangle.h"
#include "rotation.h"
#include "gcode.h"
#include "tests.h"



using namespace irr;

struct trianginfo{
	core::vector3df v1;
	core::vector3df v2;
	core::vector3df v3;
	core::vector3df normal;
};

void writestl(scene::IMeshBuffer* mesh, std::string path);

scene::IAnimatedMeshSceneNode* readstl(scene::ISceneManager* smgr,char* name);

core::vector3df getnormal(core::vector3df v1,core::vector3df v2,core::vector3df v3);

void rotatemesh(scene::IMeshBuffer* mesh, int direction, float smallz);

void makePlatform(scene::IMeshBuffer* mesh, scene::IMeshBuffer* nub);

int sliceObj(std::vector<dirnode> &allnodes, int selectednode, Triangle plane, irr::scene::ISceneManager *smgr, float smallz, int current);


#endif