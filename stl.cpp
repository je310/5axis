#include "stl.h"
using namespace irr;

core::vector3df getnormal(core::vector3df v1,core::vector3df v2,core::vector3df v3){
	core::vector3df v12  = v2 - v1;
	core::vector3df v13  = v3 - v1;
	return v12.crossProduct(v13).normalize();
}

uint32_t swapEndian(uint32_t in){
	uint32_t out = ((in>>24)&0xff) | // move byte 3 to byte 0
                    ((in<<8)&0xff0000) | // move byte 1 to byte 2
                    ((in>>8)&0xff00) | // move byte 2 to byte 1
                    ((in<<24)&0xff000000); // byte 0 to byte 3
	return out;
}

void writestl(scene::IMeshBuffer* mesh, std::string path){ //this now outputs the stl as per the input,
	char * memblock;
	path.append(".stl");
	std::vector<trianginfo> mytriangles;
	std::ofstream myfile(path,std::ofstream::binary);
	//myfile.open (path);
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
	//size = swapEndian(size);
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

void rotatemesh(scene::IMeshBuffer* mesh, int direction, float smallz){
	u32 indexcount = mesh->getIndexCount();
	u16 *indices =  mesh->getIndices();
	u32 vertexcount = mesh->getVertexCount();
	video::S3DVertex *vertices = (video::S3DVertex *)mesh->getVertices();
	for(int i = 0; i < vertexcount; i++){
		vertices[i].Pos = vertices[i].Pos - core::vector3df(0,0,smallz);
		rotateline(vertices[i].Pos,direction);
		vertices[i].Pos = vertices[i].Pos + core::vector3df(0,0,smallz);
	}
}

bool isInIndexVect(std::vector<int> lowestInd, int index){
	for(int i  = 0; i < lowestInd.size(); i ++){
		if(index== lowestInd.at(i)){
			return true;
		}
	}
	return false;
}

bool seenAlready(std::vector<video::S3DVertex> vect,video::S3DVertex vertex ){
	for(int i=0; i< vect.size(); i++){
		if(vect.at(i).Pos == vertex.Pos){
			return true;
		}
	}
	return false;
}

bool containedOnce(int a, int b, std::vector<int> vect){
	bool acontained = false;
	bool bcontained = false;
	int number =0;
	for( int i= 0 ; i < vect.size(); i = i+3){
		for(int j = 0; j <3; j++){
			if(a == vect.at(i+j)){
				acontained = true;
			}
			if(b == vect.at(i+j)){
				bcontained = true;
			}
			if (acontained && bcontained){
				number++;
				acontained = false;
				bcontained = false;
			} 
			if(number > 1){
				return false;
			}
		}
		acontained = false;
		bcontained = false;
	}
	if(number == 1){
		return true;
	}
	else{
		//this should never happen as the pair should always appear at least once.
	}
}

void findNextInLoop(std::vector<int> pairs, int &current, int &previous){
	int size = pairs.size();
	int a;
	int b;
	for (int i = 0; i< size; i= i + 2){
		a = pairs.at(i);
		b = pairs.at(i+1);
		if(a == current && b!=previous){
			previous = current;
			current = b;
			return;
		}
		if(b == current && a!=previous){
			previous = current;
			current = a;
			return;
		}
	}
	

}

int findNextLoop(std::vector<int> pairs,std::vector<int> seenInd){
	for(int i = 0; i < pairs.size()/2; i++){
		if(!isInIndexVect(seenInd,pairs.at(i))){
			return pairs.at(i);
		}
	}
	return -1; // this should not happen 
	
}



void makePlatform(scene::IMeshBuffer* mesh, scene::IMeshBuffer* nub){
	u32 indexcount = mesh->getIndexCount();
	u16 *indices =  mesh->getIndices();
	u32 vertexcount = mesh->getVertexCount();
	video::S3DVertex *vertices = (video::S3DVertex *)mesh->getVertices();
	int index[36];
	video::S3DVertex vertexarr[36];
	for(int i = 0; i< 36; i++){
		index[i]  = indices[i];
		vertexarr[i] = vertices[i];
	}
	//find lowest vertexes, first find the lowest value of z.
	float smallz= 100000;
	for(int i = 0; i< vertexcount;i++){
		if(vertices[i].Pos.Z < smallz){
			smallz = vertices[i].Pos.Z;
		}
	}
	//form a vector of all vertexes and their corresponding indecies that lie on the bottom.
	std::vector<video::S3DVertex> lowestVert;
	std::vector<int> lowestInd;
	for(int i = 0; i< vertexcount;i++){
		if (vertices[i].Pos.Z == smallz){
			lowestVert.push_back(vertices[i]);
			lowestInd.push_back(indices[i]);
		}
	}
	//find vertexes/index combinations that are entirly on the bottom. 
	int newVertCounter = 0;
	int newIndCounter = 0;
	std::vector<video::S3DVertex> platformVert;
	std::vector<int> platformInd;
	for(int i = 0; i<indexcount; i = i+3){
		if(isInIndexVect(lowestInd,indices[i])&&isInIndexVect(lowestInd,indices[i+1])&&isInIndexVect(lowestInd,indices[i+2])){
			for(int j = 0; j<3 ; j++){
				platformVert.push_back(vertices[indices[i+j]]);
				platformInd.push_back(i+j);
			}
		}
	}

	//remove duplicate vertexes and correct the index list
	int currentInd = 0;
	std::vector<video::S3DVertex> seenVert;
	for(int i = 0; i < platformVert.size(); i++){
		if(!seenAlready(seenVert,platformVert.at(i))){
			seenVert.push_back(platformVert.at(i)); // make a list of vertex locations seen till this point. 
			for(int j = 0; j < platformInd.size(); j++){
				if(platformVert.at(j).Pos == seenVert.at(currentInd).Pos){
					platformInd.at(j) = currentInd;
				}
			}
			currentInd++;
		}
		
	}

	//copy the face with the order of the triangles reversed to above the origionals by the bed thickness. 
	video::S3DVertex vertInUse;
	float plateThickness = 4;
	int faceVerts = seenVert.size();
	int faceInd  = platformInd.size();
	for(int i = 0; i< faceVerts; i++){
		vertInUse = seenVert.at(i);
		vertInUse.Pos.Z += plateThickness;
		seenVert.push_back(vertInUse);
		//platformInd.push_back(platformInd.at(i)+faceVerts);
	}
	for(int i = 0; i< faceInd; i= i+3){
		platformInd.push_back(platformInd.at(i)+faceVerts);
		platformInd.push_back(platformInd.at(i+2)+faceVerts);
		platformInd.push_back(platformInd.at(i+1)+faceVerts);
	}
	//find pairs of vertexes that only appear once 
	std::vector<int> pairs;
	for(int i = 0; i < platformInd.size(); i = i+3){
		if(containedOnce(platformInd.at(i),platformInd.at(i+1),platformInd)){
			pairs.push_back(platformInd.at(i));
			pairs.push_back(platformInd.at(i+1));
		}
		if(containedOnce(platformInd.at(i+1),platformInd.at(i+2),platformInd)){
			pairs.push_back(platformInd.at(i+1));
			pairs.push_back(platformInd.at(i+2));
		}
		if(containedOnce(platformInd.at(i+2),platformInd.at(i),platformInd)){
			pairs.push_back(platformInd.at(i+2));
			pairs.push_back(platformInd.at(i));
		}
	}

	//link up with new trianles 
	std::vector<int> seenInd;
	int first = pairs.at(0);
	int current  = pairs.at(0);
	int previous = -1;
	int addedCount = 0;
	bool notfinished  = true;
	findNextInLoop(pairs, current, previous);
	while(notfinished){		// account for seperate loops
		while(current != first){
			platformInd.push_back(previous);
			platformInd.push_back(previous+faceVerts);
			platformInd.push_back(current);
			platformInd.push_back(current+faceVerts);
			platformInd.push_back(current);
			platformInd.push_back(previous+faceVerts);
			addedCount= addedCount +2;
			seenInd.push_back(current);
			seenInd.push_back(previous);
			findNextInLoop(pairs,current,previous);
		}
		// finish the last one.
		platformInd.push_back(previous);
		platformInd.push_back(previous+faceVerts);
		platformInd.push_back(current);
		platformInd.push_back(current+faceVerts);
		platformInd.push_back(current);
		platformInd.push_back(previous+faceVerts);
		addedCount= addedCount +2;
		seenInd.push_back(current);
		seenInd.push_back(previous);
		findNextInLoop(pairs,current,previous);
		if(addedCount == pairs.size()/2){
			notfinished = false;
		}
		else{
			current =findNextLoop(pairs,seenInd);
			first = current;
			previous = -1;
			findNextInLoop(pairs, current, previous);
		}
	}

	//add the nub
	u32 indexcountNub = nub->getIndexCount();
	u16 *indicesNub =  nub->getIndices();
	u32 vertexcountNub = nub->getVertexCount();
	video::S3DVertex *verticesNub = (video::S3DVertex *)nub->getVertices();
	for(int i = 0; i <indexcountNub; i++){
		platformInd.push_back(indicesNub[i]+ seenVert.size());
	}
	video::S3DVertex verticesTemp;
	for(int i = 0; i <vertexcountNub; i++){
		verticesTemp = verticesNub[i];
		verticesTemp.Pos.Z += smallz+plateThickness;
		seenVert.push_back(verticesTemp);
	}



	// write STl
	std::vector<trianginfo> mytriangles;
	std::ofstream myfile("platform.stl",std::ofstream::binary);
	trianginfo currenttriang;
	char * memblock;
	for(int i = 0 ; i <platformInd.size(); i=i+3){
		currenttriang.normal = getnormal(seenVert.at(platformInd.at(i)).Pos,seenVert.at(platformInd.at(i+1)).Pos,seenVert.at(platformInd.at(i+2)).Pos);
		currenttriang.v1 = seenVert.at(platformInd.at(i)).Pos;
		currenttriang.v2 = seenVert.at(platformInd.at(i+1)).Pos;
		currenttriang.v3 = seenVert.at(platformInd.at(i+2)).Pos;
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



//dirnode sliceObj(std::vector<dirnode> allnodes, int selectednode, Triangle plane){
//
//}



// - core::vector3df(0,0,smallz)