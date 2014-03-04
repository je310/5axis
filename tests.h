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
	GUI_ID_GREEN

};


bool testTriangleCol();

void addlighting(scene::ISceneManager* smgr, float lightstr = 70);

std::vector<Tline> makegrid();

void doprint(scene::IAnimatedMeshSceneNode* head, std::vector<instruction> instructions,std::vector<Tline> &allLines, int &inscount, float CurrentA, float CurrentB);

void setdir(scene::ISceneManager* smgr, scene::IAnimatedMeshSceneNode* selectednode,int current);
#endif