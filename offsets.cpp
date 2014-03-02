#include "offsets.h"

core::vector3df Headoffset(core::vector3df in,scene::ISceneNode* node){
	core::vector3df rotation = node->getAbsoluteTransformation().getRotationDegrees();
	core::vector3df out; 
	out.X = in.X +5;
	out.Y = in.Y -5*cos(core::DEGTORAD*rotation.X);
	out.Z = in.Z - 5*sin(core::DEGTORAD*rotation.X);
	return out;

}



core::vector3df findworldcoord(core::vector3df in, float Adeg, float Bdeg, float zsmall){
	core::vector3df out;
	in.Z = in.Z  + zsmall;
	core::vector2df vectopiv = core::vector2df(0 - in.X, 50 - in.Z);
	vectopiv.rotateBy(Adeg,core::vector2df(0, 50));
	//todo add in the b rotation 
	out = core::vector3df(-vectopiv.X, in.Y, 50-vectopiv.Y); // in this case I am projecting into 2d so y = z! 
	return out;
}

core::vector3df findmodeloffset(scene::IAnimatedMeshSceneNode* model, float Adeg, float Bdeg, float smallz){ // this function finds the mid point of the model on the bottom face, this is where slic3r puts the middle of the model, use this to correct for slic3r moving my peices around.
	scene::IMeshBuffer *pBuffer;
    pBuffer = model->getMesh()->getMeshBuffer(0);
	float zoffset = 10000;
	float xmin = 10000; 
	float xmax= -10000; 
	float ymin = 10000; 
	float ymax = -10000; 
	int VertexCount = pBuffer->getVertexCount();
	video::S3DVertex *vertices = (video::S3DVertex *)pBuffer->getVertices();
	for(int i = 0; i< VertexCount; i++){
		if(findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).Z < zoffset){
			zoffset = findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).Z;
		}
		if(findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).X < xmin){
			xmin = findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).X;
		}
		if(findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).X > xmax){
			xmax = findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).X;
		}
		if(findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).Y < ymin){
			ymin = findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).Y;
		}
		if(findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).Y > ymax){
			ymax = findworldcoord(vertices[i].Pos,Adeg,Bdeg,smallz).Y;
		}
	}
	core::vector3df out;
	out.Z = zoffset;
	out.X = (xmin + xmax)/2;
	out.Y = (ymin + ymax)/2;
	return out;
}


float findlowestvert (scene::IAnimatedMeshSceneNode* thing){
	float smallz = 1000;
	scene::IMeshBuffer *pBuffer= thing->getMesh()->getMeshBuffer(0);;
	std::vector<video::S3DVertex> vertvect;
	int VertexCount = pBuffer->getVertexCount();
	video::S3DVertex *vertices = (video::S3DVertex *)pBuffer->getVertices();
	video::S3DVertex *trans	= new video::S3DVertex();
	for(int i = 0; i< VertexCount; i++){
		vertvect.push_back(vertices[i]);
		if(vertices[i].Pos.Z < smallz){
			smallz = vertices[i].Pos.Z;
		}
	}
	return smallz;
}