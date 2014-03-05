#include "collision.h"

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

#define SMALL_NUM   0.00000001 // anything that avoids division overflow
// dot product (3D) which allows vector operations in arguments
 


// intersect3D_RayTriangle(): find the 3D intersection of a ray with a triangle
//    Input:  a ray R, and a triangle T
//    Output: *I = intersection point (when it exists)
//    Return: -1 = triangle is degenerate (a segment or point)
//             0 =  disjoint (no intersect)
//             1 =  intersect in unique point I1
//             2 =  are in the same plane
int
intersect3D_RayTriangle( Tline R, Triangle T, core::vector3df* I )
{
    core::vector3df    u, v, n;              // triangle vectors
    core::vector3df    dir, w0, w;           // ray vectors
    float     r, a, b;              // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = T.V1 - T.V0;
    v = T.V2 - T.V0;
	n = u.crossProduct(v);              // cross product
    if (n == (core::vector3df(0,0,0)))             // triangle is degenerate
        return -1;                  // do not deal with this case

    dir = R.end - R.start;              // ray direction vector
    w0 = R.start - T.V0;
	a = -n.dotProduct(w0);
	b = n.dotProduct(dir);
    if (fabs(b) < SMALL_NUM) {     // ray is  parallel to triangle plane
        if (a == 0)                 // ray lies in triangle plane
            return 2;
        else return 0;              // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0 || r >1)                    // ray goes away from triangle
        return 0;                   // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect

    *I = R.start + r * dir;            // intersect point of ray and plane

    // is I inside T?
    float    uu, uv, vv, wu, wv, D;
	uu = u.dotProduct(u);
	uv = u.dotProduct(v);
	vv = v.dotProduct(v);
    w = *I - T.V0;
	wu = w.dotProduct(u);
	wv = w.dotProduct(v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)         // I is outside T
        return 0;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return 0;

    return 1;                       // I is in T
}

Tline validateLine(Tline myline){
	if(myline.start.X < 0 ){
		myline.start.X = 0;
	}
	if(myline.start.Y < 0 ){
		myline.start.Y = 0;
	}

	if(myline.start.Z < 0 ){
		myline.start.Z = 0;
	}
	if(myline.end.X < 0 ){
		myline.end.X = 0;
	}
	if(myline.end.Y < 0 ){
		myline.end.Y = 0;
	}
	if(myline.end.Z < 0 ){
		myline.end.Z = 0;
	}

	if(myline.start.X > BuildSize ){
		myline.start.X = BuildSize-1;
	}
	if(myline.start.Y > BuildSize ){
		myline.start.Y = BuildSize-1;
	}

	if(myline.start.Z > BuildSize ){
		myline.start.Z = BuildSize-1;
	}
	if(myline.end.X > BuildSize ){
		myline.end.X = BuildSize-1;
	}
	if(myline.end.Y > BuildSize ){
		myline.end.Y = BuildSize-1;
	}
	if(myline.end.Z > BuildSize ){
		myline.end.Z = BuildSize-1;
	}

	return myline;
}

bool checkcollision(std::vector<core::vector3df> mycubes, Tline myline){
	int size = mycubes.size();
	Triangle mytriangle;
	int collres = 0;
	core::vector3df* I = new core::vector3df;
	for(int i = 0; i<size; i++){
		TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].ptr = TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].head;
		while(TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].ptr != TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].tail){
			mytriangle = TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].ptr->triangle;
			myline.start = myline.start + 0.0008;
			myline.end = myline.end + 0.0008;
			collres = intersect3D_RayTriangle(validateLine(myline),mytriangle,I);
			if(collres == 1 ){
				return true;
			}
			TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].ptr =TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].ptr->next;
		}
	}
	return false;

}

float linemag(Tline ray){
	float xdif = ray.end.X - ray.start.X;
	float ydif = ray.end.Y - ray.start.Y;
	float zdif = ray.end.Z - ray.start.Z;
	float squsum = xdif*xdif + ydif*ydif + zdif*zdif;
	return sqrt(squsum);
}


dirnode findselected(Tline &ray, std::vector<dirnode> allnodes,float smallz){

	int numberOfNodes = allnodes.size();
	dirnode selectednode;
	scene::IMeshBuffer *mesh; 
	Triangle currenttriang;
	core::vector3df Intersection; 
	int hasInt = 0;
	float closest = 100000;

	for(int i = 0; i < numberOfNodes; i++){
		mesh = allnodes.at(i).node->getMesh()->getMeshBuffer(0);
		u32 indexcount = mesh->getIndexCount();
		u16 *indices =  mesh->getIndices();
		u32 vertexcount = mesh->getVertexCount();
		video::S3DVertex *vertices = (video::S3DVertex *)mesh->getVertices();
		for(int j = 0 ; j <indexcount; j=j+3){
			currenttriang.V0 = vertices[indices[j]].Pos;
			currenttriang.V1 = vertices[indices[j+1]].Pos;
			currenttriang.V2 = vertices[indices[j+2]].Pos;
			addoffset(currenttriang, smallz);
			hasInt = intersect3D_RayTriangle(ray,currenttriang,& Intersection);
			if (hasInt ==1){
				ray.end = Intersection;
				if(linemag(ray)<closest){
					closest = linemag(ray);
					selectednode = allnodes.at(i); 
				}
			}
		}
	}
	if(closest < 100000){
		return selectednode;
	}
	selectednode.direction = 0;
	return selectednode;
}
