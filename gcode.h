#ifndef __GCO_INCLUDED__   // if x.h hasn't been included yet...
#define __GCO_INCLUDED__   //   #define this so the compiler knows it has been included
#include <irrlicht.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <stdint.h>
using namespace irr;

struct instruction {
  int G;
  double X,Y,Z,A,B,E;
};

std::vector<instruction> ReadInGCode(instruction savedVals, char* argv);

void clear(char buffer[20]);



#endif