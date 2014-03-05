#ifndef __TESTS_INCLUDED__   // if x.h hasn't been included yet...
#define __TESTS_INCLUDED__   //   #define this so the compiler knows it has been included

#include <vector>
#include <irrlicht.h>
#include "list.h"
#include "triangle.h"
#include "collision.h"
#include "rotation.h"
#include "offsets.h"
#include "gcode.h"
#include "colours.h"

using namespace irr;

enum
{
	GUI_ID_QUIT_BUTTON = 101,
	GUI_ID_NEW_WINDOW_BUTTON,
	GUI_ID_FILE_OPEN_BUTTON,
	GUI_ID_TRANSPARENCY_SCROLL_BAR,
	GUI_ID_SELECT_SECTION,
	GUI_ID_MOVE,
	GUI_ID_YELLOW,
	GUI_ID_PINK,
	GUI_ID_RED,
	GUI_ID_BLUE,
	GUI_ID_GREEN,
	GUI_ID_SLICE

};

//class linenode : public scene::ISceneNode
//{
//
//	core::aabbox3d<f32> Box;
//	video::S3DVertex Vertices[4];
//	video::SMaterial Material;
//	 Tline aline;
//
//public:
//
//	linenode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id, Tline myline )
//		: scene::ISceneNode(parent, mgr, id)
//	{
//		Material.Wireframe = false;
//		Material.Lighting = false;
//		aline = myline ;
//	}
//
//	virtual void OnRegisterSceneNode()
//	{
//		if (IsVisible)
//			SceneManager->registerNodeForRendering(this);
//
//		ISceneNode::OnRegisterSceneNode();
//	}
//
//	virtual void render()
//	{
//		video::IVideoDriver* driver = SceneManager->getVideoDriver();
//		driver->setMaterial(Material);
//		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
//		//driver->drawVertexPrimitiveList(&Vertices[0], 4, &indices[0], 1, video::EVT_STANDARD, scene::EPT_LINES, video::EIT_16BIT);
//		driver->draw3DLine(aline.start,aline.end,video::SColor(255,50,210,200));
//	}
//
//};







bool testTriangleCol();

void addlighting(scene::ISceneManager* smgr, float lightstr = 70);

std::vector<Tline> makegrid();

void doprint(scene::IAnimatedMeshSceneNode* head, std::vector<instruction> instructions,std::vector<Tline> &allLines,scene::ISceneNode* Printed, int &inscount, float CurrentA, float CurrentB);

void setdir(scene::ISceneManager* smgr, dirnode &selectednode,int current);

dirnode makedirnode(scene::IAnimatedMeshSceneNode* node);

#endif