

#include "tests.h"


bool testTriangleCol(){
	bool hit = 0;
	Triangle Test1;
	core::vector3df* I = new core::vector3df(0,0,0);
	Test1.V0 = core::vector3df(0,0,0);
	Test1.V1 = core::vector3df(1,0,0);
	Test1.V2 = core::vector3df(1,1,0);

	Tline testl1;
	testl1.start = core::vector3df(0.5,0.1,1);
	testl1.end = core::vector3df(0.5,0.1,-1);

	Tline testl2;
	testl2.start = core::vector3df(0,0.5,1);
	testl2.end = core::vector3df(0,0.5,-1);

	hit = intersect3D_RayTriangle(testl1,Test1,I);
	if(hit == true){
		hit =false;
	}
	hit = intersect3D_RayTriangle(testl2,Test1,I);
	if(hit == true){
		hit =false;
	}
	return hit;
}


void addlighting(scene::ISceneManager* smgr,float lightstr){

	scene::ILightSceneNode * light = smgr->addLightSceneNode(0, core::vector3df(0,0,0),
		video::SColorf(1.0f,1.0f,1.0f,1.0f), lightstr);


	scene::ILightSceneNode * light2 = smgr->addLightSceneNode(0, core::vector3df(150,150,150),
		video::SColorf(1.0f,1.0f,1.0f,1.0f), lightstr);


	scene::ILightSceneNode * light3 = smgr->addLightSceneNode(0, core::vector3df(0,150,150),
		video::SColorf(1.0f,1.0f,1.0f,1.0f), lightstr);


	scene::ILightSceneNode * light4 = smgr->addLightSceneNode(0, core::vector3df(150,150,0),
		video::SColorf(1.0f,1.0f,1.0f,1.0f),lightstr);

}

std::vector<Tline> makegrid(){
	std::vector<Tline> grid;
	Tline gridline;
	for(int j = 0; j<=15; j++){
		for(int k = 0; k<=15; k++){
			gridline.start = core::vector3df(0,j*10,k*10);
			gridline.end = core::vector3df(150,j*10,k*10);
			grid.push_back(gridline);
		}
	}
	for(int i = 0; i<=15; i++){
		for(int k = 0; k<=15; k++){
			gridline.start = core::vector3df(i*10,0,k*10);
			gridline.end = core::vector3df(i*10,150,k*10);
			grid.push_back(gridline);
		}
	}
	for(int j = 0; j<=15; j++){
		for(int i = 0; i<=15;i++){
			gridline.start = core::vector3df(i*10,j*10,0);
			gridline.end = core::vector3df(i*10,j*10,150);
			grid.push_back(gridline);
		}
	}
	return grid;
}