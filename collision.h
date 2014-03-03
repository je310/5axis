#ifndef __COL_INCLUDED__   // if x.h hasn't been included yet...
#define __COL_INCLUDED__   //   #define this so the compiler knows it has been included

#include <vector>
#include <irrlicht.h>
#include "list.h"
#include "triangle.h"
#include "stl.h"
#include "offsets.h"
using namespace irr;

#define BuildSize 150
#define Boxsize		1
#define drawat		1
#define numbox  BuildSize/Boxsize

 extern list TriangArr[numbox][numbox][numbox];

 

struct Tline{
	core::vector3df start;
	core::vector3df end;
};



bool inbound(f32 start,f32 end,f32 now);

std::vector<core::vector3df> findCubesOfInterest(Tline line);

int intersect3D_RayTriangle( Tline R, Triangle T, core::vector3df* I );

Tline validateLine(Tline myline);

bool checkcollision(std::vector<core::vector3df> mycubes, Tline myline);

scene::IAnimatedMeshSceneNode* findselected(Tline &ray, std::vector<scene::IAnimatedMeshSceneNode*> allnodes, float smallz);


#endif