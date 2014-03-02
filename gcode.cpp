#include "gcode.h"

std::vector<instruction> ReadInGCode(instruction savedVals, char* argv){
	using namespace std;
	char gcode[40] ="stl/";
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
		if(iscom != ';' && iscom!= ' '){
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
					Gused = 1;
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
					currentins.X = returnins.back().X;
				}
				if(Yused ==0){
					currentins.Y = returnins.back().Y;
				}
				if(Zused ==0){
					currentins.Z = returnins.back().Z;
				}
				if(Aused ==0){
					currentins.A = returnins.back().A;
				}
				if(Bused ==0){
					currentins.B = returnins.back().B;
				}
				if(Eused ==0){
					currentins.E = returnins.back().E;
				}
			}
			Gused= 0;
			Xused= 0;
			Yused= 0;
			Zused= 0;
			Aused= 0;
			Bused = 0;
			Eused= 0;
			if(currentins.G == 1){
				returnins.push_back(currentins);
			}
		}
	}
	return returnins;


}

void clear(char buffer[20]){
	memset( buffer, '\0', sizeof(char)*20 );
	return;
}


void slice(char* stl){
	char callslicer[50]  = "slic3r --load stl/config.ini stl/";
	strcat(callslicer,stl);
	strcat(callslicer,".stl");
	system (callslicer);
}
