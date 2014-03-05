#include "rotation.h"
using namespace irr;
core::vector3df getClosestPointOnLine
( const core::vector3df& axis,const core::vector3df& pivot,const core::vector3df& point){
    core::vector3df c = point - pivot;
    f32 t = axis.dotProduct(c);
    return pivot + axis*t;
}

//both axis and pivot are in world space
void revolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot){
    node->updateAbsolutePosition();
    core::vector3df p1 = node->getAbsolutePosition();
    core::vector3df p2 = getClosestPointOnLine(axis, pivot, p1);
    core::vector3df vect = p1 - p2;
    
    core::quaternion q;
    q.fromAngleAxis(degs*core::DEGTORAD, axis);
    q.getMatrix().rotateVect(vect);
    
    node->setPosition(p2 + vect);
}

void invrevolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot){
    node->updateAbsolutePosition();
    core::vector3df p1 = node->getAbsolutePosition();
    core::vector3df p2 = getClosestPointOnLine(axis, pivot, p1);
    core::vector3df vect = p1 - p2;
	core::CMatrix4<f32> mat ;
    core::quaternion q;
    q.fromAngleAxis(degs*core::DEGTORAD, axis);
	q.getMatrix().getInverse(mat);
	mat.rotateVect(vect);
    
    node->setPosition(p2 + vect);
}

void rotateNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis)
{
    node->updateAbsolutePosition();
    core::matrix4 m2 = node->getAbsoluteTransformation();
    core::vector3df a = axis;
    m2.rotateVect(a);
    a.normalize();
 
    core::quaternion q;
    q.fromAngleAxis(degs*core::DEGTORAD, a);
    core::matrix4 m1 = q.getMatrix();
 
    core::matrix4 m = m1*m2;
    node->setRotation(m.getRotationDegrees());
}

void invrotateNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis)
{
    node->updateAbsolutePosition();
    core::matrix4 m2 = node->getAbsoluteTransformation();
    core::vector3df a = axis;
    m2.rotateVect(a);
    a.normalize();
 
    core::quaternion q;
    q.fromAngleAxis(degs*core::DEGTORAD, a);
    core::matrix4 m1 = q.getMatrix();
	m1.getInverse(m1);
    core::matrix4 m = m1*m2;
    node->setRotation(m.getRotationDegrees());
}

void moveNodeInLocalSpace(scene::ISceneNode* node, const core::vector3df& distVect)
{
    node->updateAbsolutePosition();
    core::matrix4 m = node->getAbsoluteTransformation();
    core::vector3df d = distVect;
    m.rotateVect(d);
        
    core::vector3df pos = node->getAbsolutePosition() + d;
    node->setPosition(pos);
}

void revolveNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot)
{
    moveNodeInLocalSpace(node, pivot);
    rotateNodeInLocalSpace(node, degs, axis);
    moveNodeInLocalSpace(node, -pivot);
}

void rotateNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis,const core::vector3df& pivot){
		revolveNodeInWorldSpace(node, degs, axis, pivot);
		rotateNodeInLocalSpace(node, degs,axis);
}

void mirrorx(scene::IAnimatedMeshSceneNode* obj){ // this is neccessary due to for some reason the import in irrlicht mirrors in the x axis. I must use this after all stl read ins. 
	scene::IMeshBuffer *pBuffer= obj->getMesh()->getMeshBuffer(0);
	u32 vertexcount = pBuffer->getVertexCount();
	video::S3DVertex *vertices = (video::S3DVertex *)pBuffer->getVertices();
	u32 indexcount = pBuffer->getIndexCount();
	u16 *indices =  pBuffer->getIndices();
	for(int i=0; i < vertexcount; i++){
		vertices[i].Pos.X = -vertices[i].Pos.X;
		//vertices[i].Normal = -vertices[i].Normal;
	}
	u16 temp;
	for(int i = 0 ; i <indexcount; i=i+3){
		//currenttriang.normal = getnormal(vertices[indices[i]].Pos,vertices[indices[i+1]].Pos,vertices[indices[i+2]].Pos);
		temp = indices[i];
		indices[i] = indices[i+1];
		indices[i+1] = temp;
		//mytriangles.push_back(currenttriang);
	}
	return;
}


void rotateline(core::vector3df &myline, int direction){
	core::vector3df Aaxis = core::vector3df(0,0,40);
	core::vector3df toBaxis;
	switch(direction){
	case GUI_ID_YELLOW:
		myline.rotateXZBy(-90,Aaxis);
		break;
	case GUI_ID_PINK:
		myline.rotateYZBy(90,Aaxis);
		break;
	case GUI_ID_RED:
			return;
		break;
	case GUI_ID_BLUE:
		myline.rotateXZBy(90,Aaxis);
		break;
	case GUI_ID_GREEN:
		myline.rotateYZBy(-90,Aaxis);
		break;
	}
}