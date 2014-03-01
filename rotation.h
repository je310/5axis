#ifndef __ROT_H_INCLUDED__   // if x.h hasn't been included yet...
#define __ROT_H_INCLUDED__   //   #define this so the compiler knows it has been included
#include <irrlicht.h>
using namespace irr;

core::vector3df getClosestPointOnLine( const core::vector3df& axis,const core::vector3df& pivot,const core::vector3df& point);

void revolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot);

void invrevolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot);

void rotateNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis);

void invrotateNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis);

void moveNodeInLocalSpace(scene::ISceneNode* node, const core::vector3df& distVect);

void revolveNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot);

void rotateNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis,const core::vector3df& pivot);
#endif