
#include <irrlicht.h>
#include "driverChoice.h"
//#include "octree.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <triangle3d.h>
#include <omp.h>
//#include "stl.cpp"
#include "glm/glm.hpp"
#include <IMesh.h>
#include <fstream>
#include <stdint.h>
//#define USESILLYCHECK
#define vis
//#define coll



//#define MULTI
using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
#include <IMesh.h>
#include <IColladaMeshWriter.h>
#include <IMeshWriter.h>
#include <EMeshWriterEnums.h>

#include "rotation.h"
#include "offsets.h"
#include "stl.h"
#include "gcode.h"
#include "collision.h"
#include "list.h"
#include "tests.h"
//#include "printnode.h"


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


list TriangArr[numbox][numbox][numbox];



class Printnode : public scene::ISceneNode
{

	/*
	First, we declare some member variables:
	The bounding box, 4 vertices, and the material of the tetraeder.
	*/
	core::aabbox3d<f32> Box;
	video::S3DVertex Vertices[8];
	video::SMaterial Material;

	/*
	The parameters of the constructor specify the parent of the scene node,
	a pointer to the scene manager, and an id of the scene node.
	In the constructor we call the parent class' constructor,
	set some properties of the material, and
	create the 4 vertices of the tetraeder we will draw later.
	*/

public:

	Printnode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,core::vector3df Start, core::vector3df End,core::vector3df Up,float Width, float Height)
		: scene::ISceneNode(parent, mgr, id)
	{


		core::vector3df length = End - Start;
		core::vector3df WidthVect = Up.crossProduct(length);
		WidthVect = (WidthVect.normalize()*Width)/2;
		std::vector<core::vector3df> mycubes;
		Tline myline;
		myline.end = End;
		myline.start = Start;
		mycubes = findCubesOfInterest(myline);
		int numofcubes = mycubes.size();

		core::vector3df PrintHeight = Up.normalize()*Height;

		core::vector3df Corn0 = Start - WidthVect;
		core::vector3df Corn1 = Start + WidthVect;
		core::vector3df Corn2 = Start - WidthVect - PrintHeight;
		core::vector3df Corn3 = Start + WidthVect - PrintHeight;
		core::vector3df Corn4 = End - WidthVect;
		core::vector3df Corn5 = End + WidthVect;
		core::vector3df Corn6 = End - WidthVect - PrintHeight;
		core::vector3df Corn7 = End + WidthVect - PrintHeight;
		Triangle triangle[12];

		triangle[0].V0 = Corn3;
		triangle[0].V1 = Corn2;
		triangle[0].V2 = Corn0;

		triangle[1].V0 = Corn0;
		triangle[1].V1 = Corn1;
		triangle[1].V2 = Corn3;

		triangle[2].V0 = Corn5;
		triangle[2].V1 = Corn1;
		triangle[2].V2 = Corn0;

		triangle[3].V0 = Corn0;
		triangle[3].V1 = Corn4;
		triangle[3].V2 = Corn5;

		triangle[4].V0 = Corn3;
		triangle[4].V1 = Corn1;
		triangle[4].V2 = Corn5;

		triangle[5].V0 = Corn3;
		triangle[5].V1 = Corn5;
		triangle[5].V2 = Corn7;

		triangle[6].V0 = Corn5;
		triangle[6].V1 = Corn4;
		triangle[6].V2 = Corn6;

		triangle[7].V0 = Corn6;
		triangle[7].V1 = Corn7;
		triangle[7].V2 = Corn5;

		triangle[8].V0 = Corn0;
		triangle[8].V1 = Corn2;
		triangle[8].V2 = Corn4;

		triangle[9].V0 = Corn2;
		triangle[9].V1 = Corn6;
		triangle[9].V2 = Corn4;

		triangle[10].V0 = Corn2;
		triangle[10].V1 = Corn3;
		triangle[10].V2 = Corn7;

		triangle[0].V0 = Corn6;
		triangle[0].V1 = Corn2;
		triangle[0].V2 = Corn7;


		for(int i = 0; i < numofcubes ; i++){
			for(int j = 0; j < 12; j++){
				TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].addNode(TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].initNode(triangle[j]));
			}

		}
		//3,2,0, 0,1,3, 5,1,0, 0,4,5, 3,1,5, 3,5,7, 5,4,6, 6,7,5, 0,2,4, 2,6,4, 2,3,7, 6,2,7

		Material.Wireframe = false;
		Material.Lighting = false;
		video::SColor colour(128,126,89,200);
		float widthover2 = 0.25;
		float height = 0.2;
		Vertices[0] = video::S3DVertex(Corn0.X,Corn0.Y,Corn0.Z, 1,1,0,colour, 0, 1);
		Vertices[1] = video::S3DVertex(Corn1.X,Corn1.Y,Corn1.Z, 1,0,0,colour, 1, 1);
		Vertices[2] = video::S3DVertex(Corn2.X,Corn2.Y,Corn2.Z, 0,1,1,colour, 1, 0);
		Vertices[3] = video::S3DVertex(Corn3.X,Corn3.Y,Corn3.Z, 0,0,1,colour, 0, 0);
		Vertices[4] = video::S3DVertex(Corn4.X,Corn4.Y,Corn4.Z, 1,1,0,colour, 0, 1);
		Vertices[5] = video::S3DVertex(Corn5.X,Corn5.Y,Corn5.Z, 1,0,0,colour, 1, 1);
		Vertices[6] = video::S3DVertex(Corn6.X,Corn6.Y,Corn6.Z, 0,1,1,colour, 1, 0);
		Vertices[7] = video::S3DVertex(Corn7.X,Corn7.Y,Corn7.Z, 0,0,1,colour, 0, 0);

		/*
		The Irrlicht Engine needs to know the bounding box of a scene node.
		It will use it for automatic culling and other things. Hence, we
		need to create a bounding box from the 4 vertices we use.
		If you do not want the engine to use the box for automatic culling,
		and/or don't want to create the box, you could also call
		irr::scene::ISceneNode::setAutomaticCulling() with irr::scene::EAC_OFF.
		*/
		Box.reset(Vertices[0].Pos);
		for (s32 i=1; i<8; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
	}

	/*
	Before it is drawn, the irr::scene::ISceneNode::OnRegisterSceneNode()
	method of every scene node in the scene is called by the scene manager.
	If the scene node wishes to draw itself, it may register itself in the
	scene manager to be drawn. This is necessary to tell the scene manager
	when it should call irr::scene::ISceneNode::render(). For
	example, normal scene nodes render their content one after another,
	while stencil buffer shadows would like to be drawn after all other
	scene nodes. And camera or light scene nodes need to be rendered before
	all other scene nodes (if at all). So here we simply register the
	scene node to render normally. If we would like to let it be rendered
	like cameras or light, we would have to call
	SceneManager->registerNodeForRendering(this, SNRT_LIGHT_AND_CAMERA);
	After this, we call the actual
	irr::scene::ISceneNode::OnRegisterSceneNode() method of the base class,
	which simply lets also all the child scene nodes of this node register
	themselves.
	*/
	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	/*
	In the render() method most of the interesting stuff happens: The
	Scene node renders itself. We override this method and draw the
	tetraeder.
	*/
	virtual void render()
	{
		u16 indices[] = {	3,2,0, 0,1,3, 5,1,0, 0,4,5, 3,1,5, 3,5,7, 5,4,6, 6,7,5, 0,2,4, 2,6,4, 2,3,7, 6,2,7	};
		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList(&Vertices[0], 8, &indices[0], 12, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
	}

	/*
	And finally we create three small additional methods.
	irr::scene::ISceneNode::getBoundingBox() returns the bounding box of
	this scene node, irr::scene::ISceneNode::getMaterialCount() returns the
	amount of materials in this scene node (our tetraeder only has one
	material), and irr::scene::ISceneNode::getMaterial() returns the
	material at an index. Because we have only one material here, we can
	return the only one material, assuming that no one ever calls
	getMaterial() with an index greater than 0.
	*/
	virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

	virtual u32 getMaterialCount() const
	{
		return 1;
	}

	virtual video::SMaterial& getMaterial(u32 i)
	{
		return Material;
	}	
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
std::vector<instruction> ReadInGCode(instruction savedVals,char* argv);
void faceTarget(Printnode& hero, scene::IAnimatedMeshSceneNode& target);
void clear(char buffer[20]);
bool notCollided = 1;

int numberOfInstructions = 0;


int main(int argc, char* argv[])
{
	//
	//	Setup irlicht in a sensible way
	//
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
	driver->setTransform(video::ETS_WORLD,  core::matrix4());
	//add lighting to the environment
	addlighting(smgr,80);



	//set some variables for first use.
	scene::IMeshBuffer *pBuffer; // used to store mesh data
	float CurrentA = 0; 
	float CurrentB = 0 ; 
	instruction savedVals;
	savedVals.A = 0;
	savedVals.B = 0;
	savedVals.X = 0;
	savedVals.Y = 0;
	savedVals.Z = 0;
	savedVals.G = 0;
	savedVals.E = 0;
	float smallz = 1000;
	video::SMaterial material;

	//slice the stl denoted by the comand line input (TEST ONLY)
	slice(argv[1]);

	//read in the newly sliced gcode 
	std::vector<instruction> instructions;
	instructions = ReadInGCode(savedVals, argv[1]);
	numberOfInstructions = instructions.size();

	//load the head model
	scene::IAnimatedMeshSceneNode* head = 0;
	head = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/hotendmock.stl"),
		0, ID_IsNotPickable);
	smgr->getMeshManipulator()->setVertexColors(head->getMesh(), video::SColor(255,0,0,255));
	head->setPosition(core::vector3df(5,5,1));
	head->updateAbsolutePosition();


	//section that loads the test model stls 
	scene::IAnimatedMeshSceneNode* zcore = 0;
	zcore = smgr->addAnimatedMeshSceneNode(smgr->getMesh("stl/(Z1)core.stl"),
		0, ID_IsNotPickable);
	smgr->getMeshManipulator()->setVertexColors(zcore->getMesh(), video::SColor(255,145,0,123));

	//load stl from input args, this is the translucent 'final model'
	scene::IAnimatedMeshSceneNode* thing = readstl(smgr,argv[1]);
	mirrorx(thing);
	thing->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	smgr->getMeshManipulator()->setVertexColors(thing->getMesh(), video::SColor(99,145,0,123));

	// write back the model to stl as a test
	pBuffer = thing->getMesh()->getMeshBuffer(0);
	writestl(pBuffer, "stloutputtest");


	//find the middle of the object in the same manner that slic3r does and reposition accordingly
	core::vector3df midpointtest = findmodeloffset(thing, 0,0, 0);
	thing->setPosition(thing->getAbsolutePosition()-midpointtest);

	//find the distance to bed of a print object, set the model to the bed
	smallz = findlowestvert(zcore);
	zcore->setPosition(zcore->getAbsolutePosition() - core::vector3df(0,0,smallz));

	//load the models that are adjacent in the input stl model space
	scene::IAnimatedMeshSceneNode* y1core = 0;
	y1core = smgr->addAnimatedMeshSceneNode(smgr->getMesh("stl/(Y1)core.stl"),
		0, ID_IsNotPickable);
	smgr->getMeshManipulator()->setVertexColors(y1core->getMesh(), video::SColor(0,145,124,123));
	y1core->setPosition(y1core->getAbsolutePosition() - core::vector3df(0,0,smallz));

	scene::IAnimatedMeshSceneNode* y2core = 0;
	y2core = smgr->addAnimatedMeshSceneNode(smgr->getMesh("stl/(Y2)core.stl"),
		0, ID_IsNotPickable);
	smgr->getMeshManipulator()->setVertexColors(y2core->getMesh(), video::SColor(26,145,0,123));
	y2core->setPosition(y2core->getAbsolutePosition() - core::vector3df(0,0,smallz));

	//test the rotation on both the side peices
	rotateNodeInWorldSpace(y2core,90,core::vector3df(0,1,0), core::vector3df(0,0,50));
	rotateNodeInWorldSpace(y1core,-90,core::vector3df(0,1,0), core::vector3df(0,0,50));


	int lastFPS = -1;
	int inscount = 0;

#ifdef GRID
	std::vector<Tline> grid = makegrid;

#endif

	//represents the printed material
	scene::ISceneNode * Printed  = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/cube.stl"),0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	Printed->setVisible(false);

	//latch variables for choosing the amount of lines to show. 
	int nextpressR = 0;
	bool nextpressR2 = true;

	//the cubes to be inspected
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

			//make sure the head position is updated
			head->updateAbsolutePosition();

			core::vector3df intersection;
			core::triangle3df hitTriangle;
			core::line3d<f32> ray;

			// update next move and prevmove
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
#pragma omp  for
			//		{
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
			//	}

#endif
#endif




			Tline myLine;
			if(!receiver.IsKeyDown(irr::KEY_KEY_E)){
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
							Printnode *print =new Printnode(Printed, smgr,IDFlag_IsPickable ,prevmove,nextmove,core::vector3df(0,0,1),0.5,0.2);
						}
						else if((instructions.at(inscount).E - instructions.at(inscount-1).E) >0){
							myLine.end= nextmove;
							myLine.start = prevmove;
							allLines.push_back(myLine);
							Printnode *print =new Printnode(Printed, smgr,IDFlag_IsPickable ,prevmove,nextmove,core::vector3df(0,0,1),0.5,0.2);

							//if(instructions.at(inscount).Z>25 || instructions.at(inscount).Y>150 || instructions.at(inscount).Z<-1 ||instructions.at(inscount).Y<0||instructions.at(inscount).X>140){
							//	inscount =inscount;
							//}
						}
					}
					inscount++;
				}
				else{
					//if(bigcount % drawat == 

					//std::cout << "done";
				}


			}
#ifdef vis		
			bigcount++;
			if(bigcount % drawat == 0){ //olny render infrequently
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
#ifdef GRID
						for(int i = 0; i<grid.size();i++){
							driver->setTransform(video::ETS_WORLD,  core::matrix4());

							driver->draw3DLine(grid.at(i).start,grid.at(i).end,video::SColor(20,50,210,100));

						}
#endif
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

