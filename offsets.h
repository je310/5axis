#ifndef __OFF_INCLUDED__   // if x.h hasn't been included yet...
#define __OFF_INCLUDED__   //   #define this so the compiler knows it has been included
#include <irrlicht.h>
#include <vector>
#include "triangle.h"
#include "colours.h"
using namespace irr;


core::vector3df Headoffset(core::vector3df in,scene::ISceneNode* node);

core::vector3df findworldcoord(core::vector3df in, float Adeg, float Bdeg, float zsmall);

core::vector3df findmodeloffset(scene::IAnimatedMeshSceneNode* model, float Adeg, float Bdeg, float smallz);

float findlowestvert (scene::IAnimatedMeshSceneNode* thing);

void addoffset(Triangle &triangle, float smallz);

void offsetPlane(Triangle &plane, int current, float offset);

irr::core::vector3df findClosestPoint(irr::core::vector3df point,std::vector<dirnode> allnodes,int selectednode,float smallz);

#endif