#ifndef GCODE_INCLUDED__
#define GCODE_INCLUDED__

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "types.h"



void clear(char buffer[20]);
std::vector<instruction> ReadInGCode(instruction savedVals);

#endif