#include "stl.h"
using namespace irr;

core::vector3df getnormal(core::vector3df v1,core::vector3df v2,core::vector3df v3){
	core::vector3df v12  = v2 - v1;
	core::vector3df v13  = v3 - v1;
	return v12.crossProduct(v13).normalize();
}


void writestl(scene::IMeshBuffer* mesh, std::string path){ //this now outputs the stl as per the input,
	char * memblock;
	path.append(".stl");
	std::vector<trianginfo> mytriangles;
	std::ofstream myfile;
	myfile.open (path);
	u32 indexcount = mesh->getIndexCount();
	u16 *indices =  mesh->getIndices();
	u32 vertexcount = mesh->getVertexCount();
	video::S3DVertex *vertices = (video::S3DVertex *)mesh->getVertices();
	trianginfo currenttriang;
	for(int i = 0 ; i <indexcount; i=i+3){
		currenttriang.normal = getnormal(vertices[indices[i]].Pos,vertices[indices[i+1]].Pos,vertices[indices[i+2]].Pos);
		currenttriang.v1 = vertices[indices[i]].Pos;
		currenttriang.v2 = vertices[indices[i+1]].Pos;
		currenttriang.v3 = vertices[indices[i+2]].Pos;
		mytriangles.push_back(currenttriang);
	}
	int numberofchars = 80 + 4 + mytriangles.size()*((4*3*4)+2);
	memblock = new char [numberofchars];
	for(int i = 0; i<80; i++){
		memblock[i] = (u8)0x00;
	}
	for(int i = 80; i<numberofchars; i++){
		memblock[i] = (u8)0;
	}
	memblock[0] = 's';
	memblock[1] = 't';
	memblock[2] = 'a';
	memblock[3] = 'r';
	memblock[4] = 0xcdcdef01;
	uint32_t size = mytriangles.size();
	//size = Reverseuint32(size);
	memcpy(memblock + 80,&size,4);
	float switchfloat;
	for(int i = 0; i<  mytriangles.size(); i++){
		//the normal
		switchfloat =(float)mytriangles.at(i).normal.X;
		memcpy(memblock+50*i+84,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).normal.Y;
		memcpy(memblock+50*i+88,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).normal.Z;
		memcpy(memblock+50*i+92,&switchfloat,4);

		switchfloat =(float)mytriangles.at(i).v1.X;
		memcpy(memblock+50*i+96,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v1.Y;
		memcpy(memblock+50*i+100,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v1.Z;
		memcpy(memblock+50*i+104,&switchfloat,4);

		switchfloat =(float)mytriangles.at(i).v2.X;
		memcpy(memblock+50*i+108,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v2.Y;
		memcpy(memblock+50*i+112,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v2.Z;
		memcpy(memblock+50*i+116,&switchfloat,4);

		switchfloat =(float)mytriangles.at(i).v3.X;
		memcpy(memblock+50*i+120,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v3.Y;
		memcpy(memblock+50*i+124,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v3.Z;
		memcpy(memblock+50*i+128,&switchfloat,4);

	}
	myfile.write(memblock,numberofchars);
}

scene::IAnimatedMeshSceneNode* readstl(scene::ISceneManager* smgr,char* name){
	char path[30] = "stl/";
	strcat(path,name);
	strcat(path,".stl");
	return smgr->addAnimatedMeshSceneNode(smgr->getMesh(path),0, 0);
}

void rotatemesh(scene::IMeshBuffer* mesh, int direction){
	u32 indexcount = mesh->getIndexCount();
	u16 *indices =  mesh->getIndices();
	u32 vertexcount = mesh->getVertexCount();
	video::S3DVertex *vertices = (video::S3DVertex *)mesh->getVertices();
	for(int i = 0; i < vertexcount; i++){
		rotateline(vertices[0].Pos,direction);
}
}