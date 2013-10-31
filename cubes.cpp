#include "cubes.h"


bool inbound(f32 start,f32 end,f32 now){
	f32 diff = end -start;
	if (diff >0){
		if(now <= end && now >= start){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		if(now <= start && now >= end){
			return true;
		}
		else{
			return false;
		}
	}
}

std::vector<core::vector3df> findCubesOfInterest(Tline line){
	std::vector<core::vector3df> cubes;
	core::vector3df tracepoint;
	core::vector3df cubeofinterest;
	core::vector3df direction = line.end - line.start;
	direction.normalize();
	tracepoint = line.start;
	if(direction.getLength() != 0){
		while(inbound(line.start.X,line.end.X,tracepoint.X) && inbound(line.start.Y,line.end.Y,tracepoint.Y) && inbound(line.start.Z,line.end.Z,tracepoint.Z)){
			cubeofinterest.X = abs(floor(tracepoint.X));
			cubeofinterest.Y = abs(floor(tracepoint.Y));
			cubeofinterest.Z = abs(floor(tracepoint.Z));
			if(cubeofinterest.X< 0){
				cubeofinterest.X = 0;
			}
			if(cubeofinterest.Y<0){
				cubeofinterest.Y =0;
			}
			if(cubeofinterest.Z<0){
				cubeofinterest.Z =0;
			}
			if(cubeofinterest.X>= numbox){
				cubeofinterest.X = numbox -1;
			}
			if(cubeofinterest.Y>= numbox){
				cubeofinterest.Y =numbox-1;
			}
			if(cubeofinterest.Z>= numbox){
				cubeofinterest.Z =numbox-1;
			}
			tracepoint  = tracepoint + 0.5*direction*Boxsize;
			if(!cubes.empty()){
				if(!cubes.back().equals(cubeofinterest)){
					cubes.push_back(cubeofinterest);
				}
			}
			else{
				cubes.push_back(cubeofinterest);
			}
		}
	}
	return cubes;

}