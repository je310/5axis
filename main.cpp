
#include <irrlicht.h>
#include "driverChoice.h"
//#include "octree.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <triangle3d.h>
#include <omp.h>
#include "types.h"
#include "readGcode.h"
#include "linkedlist.h"
#include "printnode.h"
#include "rotation.cpp"
#include "collision.h"
//#define USESILLYCHECK
#define vis

//#define MULTI
using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
#include <IMesh.h>



std::vector<core::vector3df> findCubesOfInterest(Tline myline);


enum
{
	// I use this ISceneNode ID to indicate a scene node that is
	// not pickable by getSceneNodeAndCollisionPointFromRay()
	ID_IsNotPickable = 0,

	// I use this flag in ISceneNode IDs to indicate that the
	// scene node can be picked by ray selection.
	IDFlag_IsPickable = 1 << 0,

	// I use this flag in ISceneNode IDs to indicate that the
	// scene node can be highlighted.  In this example, the
	// homonids can be highlighted, but the level mesh can't.
	IDFlag_IsHighlightable = 1 << 1
};


class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

std::vector<Tline> allLines;
void clear(char buffer[20]);
bool notCollided = 1;

bool nextpress =0;
int numberOfInstructions = 0;



int main()
{
	float CurrentA = 0; 
	float CurrentB = 0 ; 
	omp_set_num_threads(4);
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




	// ask user for driver
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;
	MyEventReceiver receiver;
	IrrlichtDevice *device =
		createDevice(driverType, core::dimension2d<u32>(1920, 1080), 16, false,false,false, &receiver);

	if (device == 0)
		return 1; // could not create selected driver.
	device->getCursorControl()->setVisible(false);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	scene::ICameraSceneNode* camera =
	smgr->addCameraSceneNodeFPS(0, 100.0f, .3f, ID_IsNotPickable, 0, 0, false, 3.f);
	camera->setPosition(core::vector3df(-75,75,40));
	camera->setTarget(core::vector3df(75,75,40));
	//camera->setUpVector(core::vector3df(0,0,1));

	scene::ITriangleSelector* selector = 0;
	smgr->setAmbientLight(video::SColorf(0.3,0.3,0.3,1));
	if (selector)
	{
		scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
			selector, camera, core::vector3df(30,50,30),
			core::vector3df(0,-10,0), core::vector3df(0,30,0));
		selector->drop(); // As soon as we're done with the selector, drop it.
		camera->addAnimator(anim);
		anim->drop();  // And likewise, drop the animator when we're done referring to it.
	}

	instruction savedVals;
	savedVals.A = 0;
	savedVals.B = 0;
	savedVals.X = 0;
	savedVals.Y = 0;
	savedVals.Z = 0;
	savedVals.G = 0;
	savedVals.E = 0;

	std::vector<instruction> instructions;
	instructions = ReadInGCode(savedVals);
	numberOfInstructions = instructions.size();

	driver->setTransform(video::ETS_WORLD,  core::matrix4());
	scene::IAnimatedMeshSceneNode* head = 0;
	head = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/hotendmock.stl"),
						0, ID_IsNotPickable);
	scene::IMeshBuffer *pBuffer;
	pBuffer = head->getMesh()->getMeshBuffer(0);

	head->setMaterialType(video::EMT_SOLID);
	head->setMaterialFlag(video::EMF_LIGHTING, true);
	
	head->setPosition(core::vector3df(5,5,1));
	head->updateAbsolutePosition();


	video::SMaterial material;


	std::vector<video::S3DVertex> vertvect;
	int VertexCount = pBuffer->getVertexCount();
	video::S3DVertex *vertices = (video::S3DVertex *)pBuffer->getVertices();
	video::S3DVertex *trans	= new video::S3DVertex();
	for(int i = 0; i< VertexCount; i++){
		vertvect.push_back(vertices[i]);
	}

	for(int i =0; i <VertexCount; i++){
		for (int j = i+1; j<VertexCount; j++){
			if(vertvect.at(i).Pos.equals(vertvect.at(j).Pos)){
				vertvect.erase(vertvect.begin()+ j);
				j--;
				VertexCount = vertvect.size();
			}
		}
	}

	material.Lighting = true;
	selector = smgr->createTriangleSelector(head);
	head->setTriangleSelector(selector);
	selector->drop();


	scene::IAnimatedMeshSceneNode * obj = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/cube.stl"),0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	if (obj){
		obj->setPosition(core::vector3df(0,0,223));
		obj->setMaterialTexture(0, driver->getTexture("../../media/wall.bmp"));
		obj->setMaterialFlag(video::EMF_LIGHTING, false);
	}
	selector = smgr->createTriangleSelector(obj);
	obj->setTriangleSelector(selector);
	selector->drop();
		
	scene::IBillboardSceneNode * bill;
	scene::IBillboardSceneNode * bill2;

	scene::ILightSceneNode * light = smgr->addLightSceneNode(0, core::vector3df(-60,100,400),
		video::SColorf(1.0f,1.0f,1.0f,1.0f), 600.0f);
	light->setID(ID_IsNotPickable); // Make it an invalid target for selection.

	// Remember which scene node is highlighted
	scene::ISceneNode* highlightedSceneNode = 0;
	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
	int lastFPS = -1;

	// draw the selection triangle only as wireframe
	material.Wireframe=true;

	int inscount = 0;

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
	

	scene::ISceneNode * Printed  = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/cube.stl"),0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	Printed->setVisible(false);

	int nextpressR = 0;
	bool nextpressR2 = true;
	std::vector<core::vector3df> mycubes;
	int bigcount = 0;
	bool forwards = 0;
#ifdef vis
	while(device->run())
	if (device->isWindowActive())
	{
		

#else
	while(1)
	if (1)
	{
#endif
		
		
		head->updateAbsolutePosition();

		core::vector3df intersection;
		
		core::triangle3df hitTriangle;
		scene::ISceneNode * selectedSceneNode;
		core::line3d<f32> ray;
		core::vector3df  nextmove=core::vector3df (instructions[inscount].X,instructions[inscount].Y,instructions[inscount].Z);
		core::vector3df  prevmove;
		if(inscount>0){
			prevmove=core::vector3df (instructions[inscount-1].X,instructions[inscount-1].Y,instructions[inscount-1].Z);
		}
		else{
			prevmove=core::vector3df (0,0,0);
		}
		
		Tline vertline;
#ifdef coll
#ifdef MULTI
		#pragma omp parallel
		{
		#pragma omp for
#endif
		for(int i = 0; i <VertexCount; i++){
				std::vector<core::vector3df> mycubes2;
				Tline vertline2;
				vertline2.start = vertices[i].Pos +prevmove;
				vertline2.end = vertices[i].Pos + nextmove;
				mycubes2 = findCubesOfInterest(vertline2);
				vertline2 = validateLine(vertline2);
				if(checkcollision(mycubes2,vertline2)){
					//std::cout << "impact";
				}

		}
#ifdef MULTI	
		}

#endif
#endif

			
		 nextpress = 1;
		// We're all done drawing, so end the scene.
		Tline myLine;
		if(!receiver.IsKeyDown(irr::KEY_KEY_E) && nextpress == 1){
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
						myLine.end= nextmove;
						myLine.start = prevmove;
						allLines.push_back(myLine);
						Printnode *print =new Printnode(Printed, smgr,IDFlag_IsPickable ,prevmove,nextmove,core::vector3df(0,0,1),0.5,0.2);
					}
					else if((instructions.at(inscount).E - instructions.at(inscount-1).E) >0){
						myLine.end= nextmove;
						myLine.start = prevmove;
						allLines.push_back(myLine);
						Printnode *print =new Printnode(Printed, smgr,IDFlag_IsPickable ,prevmove,nextmove,core::vector3df(0,0,1),0.5,0.2);

					}
				}
				inscount++;
			}
			else{

			}
			nextpress = 0;
			
		}
#ifdef vis		
		bigcount++;
		if(bigcount % drawat == 0){
		
		//revolveNodeInLocalSpace(head,3,core::vector3df(0,1,0),core::vector3df(-5,5,0));
		driver->beginScene(true, true, 0);
		if(receiver.IsKeyDown(irr::KEY_KEY_R) && nextpressR ==0){
			nextpressR = 1;
		}
		if(!receiver.IsKeyDown(irr::KEY_KEY_R) && nextpressR ==1){
			nextpressR2 = !nextpressR2;
			nextpressR =0;
		}
		
		if(!receiver.IsKeyDown(irr::KEY_KEY_Q)){
			if(nextpressR2){

			for(int i = 0; i<grid.size();i++){
				driver->setTransform(video::ETS_WORLD,  core::matrix4());
				driver->draw3DLine(grid.at(i).start,grid.at(i).end,video::SColor(20,50,210,100));
			}

			for(int i = allLines.size()-1000; i<allLines.size();i++){
				driver->setTransform(video::ETS_WORLD,  core::matrix4());
				driver->draw3DLine(allLines.at(i).start,allLines.at(i).end,video::SColor(200,50,210,200));
			}
		}
		else{
			for(int i = 0; i<allLines.size();i++){
				driver->setTransform(video::ETS_WORLD,  core::matrix4());
				driver->draw3DLine(allLines.at(i).start,allLines.at(i).end,video::SColor(200,50,210,200));
			}
		}
			Printed->setVisible(true);
			Printed->setVisible(false);
			smgr->drawAll();
		}
		else{
			Printed->setVisible(false);
		}
		driver->endScene();

		int fps = driver->getFPS()*drawat;

		if (lastFPS != fps)
		{
			core::stringw str = L"Collision detection example - Irrlicht Engine [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
#endif
	}
	}

	device->drop();

	return 0;
}





/*
**/

