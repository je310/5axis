#include "gcode.h"

std::vector<instruction> ReadInGCode(instruction savedVals, char* argv){
	using namespace std;
	char gcode[40] ="";
	strcat(gcode,argv);
	strcat(gcode,".gcode");
	ifstream infile(gcode);
	string line;
	string unit;
	char buffer[20];
	//int G;
	//double X,Y,Z,A,B;
	bool Gused = 0;
	bool Xused = 0;
	bool Yused = 0;
	bool Zused = 0;
	bool Aused = 0;
	bool Bused = 0;
	bool Eused = 0;
	vector<instruction> returnins;
	instruction currentins;
	
	if(!infile.is_open()){
		cout<<"oh no, the file cannot open"<< endl;
	}

	while(getline(infile, line)){
		istringstream record(line);
		char iscom = record.peek();
		if(iscom != ';' && iscom!= -1){
			while (record >> unit) {
				clear(buffer);
				//cout << "* " <<unit << endl;
				char first  = unit.front();
				switch(first){
				case 'G':
					//cout << "g command"<<endl;
					
					unit.copy(buffer,2,1);
					currentins.G = atoi(buffer);
					savedVals.G = currentins.G;
					currentins.M = -10000;
					Gused = 1;
					break;
				case 'M':
					//cout << "g command"<<endl;
					
					unit.copy(buffer,3,1);
					currentins.M = atoi(buffer);
					currentins.G = -10000;

					break;
				case 'S':
					//cout << "g command"<<endl;
					
					unit.copy(buffer,3,1);
					currentins.S = atoi(buffer);
					break;
				case 'F':
					//cout << "g command"<<endl;
					
					unit.copy(buffer,15,1);
					currentins.F = atoi(buffer);
					break;
				case 'E':
					//cout << "e command"<<endl;
					if(unit.find_first_of('.')<unit.npos){
						unit.copy(buffer,15,1);
						currentins.E = atof(buffer);
					}
					else{
						unit.append(".00");
						unit.copy(buffer,15,1);
						currentins.E = atof(buffer);
					}
					savedVals.E = currentins.E;
					Eused = 1;
					break;
				case 'X':
					//cout << "x command"<<endl;
					if(unit.find_first_of('.')<unit.npos){
						unit.copy(buffer,15,1);
						currentins.X = atof(buffer);
					}
					else{
						unit.append(".00");
						unit.copy(buffer,15,1);
						currentins.X = atof(buffer);
						if(currentins.X<1){
							Xused = 1;
						}
					}
					savedVals.X = currentins.X;
					Xused = 1;
					break;

				case 'Y':
					//cout << "y command"<<endl;
					
					if(unit.find_first_of('.')<unit.npos){
						unit.copy(buffer,15,1);
						currentins.Y = atof(buffer);
					}
					else{
						unit.append(".00");
						unit.copy(buffer,7,1);
						currentins.Y = atof(buffer);
					}
					if(currentins.Y > 400){
						Yused = 1;
					}
					savedVals.Y = currentins.Y;
					Yused = 1;
					break;

				case 'Z':
					//cout << "z command"<<endl;
					if(unit.find_first_of('.')<unit.npos){
						unit.copy(buffer,15,1);
						currentins.Z = atof(buffer);
					}
					else{
						unit.append(".00");
						unit.copy(buffer,15,1);
						currentins.Z = atof(buffer);
					}
					savedVals.Z = currentins.Z;
					Zused = 1;
					break;

				case 'A':
					if(unit.find_first_of('.')<unit.npos){
						unit.copy(buffer,unit.size()-1,1);
						currentins.A = atof(buffer);
					}
					else{
						unit.append(".00");
						unit.copy(buffer,15,1);
						currentins.A = atof(buffer);
					}
					Aused = 1;
					break;

				case 'B':
					if(unit.find_first_of('.')<unit.npos){
						unit.copy(buffer,15,1);
						currentins.B = atof(buffer);
					}
					else{
						unit.append(".00");
						unit.copy(buffer,15,1);
						currentins.B = atof(buffer);
					}
					Bused = 1;
					break;

				} //TODO: I have made a instruction datatype. I should make an vector of these to pass back 
			
			}
			if(!returnins.empty()){
				if(Gused ==0){
					currentins.G = 0;
				}
				if(Xused ==0){
					currentins.X = savedVals.X;
				}
				if(Yused ==0){
					currentins.Y = savedVals.Y;
				}
				if(Zused ==0){
					currentins.Z = savedVals.Z;
				}
				if(Aused ==0){
					currentins.A = returnins.back().A;
				}
				if(Bused ==0){
					currentins.B = returnins.back().B;
				}
				if(Eused ==0){
					currentins.E = savedVals.E;
				}
			}
			Gused= 0;
			Xused= 0;
			Yused= 0;
			Zused= 0;
			Aused= 0;
			Bused = 0;
			Eused= 0;
			currentins.T = -10000;
			returnins.push_back(currentins);
			
		}
	}
	return returnins;


}

void clear(char buffer[20]){
	memset( buffer, '\0', sizeof(char)*20 );
	return;
}


void slice(char* stl){
	char callslicer[50]  = "slic3r --load stl/config.ini ";
	strcat(callslicer,stl);
	strcat(callslicer,".stl");
	system (callslicer);
}

void clearins (instruction &thisins){
	thisins.A = -10000;		//-10000 is an indication that the feild is not used. 
	thisins.B = -10000;
	thisins.E = -10000;
	thisins.G = -10000;
	thisins.T = -10000;
	thisins.X = -10000;
	thisins.Y = -10000;
	thisins.Z = -10000;
}

// t2 is A axis, rotate yellow to blue. t1 is the rotation about red
void addRotGcode(std::vector<instruction> &instructions,  int direction, int currentdirection){
	float t2div  = 1/0.044223327;
	float t1div = 1/0.013266;
	float t1direction;
	float t2direction;
	instruction thisins;
	clearins(thisins);
	switch(currentdirection){
	case GUI_ID_YELLOW:
		t1direction = 0;
		t2direction = -90;
		break;
	case GUI_ID_PINK:
		t1direction = -90;
		t2direction = 90;	
		break;
	case GUI_ID_RED:
		t1direction = 0;
		t2direction = 0;
		break;
	case GUI_ID_BLUE:
		t1direction = 0;
		t2direction = 90;	
		break;
	case GUI_ID_GREEN:
		t1direction = -90;
		t2direction = -90;	
		break;
	}
	switch(direction){
	case GUI_ID_YELLOW:
		thisins.T =2;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E = (-90-t2direction)/t2div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 1;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E = (0-t1direction)/t1div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 0;
		instructions.push_back(thisins);			
		break;
	case GUI_ID_PINK:
		thisins.T = 2;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E = (90-t2direction)/t2div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 1;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E = (-90-t1direction)/t1div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 0;
		instructions.push_back(thisins);	
		break;
	case GUI_ID_RED:
		thisins.T = 2;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E = (0-t2direction)/t2div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 1;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E = (0-t1direction)/t1div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 0;
		instructions.push_back(thisins);
		break;
	case GUI_ID_BLUE:
		thisins.T = 2;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E = (90-t2direction)/t2div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 1;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E =(0-t1direction)/t1div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 0;
		instructions.push_back(thisins);		
		break;
	case GUI_ID_GREEN:
		thisins.T = 2;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E = (-90-t2direction)/t2div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 1;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.G = 1;
		thisins.E = (-90-t1direction)/t1div;
		instructions.push_back(thisins);
		clearins(thisins);
		thisins.T = 0;
		instructions.push_back(thisins);		
		break;
	}
}

void OutputGcode(std::vector<instruction> instructions){
	std::ofstream myfile;
	myfile.open ("5axisGcode.gcode");
	myfile<<"T1 \n G92 E0 \n T2 \n G92 E0 \n T0\n";
	for(int i = 0; i< instructions.size(); i++){
		if(instructions.at(i).G != -10000){
			if(instructions.at(i).G == 28 || instructions.at(i).G == 90 || instructions.at(i).G == 21){
				myfile <<'G'<<instructions.at(i).G<<std::endl;
			}
			if(instructions.at(i).G == 1){
				if(instructions.at(i).X != -10000){
					myfile <<'G'<<instructions.at(i).G<<' '<<'X'<<instructions.at(i).X<<' '<<'Y'<<instructions.at(i).Y<<' '<<'Z'<<instructions.at(i).Z<<' '<<'F'<<instructions.at(i).F<<' '<<'E'<<instructions.at(i).E<<std::endl;
				}
				else{
					myfile <<'G'<<instructions.at(i).G<<' '<<'F'<<"100"<<' '<<'E'<<instructions.at(i).E<<std::endl;
				}
			}
			if(instructions.at(i).G == 92){
				myfile <<'G'<<instructions.at(i).G<<' '<<"E0"<<std::endl;
			}
		}
		if(instructions.at(i).M != -10000){
			if(instructions.at(i).M == 82 ||instructions.at(i).M == 84){
				myfile <<'M'<<instructions.at(i).M<<std::endl;
			}
			if(instructions.at(i).M == 104 || instructions.at(i).M == 109){
				myfile <<'M'<<instructions.at(i).M<<' '<<"S"<<instructions.at(i).S<<std::endl;
			}
		}
		if(instructions.at(i).T != -10000){
			myfile <<'T'<<instructions.at(i).T<<std::endl;
		}


	}
	myfile.close();
}