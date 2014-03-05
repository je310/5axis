
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
//using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


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



// Define some values that we'll use to identify individual GUI controls.

int current = -1;

float pinkalpha = 70;
float yellowalpha = 70;
float redalpha = 70;
float bluealpha = 70;
float greenalpha = 70;
float blackalpha = 70;

bool sliced  = 0;
std::vector<instruction> instructions;

instruction savedVals;
float smallz = 1000;




std::vector<dirnode> allnodes;

	struct SAppContext
	{
		IrrlichtDevice *device;
		s32				counter;
		IGUIListBox*	listbox;
	}Context;



bool selectmode = 0;
class MyEventReceiver : public IEventReceiver
{
public:
	

	struct SMouseState
	{
		core::position2di Position;
		core::position2di LeftDownPos;
		core::position2di RightDownPos;
		bool LeftButtonDown;
		bool RightButtonDown;
		SMouseState() : LeftButtonDown(false),RightButtonDown(false) { }
	} MouseState;



	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{

		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* env = Context.device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{

				/*
				If a scrollbar changed its scroll position, and it is
				'our' scrollbar (the one with id GUI_ID_TRANSPARENCY_SCROLL_BAR), then we change
				the transparency of all gui elements. This is a very
				easy task: There is a skin object, in which all color
				settings are stored. We simply go through all colors
				stored in the skin and change their alpha value.
				*/
			case EGET_SCROLL_BAR_CHANGED:
				if (id == GUI_ID_TRANSPARENCY_SCROLL_BAR)
				{
					s32 pos = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();

					for (u32 i=0; i<EGDC_COUNT ; ++i)
					{
						SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
						col.setAlpha(pos);
						env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
					}

				}
				break;

				/*
				If a button was clicked, it could be one of 'our'
				three buttons. If it is the first, we shut down the engine.
				If it is the second, we create a little window with some
				text on it. We also add a string to the list box to log
				what happened. And if it is the third button, we create
				a file open dialog, and add also this as string to the list box.
				That's all for the event receiver.
				*/
			case EGET_BUTTON_CLICKED:
				switch(id)
				{
				case GUI_ID_QUIT_BUTTON:
					Context.device->closeDevice();
					return true;

				case GUI_ID_SELECT_SECTION:
					selectmode = 1;
					return true;

				case GUI_ID_YELLOW:
					if (current == GUI_ID_YELLOW){
						yellowalpha = 70;
						current = -1;
					}
					else{
						resetsols();
						yellowalpha = 255;
						current = GUI_ID_YELLOW;
					}
					return true;
				case GUI_ID_PINK:
					if (current == GUI_ID_PINK){
						pinkalpha = 70;
						current = -1;
					}
					else{
						resetsols();
						pinkalpha = 255;
						current = GUI_ID_PINK;
					}
					return true;
				case GUI_ID_RED:
					if (current == GUI_ID_RED){
						redalpha = 70;
						current = -1;
					}
					else{
						resetsols();
						redalpha = 255;
						current = GUI_ID_RED;
					}
					return true;
				case GUI_ID_BLUE:
					if (current == GUI_ID_BLUE){
						bluealpha = 70;
						current = -1;
					}
					else{
						resetsols();
						bluealpha = 255;
						current = GUI_ID_BLUE;
					}
					return true;
				case GUI_ID_GREEN:
					if (current == GUI_ID_GREEN){
						greenalpha = 70;
						current = -1;
					}
					else{
						resetsols();
						greenalpha = 255;
						current = GUI_ID_GREEN;
					}
					return true;

				case GUI_ID_MOVE:
					selectmode = 0;
					return true;

				case GUI_ID_NEW_WINDOW_BUTTON:
					{
						//Context.listbox->addItem(L"Window created");
						Context.counter += 30;
						if (Context.counter > 200)
							Context.counter = 0;

						IGUIWindow* window = env->addWindow(
							core::rect<s32>(100 + Context.counter, 100 + Context.counter, 300 + Context.counter, 200 + Context.counter),
							false, // modal?
							L"Test window");

						env->addStaticText(L"Please close me",
							core::rect<s32>(35,35,140,50),
							true, // border?
							false, // wordwrap?
							window);
					}
					return true;

				case GUI_ID_FILE_OPEN_BUTTON:
					//Context.listbox->addItem(L"File open");
					// There are some options for the file open dialog
					// We set the title, make it a modal window, and make sure
					// that the working directory is restored after the dialog
					// is finished.
					env->addFileOpenDialog(L"Please choose a file.", false, 0, -1, true);
					return true;

				case GUI_ID_SLICE:
					scene::IMeshBuffer *mesh;
					for(int i = 0; i< allnodes.size(); i++){
						mesh = allnodes.at(i).node->getMesh()->getMeshBuffer(0);
						char path[10] = "";
						itoa(i,path,10);
						rotatemesh(mesh,allnodes.at(i).direction);
						writestl(mesh,path);
						slice(path);
						core::vector3df midpointtest = findmodeloffset(allnodes.at(i).node, 0,0, 0);
						std::vector<instruction> curr;
						curr = ReadInGCode(savedVals,path);
						for(int j =0; j< curr.size(); j++){
							curr.at(j).X += midpointtest.X;
							curr.at(j).Y += midpointtest.Y; 
							curr.at(j).Z += midpointtest.Z  -smallz; 
							core::vector3df point = core::vector3df(curr.at(j).X,curr.at(j).Y,curr.at(j).Z);
							//rotateline(point,allnodes.at(i).direction);
							//curr.at(j).X = point.X;
							//curr.at(j).Y = point.Y; 
							//curr.at(j).Z = point.Z; 
						}
						instructions.insert(instructions.end(), curr.begin(), curr.end());
					}
					sliced = 1;
					
					return true;

				default:
					return false;
				}
				break;

			case EGET_FILE_SELECTED:
				{
					// show the model filename, selected in the file dialog
					IGUIFileOpenDialog* dialog =
						(IGUIFileOpenDialog*)event.GUIEvent.Caller;
					Context.listbox->addItem(dialog->getFileName());
				}
				break;

			default:
				break;
			}
		}
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		// Remember the mouse state
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			switch(event.MouseInput.Event)
			{
			case EMIE_LMOUSE_PRESSED_DOWN:
				MouseState.LeftButtonDown = true;
				MouseState.LeftDownPos.X =  event.MouseInput.X;
				MouseState.LeftDownPos.Y =  event.MouseInput.Y;
				break;

			case EMIE_RMOUSE_PRESSED_DOWN:
				MouseState.RightButtonDown = true;
				MouseState.RightDownPos.X =  event.MouseInput.X;
				MouseState.RightDownPos.Y =  event.MouseInput.Y;
				break;

			case EMIE_LMOUSE_LEFT_UP:
				MouseState.LeftButtonDown = false;
				break;

			case EMIE_RMOUSE_LEFT_UP:
				MouseState.RightButtonDown = false;
				break;

			case EMIE_MOUSE_MOVED:
				MouseState.Position.X = event.MouseInput.X;
				MouseState.Position.Y = event.MouseInput.Y;
				break;

			default:
				// We won't use the wheel
				break;
			}
		}

		return false;
	}

	const SMouseState & GetMouseState(void) const
	{
		return MouseState;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

	MyEventReceiver(SAppContext & context) : Context(context)
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
	SAppContext & Context;

	void resetsols(){
		yellowalpha = 70;
		pinkalpha = 70;
		redalpha = 70;
		bluealpha = 70;
		greenalpha = 70;
	}

};








std::vector<Tline> allLines;
std::vector<instruction> ReadInGCode(instruction savedVals,char* argv);
//void faceTarget(Printnode& hero, scene::IAnimatedMeshSceneNode& target);
void clear(char buffer[20]);
bool notCollided = 1;

int numberOfInstructions = 0;

const int ResY = 768;
const int ResX = 1360;

int cubex = 1000;
int cubey  =450;





int main(int argc, char* argv[])
{
	//
	//	Setup irlicht in a sensible way
	//
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;
	IrrlichtDevice *device;



	device =
		createDevice(driverType, core::dimension2d<u32>(ResX, ResY), 16, false,false,false);
	if (device == 0)
		return 1; // could not create selected driver.
	device->getCursorControl()->setVisible(true);
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	scene::ICameraSceneNode* camera =
		smgr->addCameraSceneNode(0, core::vector3df(50,0,0), core::vector3df(0,0,0));
	camera->setPosition(core::vector3df(-75,75,40));
	camera->setTarget(core::vector3df(75,75,40));
	camera->setUpVector(core::vector3df(0,0,1));
	smgr->setAmbientLight(video::SColorf(0.3,0.3,0.3,1));
	driver->setTransform(video::ETS_WORLD,  core::matrix4());
	//add lighting to the environment
	addlighting(smgr,80);

	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	gui::IGUIEnvironment* invis = device->getGUIEnvironment();
	gui::IGUISkin* skin = env->getSkin();
	gui::IGUIFont* font = env->getFont("../../media/fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);



	skin->setFont(env->getBuiltInFont(), gui::EGDF_TOOLTIP);
		env->addButton(core::rect<s32>(10,240,ResX/8,240 + 64), 0, GUI_ID_QUIT_BUTTON,
			L"Quit", L"Exits Program");
	env->addButton(core::rect<s32>(10,320,ResX/8,320 + 64), 0, GUI_ID_NEW_WINDOW_BUTTON,
			L"New Window", L"Launches a new Window");
	env->addButton(core::rect<s32>(10,380,ResX/8,380 + 64), 0, GUI_ID_FILE_OPEN_BUTTON,
			L"File Open", L"Opens a file");
	env->addButton(core::rect<s32>(10,440,ResX/8,440 + 64), 0, GUI_ID_SLICE,
			L"5lice", L"slices the file");

	//gui::IGUIButton* yellow;
	//yellow->setText("yellow");
	//yellow->setUseAlphaChannel();
	gui::IGUIButton* yellow = env->addButton(core::rect<s32>(cubex+17,cubey+168,cubex+17 +120,cubey+157 + 40), 0, GUI_ID_YELLOW,
		L"", L"switches to a mode where you can move your view");
	
	yellow->setUseAlphaChannel();
	yellow->setImage(driver->getTexture("cube/blank.png"));
	yellow->setDrawBorder(0);
	
	gui::IGUIButton* green = env->addButton(core::rect<s32>(cubex+167,cubey+146,cubex+167 +110,cubey+146 + 110), 0, GUI_ID_GREEN,
			L"", L"switches to a mode where you can move your view");
	green->setUseAlphaChannel();
	green->setImage(driver->getTexture("cube/blank.png"));
	green->setDrawBorder(0);

	gui::IGUIButton* red = env->addButton(core::rect<s32>(cubex+182,cubey+110,cubex+182 +100,cubey+110 + 40), 0, GUI_ID_RED,
			L"", L"switches to a mode where you can move your view");
	red->setUseAlphaChannel();
	red->setImage(driver->getTexture("cube/blank.png"));
	red->setDrawBorder(0);

	gui::IGUIButton* blue = env->addButton(core::rect<s32>(cubex+272,cubey+122,cubex+272 +30,cubey+122 + 90), 0, GUI_ID_BLUE,
			L"", L"switches to a mode where you can move your view");
	blue->setUseAlphaChannel();
	blue->setImage(driver->getTexture("cube/blank.png"));
	blue->setDrawBorder(0);

	gui::IGUIButton* pink = env->addButton(core::rect<s32>(cubex+252,cubey+15,cubex+252 +45,cubey+15 + 75), 0, GUI_ID_PINK,
			L"", L"switches to a mode where you can move your view");
	pink->setUseAlphaChannel();
	pink->setImage(driver->getTexture("cube/blank.png"));
	pink->setDrawBorder(0);

	scene::ISceneNode* n = smgr->addCubeSceneNode(100,0,-1,core::vector3df(0,0,-0.01),core::vector3df(0,0,180),core::vector3df(1,1,0.000001));
	n->setMaterialTexture(0, driver->getTexture("cube/bed.png"));
	n->setMaterialFlag(video::EMF_LIGHTING, false);
	n->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

	

	//	env->addStaticText(L"Transparent Control:", core::rect<s32>(150,20,350,40), true);
	//IGUIScrollBar* scrollbar = env->addScrollBar(true,
	//		core::rect<s32>(150, 45, 350, 60), 0, GUI_ID_TRANSPARENCY_SCROLL_BAR);
	//scrollbar->setMax(255);

	////// set scrollbar position to alpha value of an arbitrary element			
	//scrollbar->setPos(invis->getSkin()->getColor(EGDC_WINDOW).getAlpha());

	//env->addStaticText(L"Logging ListBox:", core::rect<s32>(50,110,250,130), true);->setUseAlphaChannel()->setImage(driver->getTexture("cube/blank.png")->,core::rect<s32>(0,0,300,256)
	//IGUIListBox * listbox = env->addListBox(core::rect<s32>(50, 140, 250, 210));
	//env->addEditBox(L"Editable Text", core::rect<s32>(350, 80, 550, 100));

	// Store the appropriate data in a context structure.


	// Then create the event receiver, giving it that context structure.
	SAppContext context;
	context.device = device;
	context.counter = 0;
	context.listbox = 0;

	MyEventReceiver receiver(context);

	// And tell the device to use our custom event receiver.
	device->setEventReceiver(&receiver);



	//set some variables for first use.
	scene::IMeshBuffer *pBuffer; // used to store mesh data
	float CurrentA = 0; 
	float CurrentB = 0 ; 

	
	video::SMaterial material;

	//slice the stl denoted by the comand line input (TEST ONLY)
	//slice(argv[1]);

	//read in the newly sliced gcode 

	savedVals.A = 0;
	savedVals.B = 0;
	savedVals.X = 0;
	savedVals.Y = 0;
	savedVals.Z = 0;
	savedVals.G = 0;
	savedVals.E = 0;

	//load the head model
	scene::IAnimatedMeshSceneNode* head = 0;
	head = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/hotendmock.stl"),
		0, ID_IsNotPickable);
	smgr->getMeshManipulator()->setVertexColors(head->getMesh(), video::SColor(255,0,0,255));
	head->setPosition(core::vector3df(5,5,1));
	head->updateAbsolutePosition();
	head->setVisible(false);


	//section that loads the test model stls 
	scene::IAnimatedMeshSceneNode* zcore = 0;
	zcore = smgr->addAnimatedMeshSceneNode(smgr->getMesh("stl/(Z1)core.stl"),
		0, IDFlag_IsPickable);
	smgr->getMeshManipulator()->setVertexColors(zcore->getMesh(), video::SColor(120,145,0,123));
	zcore->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

	//load stl from input args, this is the translucent 'final model'
	//scene::IAnimatedMeshSceneNode* thing = readstl(smgr,argv[1]);
	//mirrorx(thing);
	//thing->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	//smgr->getMeshManipulator()->setVertexColors(thing->getMesh(), video::SColor(99,145,0,123));

	// write back the model to stl as a test
	//pBuffer = thing->getMesh()->getMeshBuffer(0);
	//writestl(pBuffer, "stloutputtest");


	//find the middle of the object in the same manner that slic3r does and reposition accordingly
	//core::vector3df midpointtest = findmodeloffset(thing, 0,0, 0);
	//thing->setPosition(thing->getAbsolutePosition()-midpointtest);

	//find the distance to bed of a print object, set the model to the bed
	smallz = findlowestvert(zcore);
	zcore->setPosition(zcore->getAbsolutePosition() - core::vector3df(0,0,smallz));

	//load the models that are adjacent in the input stl model space
	scene::IAnimatedMeshSceneNode* y1core = 0;
	y1core = smgr->addAnimatedMeshSceneNode(smgr->getMesh("stl/(Y1)core.stl"),
		0, IDFlag_IsPickable);
	smgr->getMeshManipulator()->setVertexColors(y1core->getMesh(), video::SColor(120,145,0,123));
	y1core->setPosition(y1core->getAbsolutePosition() - core::vector3df(0,0,smallz));
	y1core->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

	scene::IAnimatedMeshSceneNode* y2core = 0;
	y2core = smgr->addAnimatedMeshSceneNode(smgr->getMesh("stl/(Y2)core.stl"),
		0, IDFlag_IsPickable);
	smgr->getMeshManipulator()->setVertexColors(y2core->getMesh(), video::SColor(120,145,0,123));
	y2core->setPosition(y2core->getAbsolutePosition() - core::vector3df(0,0,smallz));
	y2core->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

	//test the rotation on both the side peices
	//rotateNodeInWorldSpace(y2core,90,core::vector3df(0,1,0), core::vector3df(0,0,50));
	//rotateNodeInWorldSpace(y1core,-90,core::vector3df(0,1,0), core::vector3df(0,0,50));

	
	allnodes.push_back(makedirnode(zcore));
	allnodes.push_back(makedirnode(y1core));
	allnodes.push_back(makedirnode(y2core));
	int lastFPS = -1;
	int inscount = 0;

#ifdef GRID
	std::vector<Tline> grid = makegrid;

#endif

	//represents the printed material
	scene::ISceneNode * Printed;
	//latch variables for choosing the amount of lines to show. 
	int nextpressR = 0;
	bool nextpressR2 = true;
	//the cubes to be inspected
	std::vector<core::vector3df> mycubes;
	int bigcount = 0;
	bool forwards = 0;
	core::vector3df intersection;
	core::triangle3df hitTriangle;
	Tline ray;
	int selectednode;

	//scene::IBillboardSceneNode * bill = smgr->addBillboardSceneNode();
	//bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
	//bill->setMaterialTexture(0, driver->getTexture("../../media/particle.bmp"));
	//bill->setMaterialFlag(video::EMF_LIGHTING, false);
	//bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	//bill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	//bill->setID(ID_IsNotPickable); 
	core::position2di newPosition;
	core::position2di oldPosition =core::position2di(0,0);
	float xmovemouse = 0;
	float ymovemouse = 0;
	core::position2di rnewPosition;
	core::position2di roldPosition =core::position2di(0,0);
	float rxmovemouse = 0;
	float rymovemouse = 0;
	static core::vector3df campos = core::vector3df(0,0,0);
	static core::vector3df camoffset = core::vector3df(0,0,0);
	float diam = 90;
	float height = 0;
	bool held = 0;
	bool rheld = 0;
	static core::vector3df nearleftup;
	static core::vector3df nearleftdown;
	static core::vector3df downvect ;
	static float lookside = 0;
	static float lookup = 0;
#ifdef vis
	while(device->run())
		if (device->isWindowActive())
		{
#else
	while(1)
		if (1)
		{
#endif
			//virtualprint move
			if(sliced){
				doprint(head,instructions,allLines,Printed, inscount, CurrentA,CurrentB);
				head->setVisible(true);
			}


			if(receiver.GetMouseState().LeftButtonDown && current ==-1){
				if(held){
					
					newPosition = receiver.MouseState.Position;
					xmovemouse = xmovemouse - newPosition.X + oldPosition.X ;
					ymovemouse = ymovemouse + newPosition.Y - oldPosition.Y ;
					diam = 90 * cos(ymovemouse/100);
					height = 90*sin(ymovemouse/100);
					if(diam < 1) diam = 1;
					if(ymovemouse/100 > 3.14/2){
						height = 90;
						ymovemouse = ymovemouse - newPosition.Y + oldPosition.Y ;
					}
					if(ymovemouse/100 < 0){
						height = 0;
						ymovemouse = ymovemouse - newPosition.Y + oldPosition.Y ;
					}
					campos = core::vector3df(diam*sin(xmovemouse/100),diam*cos(xmovemouse/100) ,height );
					camera->setPosition(campos);//+camera->getTarget());
					//camoffset = core::vector3df(camera->getTarget().X*sin(xmovemouse/100), camera->getTarget().Y*diam*cos(xmovemouse/100), height);
					core::vector3df camoffset2 = camoffset;
					camoffset2.rotateXZBy(180*lookside/3.14,campos);
					camera->setTarget(camoffset);
					oldPosition = newPosition;
				}
				else{
					oldPosition = receiver.MouseState.LeftDownPos;
					held = 1;
				}
			}
			else{
				held = 0;
			}
			nearleftup  = camera->getViewFrustum()->getNearLeftUp();
			nearleftdown = camera->getViewFrustum()->getNearLeftDown();
			if(receiver.GetMouseState().RightButtonDown  && current ==-1){
				if(rheld){
					downvect = nearleftdown -nearleftup;
					rnewPosition = receiver.MouseState.Position;
					rxmovemouse = - rnewPosition.X + roldPosition.X ;
					rymovemouse =  + rnewPosition.Y - roldPosition.Y ;
					camoffset =rxmovemouse/10 * (camera->getPosition() - camera->getTarget()).crossProduct(camera->getUpVector()).normalize() - rymovemouse/10 * downvect.normalize();
					camera->setPosition(camera->getAbsolutePosition() + camoffset);
					camera->setTarget(camera->getTarget() + camoffset);
					roldPosition = rnewPosition;
					

					core::vector3df tomid = - camera->getAbsolutePosition().normalize();
					core::vector3df tolook  = (camera->getTarget() - camera->getAbsolutePosition()).normalize();
					lookside = acos(core::vector2df(tomid.X,tomid.Y	).normalize().dotProduct(core::vector2df(tolook.X,tolook.Y).normalize()));
				}
				else{
					roldPosition = receiver.MouseState.RightDownPos;
					rheld = 1;
				}
			}
			else{
				rheld = 0;
			}
			//= camera->getViewFrustum()->getNearLeftUp();
			static core::vector3df nearrightdown;// = camera->getViewFrustum()->getNearLeftDown();
			//= camera->getViewFrustum()->getNearRightUp();
			static core::vector3df nearrightup;//= camera->getViewFrustum()->getNearRightDown();
			static core::position2di clickpos;
			static core::vector3df topvect ;
			

			if(receiver.GetMouseState().LeftButtonDown && current !=-1){
				ray.start = camera->getPosition();
				clickpos  =receiver.MouseState.Position;
				 nearleftup  = camera->getViewFrustum()->getNearLeftUp();
				//camera->getAbsoluteTransformation().transformVect(nearleftup);

				nearleftdown = camera->getViewFrustum()->getNearLeftDown();
				//camera->getAbsoluteTransformation().transformVect(nearleftdown);

				 nearrightup = camera->getViewFrustum()->getNearRightUp();
				//camera->getAbsoluteTransformation().transformVect(nearrightup);

				nearrightdown = camera->getViewFrustum()->getNearRightDown();
				//camera->getAbsoluteTransformation().transformVect(nearrightdown);

				topvect = nearrightup - nearleftup;
				downvect = nearleftdown -nearleftup;
				//camera->getAbsoluteTransformation().transformVect(topvect);
				//camera->getAbsoluteTransformation().transformVect(downvect);
				//float fov = camera->getFOV();
				//core::position2di distfrommid = core::position2di(fov*(clickpos.X -ResX/2)/ResX, fov*(clickpos.Y -ResY/2)/ResY);
				core::vector3df in = (nearleftup + ((float)clickpos.X/(float)ResX)*topvect + ((float)clickpos.Y/(float)ResY)*downvect);
				//camera->getAbsoluteTransformation().transformVect(in);
				core::vector3df  rot = camera->getRotation();
		
				ray.end = ray.start+ (in -ray.start).normalize()*100;
				//ray.end = ray.start + (camera->getTarget() - ray.start).normalize() * 1000.0f;
				selectednode = findselected(ray,allnodes,smallz);
				//bill->setPosition(ray.end);
				if(selectednode != -1){
					setdir(smgr,allnodes.at(selectednode), current);
					//smgr->getMeshManipulator()->setVertexColors(selectednode->getMesh(), video::SColor(123,123,123,123));
					//bill->setPosition(ray.end);
				}
			}
			
#ifdef vis		
			bigcount++;
			if(bigcount % drawat == 0){ //olny render infrequently
				//driver->setViewPort(core::rect<s32>(0,0,ResX,ResY));
				driver->beginScene(true, true, SColor(0,0,0,0));
				driver->setTransform(video::ETS_WORLD,  core::matrix4());
				//driver->setViewPort(core::rect<s32>(0,0,ResX/4,ResY));

				//driver->setViewPort(core::rect<s32>(ResX/4,0,ResX,ResY));
				//driver->draw3DLine(ray.start,ray.end,video::SColor(200,50,210,200));
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
							SMaterial m; 
							m.Lighting=false;
							driver->setMaterial(m);
							driver->setTransform(video::ETS_WORLD,  core::matrix4());
							driver->draw3DLine(allLines.at(i).start,allLines.at(i).end,video::SColor(255,50,210,200));
						}
					}
					else{
						for(int i = 0; i<allLines.size();i++){
							SMaterial m; 
							m.Lighting=false;
							driver->setMaterial(m);
							driver->setTransform(video::ETS_WORLD,  core::matrix4());
							driver->draw3DLine(allLines.at(i).start,allLines.at(i).end,video::SColor(255,50,210,200));
						}
					}
											
					
				}
				else{
					inscount = 0;
					allLines.clear();
				}


								driver->draw2DImage(driver->getTexture("cube/cubeguidepink.png"), core::position2d<s32>(cubex,cubey),
					core::rect<s32>(0,0,312,256), 0,
					video::SColor(pinkalpha,255,255,255), true);

				driver->draw2DImage(driver->getTexture("cube/cubeguideyellow.png"), core::position2d<s32>(cubex,cubey),
					core::rect<s32>(0,0,312,256), 0,
					video::SColor(yellowalpha,255,255,255), true);

				driver->draw2DImage(driver->getTexture("cube/cubeguidered.png"), core::position2d<s32>(cubex,cubey),
					core::rect<s32>(0,0,312,256), 0,
					video::SColor(redalpha,255,255,0), true);

				driver->draw2DImage(driver->getTexture("cube/cubeguideblue.png"), core::position2d<s32>(cubex,cubey),
					core::rect<s32>(0,0,312,256), 0,
					video::SColor(bluealpha,255,255,255), true);

				driver->draw2DImage(driver->getTexture("cube/cubeguidegreen.png"), core::position2d<s32>(cubex,cubey),
					core::rect<s32>(0,0,312,256), 0,
					video::SColor(greenalpha,255,255,255), true);

				driver->draw2DImage(driver->getTexture("cube/cubeguide.png"), core::position2d<s32>(cubex,cubey),
					core::rect<s32>(0,0,312,256), 0,
					video::SColor(blackalpha,255,255,255), true);

				

				smgr->drawAll();

				env->drawAll();
				
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

