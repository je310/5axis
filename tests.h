#ifndef __TESTS_INCLUDED__   // if x.h hasn't been included yet...
#define __TESTS_INCLUDED__   //   #define this so the compiler knows it has been included

#include <vector>
#include <irrlicht.h>
#include "list.h"
#include "triangle.h"
#include "collision.h"

using namespace irr;

bool testTriangleCol();

void addlighting(scene::ISceneManager* smgr, float lightstr = 70);

std::vector<Tline> makegrid();


#endif