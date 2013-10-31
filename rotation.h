#include <irrlicht.h>
#include "types.h"
#include <vector>

using namespace irr;



core::vector3df getClosestPointOnLine
( const core::vector3df& axis,const core::vector3df& pivot,const core::vector3df& point);

//both axis and pivot are in world space
void revolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot);
void invrevolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot);

void rotateNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis);

void invrotateNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis);

void moveNodeInLocalSpace(scene::ISceneNode* node, const core::vector3df& distVect);

void revolveNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot);