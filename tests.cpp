

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

	scene::ILightSceneNode * light = smgr->addLightSceneNode(0, core::vector3df(-150,-150,150),
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

void doprint(scene::IAnimatedMeshSceneNode* head, std::vector<instruction> instructions,std::vector<Tline> &allLines, int &inscount, float CurrentA, float CurrentB){
	int numberOfInstructions  = instructions.size(); 
	//make sure the head position is updated
	head->updateAbsolutePosition();

	// update next move and prevmove
	core::vector3df  nextmove=core::vector3df (instructions[inscount].X,instructions[inscount].Y,instructions[inscount].Z);
	core::vector3df  prevmove;
	if(inscount>0){
		prevmove=core::vector3df (instructions[inscount-1].X,instructions[inscount-1].Y,instructions[inscount-1].Z);
	}
	else{
		prevmove=core::vector3df (0,0,0);
	}

	Tline myLine;
	if(1){
		if(inscount < numberOfInstructions - 1){

			if(instructions.at(inscount).A != CurrentA){
				revolveNodeInWorldSpace(head,instructions.at(inscount).A-CurrentA,core::vector3df(1,0,0),core::vector3df(BuildSize/2,BuildSize/2,40));
				CurrentA = instructions.at(inscount).A;
				//camera->setUpVector(core::vector3df(sin(core::DEGTORAD*CurrentA),0,cos(core::DEGTORAD*CurrentA)));
			}
			if(instructions.at(inscount).B != CurrentB){
				revolveNodeInWorldSpace(head,instructions.at(inscount).B-CurrentB,core::vector3df(0,1,0),core::vector3df(BuildSize/2,BuildSize/2,40));
				CurrentA = instructions.at(inscount).B;
			}
			//inscount++;
			if(instructions.at(inscount).G ==1  && inscount < numberOfInstructions - 2){
				head->setPosition(Headoffset(nextmove,head));
				head->updateAbsolutePosition();
				//core::vector3df  nextnextmove;
				//nextnextmove = core::vector3df (instructions.at(inscount).X,instructions.at(inscount).Y,instructions.at(inscount).Z);
				if(inscount ==0 ){
					myLine.end= nextmove  ;
					myLine.start = prevmove;
					allLines.push_back(myLine);
				}
				else if((instructions.at(inscount).E - instructions.at(inscount-1).E) >0){
					myLine.end= nextmove;
					myLine.start = prevmove;
					allLines.push_back(myLine);
				}
			}
			inscount++;
		}


	}
}
video::SColor greencol  =	video::SColor(255,34,177,76);
video::SColor pinkcol  =	video::SColor(255,255,174,201);
video::SColor bluecol  =	video::SColor(255,63,72,204);
video::SColor redcol  =		video::SColor(255,237,28,36);
video::SColor yellowcol  =  video::SColor(255,255,201,14);



void setdir(scene::ISceneManager* smgr, scene::IAnimatedMeshSceneNode* selectednode,int current){
	switch (current){
	case GUI_ID_YELLOW:
		smgr->getMeshManipulator()->setVertexColors(selectednode->getMesh(), yellowcol);
		break;

	case GUI_ID_PINK:
		smgr->getMeshManipulator()->setVertexColors(selectednode->getMesh(), pinkcol);
		break;

	case GUI_ID_RED:
		smgr->getMeshManipulator()->setVertexColors(selectednode->getMesh(), redcol);
		break;

	case GUI_ID_BLUE:
		smgr->getMeshManipulator()->setVertexColors(selectednode->getMesh(), bluecol);
		break;

	case GUI_ID_GREEN:
		smgr->getMeshManipulator()->setVertexColors(selectednode->getMesh(), greencol);
		break;

	};
}