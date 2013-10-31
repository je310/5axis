
#ifndef COLLISION_INCLUDED__
#define COLLISION_INCLUDED__
#include <irrlicht.h>
#include <vector>
#include "types.h"
#include "linkedlist.h"


using namespace irr;

int intersect3D_RayTriangle( Tline R, Triangle T, core::vector3df* I );
Tline validateLine(Tline myline);
bool checkcollision(std::vector<core::vector3df> mycubes, Tline myline);
core::vector3df Headoffset(core::vector3df in,scene::ISceneNode* node);

list TriangArr[numbox][numbox][numbox];
#endif