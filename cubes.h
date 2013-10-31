#ifndef CUBES_INCLUDED__
#define CUBES_INCLUDED__

#include <vector>
#include <irrlicht.h>
#include "types.h"


using namespace irr;


bool inbound(f32 start,f32 end,f32 now);
std::vector<core::vector3df> findCubesOfInterest(Tline line);

#endif