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

bool seenAlreadyPos(std::vector<core::vector3df> vect,core::vector3df vertex ){
	for(int i=0; i< vect.size(); i++){
		if(abs((vect.at(i) - vertex).getLength())<0.0001){
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
//return 1 for one side 0 for the other, 2 for in-plane. 
int checkSide(core::vector3df point, core::vector3df normal, Triangle plane){
	core::vector3df pointToPlane = plane.V0 - point;
	float dot = normal.dotProduct(pointToPlane);
	if(dot > 0){
		return 1;
	}
	if(dot < 0){
		return 0;
	}
	if(dot == 0){
		return 2;
	}
}


void STLWriteFromVect(std::vector<int> rightInd, std::vector<core::vector3df> checkedRight , std::string name){
	char nameArr[150];
	strcpy(nameArr,name.c_str());
	strcat(nameArr,".stl");
	std::vector<trianginfo> mytriangles;
	std::ofstream myfile(nameArr,std::ofstream::binary);
	trianginfo currenttriang;
	char * memblock;
	for(int i = 0 ; i <rightInd.size(); i=i+3){
		currenttriang.normal = getnormal(checkedRight.at(rightInd.at(i)),checkedRight.at(rightInd.at(i+1)),checkedRight.at(rightInd.at(i+2)));
		currenttriang.v1 = checkedRight.at(rightInd.at(i));
		currenttriang.v3 = checkedRight.at(rightInd.at(i+2));
		currenttriang.v2 = checkedRight.at(rightInd.at(i+1));
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

		switchfloat =-(float)mytriangles.at(i).v1.X;
		memcpy(memblock+50*i+96,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v1.Y;
		memcpy(memblock+50*i+100,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v1.Z;
		memcpy(memblock+50*i+104,&switchfloat,4);

		switchfloat =-(float)mytriangles.at(i).v2.X;
		memcpy(memblock+50*i+108,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v2.Y;
		memcpy(memblock+50*i+112,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v2.Z;
		memcpy(memblock+50*i+116,&switchfloat,4);

		switchfloat =-(float)mytriangles.at(i).v3.X;
		memcpy(memblock+50*i+120,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v3.Y;
		memcpy(memblock+50*i+124,&switchfloat,4);
		switchfloat =(float)mytriangles.at(i).v3.Z;
		memcpy(memblock+50*i+128,&switchfloat,4);

	}
	myfile.write(memblock,numberofchars);
}

int findIndex(std::vector<int> pairs,int index){
	for(int i  = 0; i <pairs.size();i= i +2){
		if(pairs.at(i) == index){
			return i/2;
		}
	}
}

void writePolyFile(std::vector<core::vector2df> projected, std::vector<int> pairs){
	std::ofstream myfile("test.poly",std::ofstream::binary);
	myfile<< pairs.size()/2<< " 2 0 0"<< std::endl;
	for(int i  = 0; i <pairs.size();i= i +2){
		myfile << i/2 << " " << projected.at(pairs.at(i)).X << " " << projected.at(pairs.at(i)).Y<< std::endl;
	}
	myfile << pairs.size()/2 << " 0"<< std::endl;
	for(int i = 0; i < pairs.size(); i= i +2){
		myfile << i/2 << " " <<  findIndex(pairs,pairs.at(i))<<" "<< findIndex( pairs,pairs.at(i+1)) << std::endl;
	}
	myfile <<"0";
}

void appendTriangles(std::vector<int> &rightInd,std::vector<int> pairs,int way){
	std::ifstream myfile("test.1.ele",std::ofstream::binary);
	std::string line;
	std::string unit;
	std::vector<int> read;
	char buffer[20];
	getline(myfile, line);
	std::istringstream record(line);
	record >> unit;
	unit.copy(buffer,5,0);
	int numTriangles = atoi(buffer);
	for(int i = 0; i < numTriangles ; i++){
		clear(buffer);
		getline(myfile, line);
		std::istringstream record(line);
		record >> unit; //skip triangle index
		record >> unit; // first vertex index
		unit.copy(buffer,5,0);
		read.push_back(atoi(buffer));
		clear(buffer);
		record >> unit; //second vertex index
		unit.copy(buffer,5,0);
		read.push_back(atoi(buffer));
		clear(buffer);
		record >> unit; // third vertex index
		unit.copy(buffer,5,0);
		read.push_back(atoi(buffer));
	}
	if(way == 1){
		for(int i = 0; i < read.size(); i= i+3){
			rightInd.push_back(pairs.at(2*read.at(i)));
			rightInd.push_back(pairs.at(2*read.at(i+2)));
			rightInd.push_back(pairs.at(2*read.at(i+1)));
		}
	}
	else{
		for(int i = 0; i < read.size(); i= i+3){
			rightInd.push_back(pairs.at(2*read.at(i)));
			rightInd.push_back(pairs.at(2*read.at(i+1)));
			rightInd.push_back(pairs.at(2*read.at(i+2)));
		}
	}
}

void callTriangle(){
	char callTriangle[50]  = "triangle test.poly";
	system (callTriangle);
}

std::vector<int> returnPairs(std::vector<int> rightInd){
	std::vector<int> pairs;
	for(int i = 0; i < rightInd.size(); i = i+3){
		if(containedOnce(rightInd.at(i),rightInd.at(i+1),rightInd)){
			pairs.push_back(rightInd.at(i));
			pairs.push_back(rightInd.at(i+1));
		}
		if(containedOnce(rightInd.at(i+1),rightInd.at(i+2),rightInd)){
			pairs.push_back(rightInd.at(i+1));
			pairs.push_back(rightInd.at(i+2));
		}
		if(containedOnce(rightInd.at(i+2),rightInd.at(i),rightInd)){
			pairs.push_back(rightInd.at(i+2));
			pairs.push_back(rightInd.at(i));
		}
	}
	return pairs;
}
std::vector<core::vector2df> returnProjected(std::vector<core::vector3df> checkedRight, core::vector3df normal){
	std::vector<core::vector2df> projected;
	if(abs(core::vector3df(1,0,0).dotProduct(normal.normalize()))>0.25){
		//project on x
		for(int j = 0; j< checkedRight.size(); j++){
			core::vector2df thisone;
			thisone.X = checkedRight.at(j).Y;
			thisone.Y = checkedRight.at(j).Z;
			projected.push_back(thisone);
		}
	}
	else if(abs(core::vector3df(0,1,0).dotProduct(normal.normalize()))>0.25){
		//project on x
		for(int j = 0; j< checkedRight.size(); j++){
			core::vector2df thisone;
			thisone.X = checkedRight.at(j).X;
			thisone.Y = checkedRight.at(j).Z;
			projected.push_back(thisone);
		}
	}
	else if(abs(core::vector3df(0,0,1).dotProduct(normal.normalize()))>0.25){
		//project on x
		for(int j = 0; j< checkedRight.size(); j++){
			core::vector2df thisone;
			thisone.X = checkedRight.at(j).X;
			thisone.Y = checkedRight.at(j).Y;
			projected.push_back(thisone);
		}
	}
	return projected;
}

bool findCorrectDirection(int current){
	switch(current){
	case GUI_ID_YELLOW:
		return false;
		break;
	case GUI_ID_PINK:
		return true;
		break;
	case GUI_ID_RED:
		return true;
		break;
	case GUI_ID_BLUE:
		return true;
		break;
	case GUI_ID_GREEN:
		return false;
		break;
	}

}

void flipTriangles(std::vector<int> &index){
	int temp = 0;
	for(int i= 0; i < index.size(); i = i+3){
		temp = index.at(i);
		index.at(i) = index.at(i+1);
		index.at(i+1) = temp;
	}
}

int sliceObj(std::vector<dirnode> &allnodes, int selectednode, Triangle plane,irr::scene::ISceneManager* smgr,float smallz, int current){
	core::vector3df normal = getnormal(plane.V0,plane.V1,plane.V2);
	scene::IMeshBuffer* wholeMesh = allnodes.at(selectednode).node->getMesh()->getMeshBuffer(0);
	u32 indexcountWhole = wholeMesh->getIndexCount();
	u16 *indicesWhole = wholeMesh->getIndices();
	u32 vertexcountWhole = wholeMesh->getVertexCount();
	video::S3DVertex *verticesWhole = (video::S3DVertex *)wholeMesh->getVertices();
	core::vector3df Intersection; 
	core::vector3df IntA; 
	core::vector3df IntB; 
	Tline ray;
	int error = 0;
	std::vector<core::vector3df> left; 
	std::vector<core::vector3df> right; 
	for(int i = 0; i < indexcountWhole; i = i+3){
		if(checkSide(verticesWhole[indicesWhole[i]].Pos, normal,plane) ==1 && checkSide(verticesWhole[indicesWhole[i+1]].Pos, normal,plane) ==1 && checkSide(verticesWhole[indicesWhole[i+2]].Pos, normal,plane) ==1){
			left.push_back(verticesWhole[indicesWhole[i]].Pos);
			left.push_back(verticesWhole[indicesWhole[i+1]].Pos);
			left.push_back(verticesWhole[indicesWhole[i+2]].Pos);
		}
		else if(checkSide(verticesWhole[indicesWhole[i]].Pos, normal,plane) ==0 && checkSide(verticesWhole[indicesWhole[i+1]].Pos, normal,plane) ==0 && checkSide(verticesWhole[indicesWhole[i+2]].Pos, normal,plane) ==0){
			right.push_back(verticesWhole[indicesWhole[i]].Pos);
			right.push_back(verticesWhole[indicesWhole[i+1]].Pos);
			right.push_back(verticesWhole[indicesWhole[i+2]].Pos);
		}
		//the particularly annoying edge cases where there are vertices on the cut plane. 
		else if(checkSide(verticesWhole[indicesWhole[i]].Pos, normal,plane) ==2 || checkSide(verticesWhole[indicesWhole[i+1]].Pos, normal,plane) ==2 || checkSide(verticesWhole[indicesWhole[i+2]].Pos, normal,plane) ==2){
			std::vector<int> onBoundry;
			for(int j = 0; j <3; j++){
				if( checkSide(verticesWhole[indicesWhole[i+j]].Pos, normal,plane)==2){
					onBoundry.push_back(j);
				}
			}
			// when there is only one vertex on the cutting plane we want to give a smaller triangle to both sides. 
			if(onBoundry.size() == 1){
				std::vector<int> onLeft;
				std::vector<int> onRight;
				for(int j = 0; j <3; j++){
					if( checkSide(verticesWhole[indicesWhole[i+j]].Pos, normal,plane)==1){
						onLeft.push_back(j);
					}
					if( checkSide(verticesWhole[indicesWhole[i+j]].Pos, normal,plane)==0){
						onRight.push_back(j);
					}
				}
				if(onLeft.size() == 2 || onRight.size() ==2){
					if(onLeft.size() == 2){
						left.push_back(verticesWhole[indicesWhole[i]].Pos);
						left.push_back(verticesWhole[indicesWhole[i+1]].Pos);
						left.push_back(verticesWhole[indicesWhole[i+2]].Pos);
					}
					if(onRight.size() == 2){
						right.push_back(verticesWhole[indicesWhole[i]].Pos);
						right.push_back(verticesWhole[indicesWhole[i+1]].Pos);
						right.push_back(verticesWhole[indicesWhole[i+2]].Pos);
					}
				}
				else{
					ray.start = verticesWhole[indicesWhole[i+(onBoundry.at(0)+1)%3]].Pos;
					ray.end = verticesWhole[indicesWhole[i+(onBoundry.at(0)+2)%3]].Pos;
					Intersection = core::vector3df(99999,99999,99999);
					intersect3D_RayTriangle(ray,plane,& Intersection);
					IntA = Intersection; // the new vertex that is needed.
					if(Intersection.equals(core::vector3df(99999,99999,99999))){
						//something went wrong. 
						error++;
					}
					if(checkSide(verticesWhole[indicesWhole[i+(onBoundry.at(0)+1)%3]].Pos,normal,plane)==0){
						right.push_back(verticesWhole[indicesWhole[i+(onBoundry.at(0))]].Pos);
						right.push_back(verticesWhole[indicesWhole[i+(onBoundry.at(0)+1)%3]].Pos);
						right.push_back(IntA);

						left.push_back(verticesWhole[indicesWhole[i+(onBoundry.at(0))]].Pos);
						left.push_back(IntA);
						left.push_back(verticesWhole[indicesWhole[i+(onBoundry.at(0)+2)%3]].Pos);
					}
					else{
						left.push_back(verticesWhole[indicesWhole[i+(onBoundry.at(0))]].Pos);
						left.push_back(verticesWhole[indicesWhole[i+(onBoundry.at(0)+1)%3]].Pos);
						left.push_back(IntA);

						right.push_back(verticesWhole[indicesWhole[i+(onBoundry.at(0))]].Pos);
						right.push_back(IntA);
						right.push_back(verticesWhole[indicesWhole[i+(onBoundry.at(0)+2)%3]].Pos);
					}
				}
			}
			//when there is two vertexes we need there to be the triangle preserved as is on the side with the valid vertex, none on the other side. 
			if(onBoundry.size() == 2){
				int validPoint = 0;
				for(int j =0; j <3; j++){
					if(!isInIndexVect(onBoundry,j)){
						validPoint = j;
					}
				}
				if(checkSide(verticesWhole[indicesWhole[i+validPoint]].Pos, normal,plane)==1){
					left.push_back(verticesWhole[indicesWhole[i]].Pos);
					left.push_back(verticesWhole[indicesWhole[i+1]].Pos);
					left.push_back(verticesWhole[indicesWhole[i+2]].Pos);
				}
				else{
					right.push_back(verticesWhole[indicesWhole[i]].Pos);
					right.push_back(verticesWhole[indicesWhole[i+1]].Pos);
					right.push_back(verticesWhole[indicesWhole[i+2]].Pos);
				}

			}
			//if they are on the cutting plane they will be fixed by Triangle later. 
			if(onBoundry.size() == 3){
				/*left.push_back(verticesWhole[indicesWhole[i]].Pos);
				left.push_back(verticesWhole[indicesWhole[i+1]].Pos);
				left.push_back(verticesWhole[indicesWhole[i+2]].Pos);
				right.push_back(verticesWhole[indicesWhole[i]].Pos);
				right.push_back(verticesWhole[indicesWhole[i+1]].Pos);
				right.push_back(verticesWhole[indicesWhole[i+2]].Pos);*/
			}
		}
		else{
			//deal with the split case, we will end up with a quad and a smaller triangle. 
			//intersect3D_RayTriangle(ray,plane,& Intersection);
			int numberOnLeft = 0;
			std::vector<int> onLeft;
			std::vector<int> onRight;
			for(int j = 0; j <3; j++){
				if( checkSide(verticesWhole[indicesWhole[i+j]].Pos, normal,plane)==1){
					onLeft.push_back(j);
				}
				else{
					onRight.push_back(j);
				}
			}
			if(onLeft.size()==1){// triangle on left, quad on right.
				left.push_back(verticesWhole[indicesWhole[i+onLeft.at(0)]].Pos);
				ray.start = verticesWhole[indicesWhole[i+onLeft.at(0)]].Pos;
				ray.end = verticesWhole[indicesWhole[i+(onLeft.at(0)+1)%3]].Pos;
				Intersection = core::vector3df(99999,99999,99999);
				intersect3D_RayTriangle(ray,plane,& Intersection);
				IntA = Intersection;
				if(Intersection.equals(core::vector3df(99999,99999,99999))){
					//something went wrong. 
					error++;
				}
				else{
					left.push_back(Intersection);
				}
				ray.end = verticesWhole[indicesWhole[i+(onLeft.at(0)+2)%3]].Pos;
				Intersection = core::vector3df(99999,99999,99999);
				intersect3D_RayTriangle(ray,plane,& Intersection);
				IntB = Intersection;
				if(Intersection.equals(core::vector3df(99999,99999,99999))){
					//something went wrong. 
					error++;
				}
				else{
					left.push_back(Intersection);
				}
				// done small triangle. 
				right.push_back(verticesWhole[i+(onLeft.at(0)+1)%3].Pos);
				right.push_back(verticesWhole[i+(onLeft.at(0)+2)%3].Pos);
				right.push_back(IntA);
				right.push_back(verticesWhole[i+(onLeft.at(0)+2)%3].Pos);
				right.push_back(IntB);
				right.push_back(IntA);
				// done quad as two triangles. 
			}
			else{// quad on left, triangle on right. 
				right.push_back(verticesWhole[indicesWhole[i+onRight.at(0)]].Pos);
				ray.start = verticesWhole[indicesWhole[i+onRight.at(0)]].Pos;
				ray.end = verticesWhole[indicesWhole[i+(onRight.at(0)+1)%3]].Pos;
				Intersection = core::vector3df(99999,99999,99999);
				intersect3D_RayTriangle(ray,plane,& Intersection);
				IntA = Intersection;
				if(Intersection.equals(core::vector3df(99999,99999,99999))){
					//something went wrong. 
					error++;
				}
				else{
					right.push_back(Intersection);
				}
				ray.end = verticesWhole[indicesWhole[i+(onRight.at(0)+2)%3]].Pos;
				Intersection = core::vector3df(99999,99999,99999);
				intersect3D_RayTriangle(ray,plane,& Intersection);
				IntB = Intersection;
				if(Intersection.equals(core::vector3df(99999,99999,99999))){
					//something went wrong. 
					error++;
				}
				else{
					right.push_back(Intersection);
				}
				// done small triangle. 
				left.push_back(verticesWhole[i+(onRight.at(0)+1)%3].Pos);
				left.push_back(verticesWhole[i+(onRight.at(0)+2)%3].Pos);
				left.push_back(IntA);
				left.push_back(verticesWhole[i+(onRight.at(0)+2)%3].Pos);
				left.push_back(IntB);
				left.push_back(IntA);
				// done quad as two triangles. 
			}
		}
	}
	if(right.size() == 0 || left.size() == 0 ){
		return 2 ; // the split is outsid the object. 
	}
	// left and right should contain a separated shell of the origional now. We should now remove redundancy in the indices, find pairs that appear oonly once (ragged edge) on one half, triangulate this, and add to both halfs.
	std::vector<int> rightInd;	
	std::vector<int> leftInd;	
	for(int i = 0; i < left.size(); i++){
		leftInd.push_back(i);
	}
	for(int i = 0; i < right.size(); i++){
		rightInd.push_back(i);
	}
	STLWriteFromVect(rightInd, right, "partialtestright");
	STLWriteFromVect(leftInd, left, "partialtestleft");
	
	//remove duplicate vertexes and correct the index list
	int currentInd = 0;
	std::vector<core::vector3df> checkedRight;
	for(int i = 0; i < right.size(); i++){
		if(!seenAlreadyPos(checkedRight,right.at(i))){
			checkedRight.push_back(right.at(i)); // make a list of vertex locations seen till this point. 
			for(int j = 0; j < rightInd.size(); j++){
				if(abs((right.at(j)- checkedRight.at(currentInd)).getLength())<0.0001){
					rightInd.at(j) = currentInd;
				}
			}
			currentInd++;
		}

	}
	currentInd = 0;
	std::vector<core::vector3df> checkedLeft;
	for(int i = 0; i < left.size(); i++){
		if(!seenAlreadyPos(checkedLeft,left.at(i))){
			checkedLeft.push_back(left.at(i)); // make a list of vertex locations seen till this point. 
			for(int j = 0; j < leftInd.size(); j++){
				if(abs((left.at(j)- checkedLeft.at(currentInd)).getLength())<0.0001){
					leftInd.at(j) = currentInd;
				}
			}
			currentInd++;
		}

	}

	//find pairs of vertexes that only appear once 
	std::vector<int> pairsRight= returnPairs(rightInd);
	std::vector<int> pairsLeft= returnPairs(leftInd);

	bool triangleDirection = findCorrectDirection(current);


	// now we need to write to a file using this loop information. first find an appropriate axis aligned directoin to project the loop points onto. 
	std::vector<core::vector2df> projectedRight = returnProjected(checkedRight,normal);
	writePolyFile(projectedRight, pairsRight);
	callTriangle();
	appendTriangles(rightInd, pairsRight, triangleDirection);

	pairsLeft= returnPairs(leftInd);
	std::vector<core::vector2df>  projectedLeft = returnProjected(checkedLeft,normal);
	writePolyFile(projectedLeft, pairsLeft);
	callTriangle();
	appendTriangles(leftInd, pairsLeft, !triangleDirection);


	static int id=0;
	char names[50];
	itoa(id,names,10);
	char folderleft[50] = "IntermediateSTL/left";
	strcat(folderleft,names);
	char folderright[50] = "IntermediateSTL/right";
	strcat(folderright,names);
	// get the direction of the parent node.
	int direction  = allnodes.at(selectednode).direction;

	// remove the old node from the list.
	allnodes.at(selectednode).node->remove();
	allnodes.erase(allnodes.begin()+selectednode);
	flipTriangles(rightInd);
	flipTriangles(leftInd);
	STLWriteFromVect(rightInd, checkedRight, folderright);
	scene::IAnimatedMeshSceneNode *rightNode = 0;
	rightNode = smgr->addAnimatedMeshSceneNode(smgr->getMesh(strcat(folderright,".stl")));
	rightNode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
	id++;
	//rightNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	smgr->getMeshManipulator()->setVertexColors(rightNode->getMesh(), video::SColor(255,145,0,123));
	rightNode->setPosition(rightNode->getAbsolutePosition() - core::vector3df(0,0,smallz));
	dirnode rightDirNode =  makedirnode(rightNode);
	setdir( smgr, rightDirNode,direction);
	allnodes.push_back(rightDirNode);

	STLWriteFromVect(leftInd, checkedLeft,folderleft);
	scene::IAnimatedMeshSceneNode *leftNode = 0;
	leftNode = smgr->addAnimatedMeshSceneNode(smgr->getMesh(strcat(folderleft,".stl")));
	leftNode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
	//leftNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	smgr->getMeshManipulator()->setVertexColors(leftNode->getMesh(), video::SColor(255,145,0,123));
	leftNode->setPosition(leftNode->getAbsolutePosition() - core::vector3df(0,0,smallz));
	dirnode leftDirNode =  makedirnode(leftNode);
	setdir( smgr, leftDirNode,direction);
	allnodes.push_back(leftDirNode);
	

	
	


	return 1;

}




// - core::vector3df(0,0,smallz)