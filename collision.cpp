#include "collision.h"



int intersect3D_RayTriangle( Tline R, Triangle T, core::vector3df* I )
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

core::vector3df Headoffset(core::vector3df in,scene::ISceneNode* node){
	core::vector3df rotation = node->getAbsoluteTransformation().getRotationDegrees();
	core::vector3df out; 
	out.X = in.X +5;
	out.Y = in.Y -5*cos(core::DEGTORAD*rotation.X);
	out.Z = in.Z - 5*sin(core::DEGTORAD*rotation.X);
	return out;

}