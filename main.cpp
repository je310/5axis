
#include <irrlicht.h>
#include "driverChoice.h"
//#include "octree.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <triangle3d.h>
#include <omp.h>
#include "stl.cpp"
#include "glm/glm.hpp"

//#define USESILLYCHECK
#define vis
#define coll

#define BuildSize 150
#define Boxsize		1
#define drawat		150
#define numbox  BuildSize/Boxsize

//#define MULTI
using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
#include <IMesh.h>
#include <IColladaMeshWriter.h>
#include <IMeshWriter.h>
#include <EMeshWriterEnums.h>

struct Tline{
	core::vector3df start;
	core::vector3df end;
};

std::vector<core::vector3df> findCubesOfInterest(Tline myline);


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

struct Triangle{
	core::vector3df V0;
	core::vector3df V1;
	core::vector3df V2;
};
class list
{
public:
	struct node {
		Triangle triangle;
		//core::triangle3d<f32>* PrevTriangle;
		struct node *next;
	} *head, *tail, *ptr;	

	list():head(NULL),tail(NULL){}	// constructor	
	~list();			// destructor

	struct list::node* initNode(Triangle);
	void addNode( struct list::node*);
	void insertNode( struct list::node*);
	void deleteNode( struct list::node*);
	void deleteList( struct list::node*);

};

list TriangArr[numbox][numbox][numbox];



class Printnode : public scene::ISceneNode
{

	/*
	First, we declare some member variables:
	The bounding box, 4 vertices, and the material of the tetraeder.
	*/
	core::aabbox3d<f32> Box;
	video::S3DVertex Vertices[8];
	video::SMaterial Material;

	/*
	The parameters of the constructor specify the parent of the scene node,
	a pointer to the scene manager, and an id of the scene node.
	In the constructor we call the parent class' constructor,
	set some properties of the material, and
	create the 4 vertices of the tetraeder we will draw later.
	*/

public:

	Printnode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,core::vector3df Start, core::vector3df End,core::vector3df Up,float Width, float Height)
		: scene::ISceneNode(parent, mgr, id)
	{


		core::vector3df length = End - Start;
		core::vector3df WidthVect = Up.crossProduct(length);
		WidthVect = (WidthVect.normalize()*Width)/2;
		std::vector<core::vector3df> mycubes;
		Tline myline;
		myline.end = End;
		myline.start = Start;
		mycubes = findCubesOfInterest(myline);
		int numofcubes = mycubes.size();

		core::vector3df PrintHeight = Up.normalize()*Height;

		core::vector3df Corn0 = Start - WidthVect;
		core::vector3df Corn1 = Start + WidthVect;
		core::vector3df Corn2 = Start - WidthVect - PrintHeight;
		core::vector3df Corn3 = Start + WidthVect - PrintHeight;
		core::vector3df Corn4 = End - WidthVect;
		core::vector3df Corn5 = End + WidthVect;
		core::vector3df Corn6 = End - WidthVect - PrintHeight;
		core::vector3df Corn7 = End + WidthVect - PrintHeight;
		Triangle triangle[12];

		triangle[0].V0 = Corn3;
		triangle[0].V1 = Corn2;
		triangle[0].V2 = Corn0;

		triangle[1].V0 = Corn0;
		triangle[1].V1 = Corn1;
		triangle[1].V2 = Corn3;

		triangle[2].V0 = Corn5;
		triangle[2].V1 = Corn1;
		triangle[2].V2 = Corn0;

		triangle[3].V0 = Corn0;
		triangle[3].V1 = Corn4;
		triangle[3].V2 = Corn5;

		triangle[4].V0 = Corn3;
		triangle[4].V1 = Corn1;
		triangle[4].V2 = Corn5;

		triangle[5].V0 = Corn3;
		triangle[5].V1 = Corn5;
		triangle[5].V2 = Corn7;

		triangle[6].V0 = Corn5;
		triangle[6].V1 = Corn4;
		triangle[6].V2 = Corn6;

		triangle[7].V0 = Corn6;
		triangle[7].V1 = Corn7;
		triangle[7].V2 = Corn5;

		triangle[8].V0 = Corn0;
		triangle[8].V1 = Corn2;
		triangle[8].V2 = Corn4;

		triangle[9].V0 = Corn2;
		triangle[9].V1 = Corn6;
		triangle[9].V2 = Corn4;

		triangle[10].V0 = Corn2;
		triangle[10].V1 = Corn3;
		triangle[10].V2 = Corn7;

		triangle[0].V0 = Corn6;
		triangle[0].V1 = Corn2;
		triangle[0].V2 = Corn7;


		for(int i = 0; i < numofcubes ; i++){
			for(int j = 0; j < 12; j++){
				TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].addNode(TriangArr[int(mycubes.at(i).X)][int(mycubes.at(i).Y)][int(mycubes.at(i).Z)].initNode(triangle[j]));
			}

		}
		//3,2,0, 0,1,3, 5,1,0, 0,4,5, 3,1,5, 3,5,7, 5,4,6, 6,7,5, 0,2,4, 2,6,4, 2,3,7, 6,2,7

		Material.Wireframe = false;
		Material.Lighting = false;
		video::SColor colour(128,126,89,200);
		float widthover2 = 0.25;
		float height = 0.2;
		Vertices[0] = video::S3DVertex(Corn0.X,Corn0.Y,Corn0.Z, 1,1,0,colour, 0, 1);
		Vertices[1] = video::S3DVertex(Corn1.X,Corn1.Y,Corn1.Z, 1,0,0,colour, 1, 1);
		Vertices[2] = video::S3DVertex(Corn2.X,Corn2.Y,Corn2.Z, 0,1,1,colour, 1, 0);
		Vertices[3] = video::S3DVertex(Corn3.X,Corn3.Y,Corn3.Z, 0,0,1,colour, 0, 0);
		Vertices[4] = video::S3DVertex(Corn4.X,Corn4.Y,Corn4.Z, 1,1,0,colour, 0, 1);
		Vertices[5] = video::S3DVertex(Corn5.X,Corn5.Y,Corn5.Z, 1,0,0,colour, 1, 1);
		Vertices[6] = video::S3DVertex(Corn6.X,Corn6.Y,Corn6.Z, 0,1,1,colour, 1, 0);
		Vertices[7] = video::S3DVertex(Corn7.X,Corn7.Y,Corn7.Z, 0,0,1,colour, 0, 0);

	/*
	The Irrlicht Engine needs to know the bounding box of a scene node.
	It will use it for automatic culling and other things. Hence, we
	need to create a bounding box from the 4 vertices we use.
	If you do not want the engine to use the box for automatic culling,
	and/or don't want to create the box, you could also call
	irr::scene::ISceneNode::setAutomaticCulling() with irr::scene::EAC_OFF.
	*/
		Box.reset(Vertices[0].Pos);
		for (s32 i=1; i<8; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
	}

	/*
	Before it is drawn, the irr::scene::ISceneNode::OnRegisterSceneNode()
	method of every scene node in the scene is called by the scene manager.
	If the scene node wishes to draw itself, it may register itself in the
	scene manager to be drawn. This is necessary to tell the scene manager
	when it should call irr::scene::ISceneNode::render(). For
	example, normal scene nodes render their content one after another,
	while stencil buffer shadows would like to be drawn after all other
	scene nodes. And camera or light scene nodes need to be rendered before
	all other scene nodes (if at all). So here we simply register the
	scene node to render normally. If we would like to let it be rendered
	like cameras or light, we would have to call
	SceneManager->registerNodeForRendering(this, SNRT_LIGHT_AND_CAMERA);
	After this, we call the actual
	irr::scene::ISceneNode::OnRegisterSceneNode() method of the base class,
	which simply lets also all the child scene nodes of this node register
	themselves.
	*/
	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	/*
	In the render() method most of the interesting stuff happens: The
	Scene node renders itself. We override this method and draw the
	tetraeder.
	*/
	virtual void render()
	{
		u16 indices[] = {	3,2,0, 0,1,3, 5,1,0, 0,4,5, 3,1,5, 3,5,7, 5,4,6, 6,7,5, 0,2,4, 2,6,4, 2,3,7, 6,2,7	};
		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList(&Vertices[0], 8, &indices[0], 12, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
	}

	/*
	And finally we create three small additional methods.
	irr::scene::ISceneNode::getBoundingBox() returns the bounding box of
	this scene node, irr::scene::ISceneNode::getMaterialCount() returns the
	amount of materials in this scene node (our tetraeder only has one
	material), and irr::scene::ISceneNode::getMaterial() returns the
	material at an index. Because we have only one material here, we can
	return the only one material, assuming that no one ever calls
	getMaterial() with an index greater than 0.
	*/
	virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

	virtual u32 getMaterialCount() const
	{
		return 1;
	}

	virtual video::SMaterial& getMaterial(u32 i)
	{
		return Material;
	}	
};



class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

struct instruction {
  int G;
  double X,Y,Z,A,B,E;
};




list::~list() {
	node *current,*temp;
	current = head;
	temp = head;
	while(current != NULL) {
		current = current->next;
		delete temp;
		temp = current;
	}
}

struct list::node* list::initNode(Triangle triangle) {
	struct node *ptr = new node;
 
	// error? then just return
	if( ptr == NULL )                         
		return static_cast<struct node *>(NULL);  
	// assign it 
	// then return pointer to ne node
	else {  
		ptr->triangle =triangle;                        
		return ptr;                         
	}
}

// adding to the end of list  
void list::addNode( struct node *newNode )  {
	// if there is no node, put it to head
	if( head == NULL ) {
		head = newNode;  
		tail = newNode;
	}
   
	// link in the new_node to the tail of the list
	// then mark the next field as the end of the list
	// adjust tail to point to the last node

	tail->next = newNode;       
	newNode->next = NULL;       
	tail = newNode;               
}


void list::deleteNode( struct list::node *ptr )
{
  struct node *temp, *prev;
   temp = ptr;    // node to be deleted 
   prev = head;   // start of the list, will cycle to node before temp    

   if( temp == prev ) {                    // deleting first node?  
       head = head->next;                  // moves head to next node     
       if( tail == temp )                  // is it end, only one node?   
          tail = tail->next;               // adjust end as well          
       delete temp ;                       // free up space 
   }
   else {                                  // if not the first node, then 
       while( prev->next != temp ) {       // move prev to the node before
           prev = prev->next;              // the one to be deleted       
       }
       prev->next = temp->next;            // link previous node to next  
       if( tail == temp )                  // if this was the end node,   
           tail = prev;                    // then reset the end pointer  
      delete temp;                         // free up space
   }
}

void list::deleteList( struct node *ptr )
{
   struct node *temp;

   if( head == NULL ) return;   	// don't try to delete an empty list      

   if( ptr == head ) {			// if we are deleting the entire list    
       head = NULL;			// then reset head and    
       tail = NULL;			// end to empty                                     
   }
   else {
       temp = head;			// if it's not the entire list, readjust end   
       while( temp->next != ptr )       // locate previous node to ptr   
           temp = temp->next;
       tail = temp;                     // set end to node before ptr   
   }

   while( ptr != NULL ) {		// whilst there are still nodes to delete      
      temp = ptr->next;			// record address of next node                 
      delete ptr;			// free this node                             
      ptr = temp;			// point to next node to be deleted           
   }
}

std::vector<Tline> allLines;
std::vector<instruction> ReadInGCode(instruction savedVals);
void faceTarget(Printnode& hero, scene::IAnimatedMeshSceneNode& target);
void clear(char buffer[20]);
bool notCollided = 1;

bool nextpress =0;
int numberOfInstructions = 0;


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

core::vector3df Headoffset(core::vector3df in,scene::ISceneNode* node){
	core::vector3df rotation = node->getAbsoluteTransformation().getRotationDegrees();
	core::vector3df out; 
	out.X = in.X +5;
	out.Y = in.Y -5*cos(core::DEGTORAD*rotation.X);
	out.Z = in.Z - 5*sin(core::DEGTORAD*rotation.X);
	return out;

}
core::vector3df getClosestPointOnLine
( const core::vector3df& axis,const core::vector3df& pivot,const core::vector3df& point){
    core::vector3df c = point - pivot;
    f32 t = axis.dotProduct(c);
    return pivot + axis*t;
}

//both axis and pivot are in world space
void revolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot){
    node->updateAbsolutePosition();
    core::vector3df p1 = node->getAbsolutePosition();
    core::vector3df p2 = getClosestPointOnLine(axis, pivot, p1);
    core::vector3df vect = p1 - p2;
    
    core::quaternion q;
    q.fromAngleAxis(degs*core::DEGTORAD, axis);
    q.getMatrix().rotateVect(vect);
    
    node->setPosition(p2 + vect);
}

void invrevolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot){
    node->updateAbsolutePosition();
    core::vector3df p1 = node->getAbsolutePosition();
    core::vector3df p2 = getClosestPointOnLine(axis, pivot, p1);
    core::vector3df vect = p1 - p2;
	core::CMatrix4<f32> mat ;
    core::quaternion q;
    q.fromAngleAxis(degs*core::DEGTORAD, axis);
	q.getMatrix().getInverse(mat);
	mat.rotateVect(vect);
    
    node->setPosition(p2 + vect);
}

void rotateNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis)
{
    node->updateAbsolutePosition();
    core::matrix4 m2 = node->getAbsoluteTransformation();
    core::vector3df a = axis;
    m2.rotateVect(a);
    a.normalize();
 
    core::quaternion q;
    q.fromAngleAxis(degs*core::DEGTORAD, a);
    core::matrix4 m1 = q.getMatrix();
 
    core::matrix4 m = m1*m2;
    node->setRotation(m.getRotationDegrees());
}

void invrotateNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis)
{
    node->updateAbsolutePosition();
    core::matrix4 m2 = node->getAbsoluteTransformation();
    core::vector3df a = axis;
    m2.rotateVect(a);
    a.normalize();
 
    core::quaternion q;
    q.fromAngleAxis(degs*core::DEGTORAD, a);
    core::matrix4 m1 = q.getMatrix();
	m1.getInverse(m1);
    core::matrix4 m = m1*m2;
    node->setRotation(m.getRotationDegrees());
}

void moveNodeInLocalSpace(scene::ISceneNode* node, const core::vector3df& distVect)
{
    node->updateAbsolutePosition();
    core::matrix4 m = node->getAbsoluteTransformation();
    core::vector3df d = distVect;
    m.rotateVect(d);
        
    core::vector3df pos = node->getAbsolutePosition() + d;
    node->setPosition(pos);
}

void revolveNodeInLocalSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot)
{
    moveNodeInLocalSpace(node, pivot);
    rotateNodeInLocalSpace(node, degs, axis);
    moveNodeInLocalSpace(node, -pivot);
}

struct v3 { 
    float x, y, z; 
    v3(float _x=0, float _y=0, float _z=0) : x(_x), y(_y), z(_z) {} 
    float dotproduct(const v3 &v) const { return x*v.x + y*v.y + z*v.z; }
    void transform(const glm::mat4 &mat) { glm::vec4 v = glm::vec4(x, y, z, 1.0f), vt; vt = mat*v; x=vt.x; y=vt.y; z=vt.z;}
    v3& operator-=(const v3 &pt) { x-=pt.x; y-=pt.y; z-=pt.z; return *this;    }
    v3 operator-(const v3 &pt) { return v3(x-pt.x, y-pt.y, z-pt.z); }
    v3 operator+(const v3 &pt) { return v3(x+pt.x, y+pt.y, z+pt.z); }
    v3 operator/(float a) { return v3(x/a, y/a, z/a); }
    v3 operator*(float a) { return v3(x*a, y*a, z*a); }
    float normalize() const { return sqrt(x*x+y*y+z*z); } 
};
v3 operator-(const v3 &a, const v3 &b) {return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
v3 operator+(const v3 &a, const v3 &b) {return v3(a.x+b.x, a.y+b.y, a.z+b.z); }

struct LineSegment {
    LineSegment(v3 p0=v3(), v3 p1=v3()) { v[0]=p0; v[1]=p1; }
    v3 v[2];
};
class Plane { 
public:
    Plane() : mDistance(0) {}
    float distance() const { return mDistance; }
    float distanceToPoint(const v3 &vertex) const {    return vertex.dotproduct(mNormal) - mDistance; }
    void setNormal(v3 normal) { mNormal = normal; }
    void setDistance(float distance) { mDistance = distance; }
protected:
    v3        mNormal;    // normalized Normal-Vector of the plane
    float   mDistance;  // shortest distance from plane to Origin
};
struct Triangle2    { 
    Triangle2(v3 n, v3 v0, v3 v1, v3 v2) : normal(n) { v[0]=v0; v[1]=v1; v[2]=v2; } 
    Triangle2& operator-=(const v3 &pt) { v[0]-=pt; v[1]-=pt; v[2]-=pt; return *this;}
    void transform(const glm::mat4 &mat) { v[0].transform(mat); v[1].transform(mat); v[2].transform(mat);}
    // @return -1 = all triangle is on plane back side
    //          0 = plane intersects the triangle
    //          1 = all triangle is on plane front side
    //         -2 = error in function
    int intersectPlane(const Plane &plane, LineSegment &ls) const {
        // a triangle has 3 vertices that construct 3 line segments
        size_t cntFront=0, cntBack=0;
        for (size_t j=0; j<3; ++j) {
            float distance = plane.distanceToPoint(v[j]);
            if (distance<0) ++cntBack;
            else ++cntFront;
        }
        if (3 == cntBack) {
            return -1;
        }
        else if (3 == cntFront) {
            return 1;
        }
        size_t lines[] = {0,1,1,2,2,0}; // CCW Triangle2
        std::vector<v3> intersectPoints;
        for (size_t i=0; i<3; ++i) {
            const v3 &a = v[lines[i*2+0]];
            const v3 &b = v[lines[i*2+1]];
            const float da = plane.distanceToPoint(a);
            const float db = plane.distanceToPoint(b);
            if (da*db<0) {
                const float s = da/(da-db); // intersection factor (between 0 and 1)
                v3 bMinusa = b-a;
                intersectPoints.push_back(a+bMinusa*s);
            }
            else if (0==da) { // plane falls exactly on one of the three Triangle2 vertices
                if (intersectPoints.size()<2)
                    intersectPoints.push_back(a);
            }
            else if (0==db) { // plane falls exactly on one of the three Triangle2 vertices
                if (intersectPoints.size()<2)
                    intersectPoints.push_back(b);
            }
        }
        if (2==intersectPoints.size()) {
            // Output the intersecting line segment object
            ls.v[0]=intersectPoints[0];
            ls.v[1]=intersectPoints[1];
            return 0;
        }
        return -2;        
    }
    v3 v[3], normal; 
};
class Triangle2Mesh { 
public:
    Triangle2Mesh() : bottomLeftVertex(999999,999999,999999), upperRightVertex(-999999,-999999,-999999) {}
    size_t size() const { return mesh.size(); }
    // move 3D Model coordinates to be center around COG(0,0,0)
    void normalize() {
        v3 halfBbox = (upperRightVertex - bottomLeftVertex)/2.0f;
        v3 start = bottomLeftVertex + halfBbox;
        for (size_t i=0; i<mesh.size(); ++i) {
            Triangle2 &triangle = mesh[i];
            triangle -= start;
        }
        bottomLeftVertex = halfBbox*-1.0f;
        upperRightVertex = halfBbox;
    }
    void push_back(const Triangle2 &t) {
        mesh.push_back(t);
        for (size_t i=0; i<3; ++i) {
            if (t.v[i].x < bottomLeftVertex.x) bottomLeftVertex.x = t.v[i].x;
            if (t.v[i].y < bottomLeftVertex.y) bottomLeftVertex.y = t.v[i].y;
            if (t.v[i].z < bottomLeftVertex.z) bottomLeftVertex.z = t.v[i].z;
            if (t.v[i].x > upperRightVertex.x) upperRightVertex.x = t.v[i].x;
            if (t.v[i].y > upperRightVertex.y) upperRightVertex.y = t.v[i].y;
            if (t.v[i].z > upperRightVertex.z) upperRightVertex.z = t.v[i].z;
        }
    }
    v3 meshAABBSize() const {
        return v3(upperRightVertex.x-bottomLeftVertex.x, upperRightVertex.y-bottomLeftVertex.y, upperRightVertex.z-bottomLeftVertex.z);
    }
    std::vector<Triangle2>& getMesh() { return mesh; }
    const std::vector<Triangle2>& getMesh() const { return mesh; }
    v3 getBottomLeftVertex() const { return bottomLeftVertex; }
    v3 getUpperRightVertex() const { return upperRightVertex; }
    // Mesh COG point should be at (0,0,0)
    int transform(const glm::mat4 &mat) {
        for (size_t i=0; i<mesh.size(); ++i) {
            Triangle2 &triangle = mesh[i];
            triangle.transform(mat);
        }
        return 0;
    }
protected:
    std::vector<Triangle2> mesh; 
    v3 bottomLeftVertex, upperRightVertex;
};

// read the given STL file name (ascii or binary is set using ‘isBinaryFormat’)
// and generate a Triangle2 Mesh object in output parameter ‘mesh’
int stlToMeshInMemory(const char *stlFile, Triangle2Mesh *mesh, bool isBinaryFormat) {    
    if (!isBinaryFormat) {
        std::ifstream in(stlFile);
        if (!in.good()) return 1;
        char title[80];
        std::string s0, s1;
        float n0, n1, n2, f0, f1, f2, f3, f4, f5, f6, f7, f8;
        in.read(title, 80);
        while (!in.eof()) {
            in >> s0;                                // facet || endsolid
            if (s0=="facet") {
                in >> s1 >> n0 >> n1 >> n2;            // normal x y z
                in >> s0 >> s1;                        // outer loop
                in >> s0 >> f0 >> f1 >> f2;         // vertex x y z
                in >> s0 >> f3 >> f4 >> f5;         // vertex x y z
                in >> s0 >> f6 >> f7 >> f8;         // vertex x y z
                in >> s0;                            // endloop
                in >> s0;                            // endfacet
                // Generate a new Triangle2 with Normal as 3 Vertices
                Triangle2 t(v3(n0, n1, n2), v3(f0, f1, f2), v3(f3, f4, f5), v3(f6, f7, f8));
                mesh->push_back(t);
            }
            else if (s0=="endsolid") {
                break;
            }
        }
        in.close();
    }
    else {
        FILE *f = fopen(stlFile, "rb");
        if (!f) return 1;
        char title[80];
        int nFaces;
        fread(title, 80, 1, f);
        fread((void*)&nFaces, 4, 1, f);
        float v[12]; // normal=3, vertices=3*3 = 12
        unsigned short uint16; 
        // Every Face is 50 Bytes: Normal(3*float), Vertices(9*float), 2 Bytes Spacer
        for (size_t i=0; i<nFaces; ++i) {
            for (size_t j=0; j<12; ++j) {
                fread((void*)&v[j], sizeof(float), 1, f);
            }
            fread((void*)&uint16, sizeof(unsigned short), 1, f); // spacer between successive faces
            Triangle2 t(v3(v[0], v[1], v[2]), v3(v[3], v[4], v[5]), v3(v[6], v[7], v[8]), v3(v[9], v[10], v[11]));
            mesh->push_back(t);
        }
        fclose(f);
    }    
    mesh->normalize(); 
    return 0;
}
    


int main()
{
	float CurrentA = 0; 
	float CurrentB = 0 ; 
	omp_set_num_threads(4);
	bool hit = 0;
	Triangle Test1;
	core::vector3df* I = new core::vector3df(0,0,0);
	Test1.V0 = core::vector3df(0,0,0);
	Test1.V1 = core::vector3df(1,0,0);
	Test1.V2 = core::vector3df(1,1,0);

	Tline testl1;
	testl1.start = core::vector3df(0.5,0.1,1);
	testl1.end = core::vector3df(0.5,0.1,-1);

	Tline testl2;
	testl2.start = core::vector3df(0,0.5,1);
	testl2.end = core::vector3df(0,0.5,-1);

	hit = intersect3D_RayTriangle(testl1,Test1,I);
	if(hit == true){
		hit =false;
	}
	hit = intersect3D_RayTriangle(testl2,Test1,I);
	if(hit == true){
		hit =false;
	}

	Triangle2Mesh mesh;

	float    sliceSize = 1.0f;
    char    modelFileName[1024] = "test.stl";    
    bool    isBinaryFormat = true;

	if (stlToMeshInMemory(modelFileName, &mesh, isBinaryFormat)!=0)
        return 1;
    fprintf(stderr, "Mesh has %d triangles\n", mesh.size());




	// ask user for driver
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;
	MyEventReceiver receiver;
	IrrlichtDevice *device =
		createDevice(driverType, core::dimension2d<u32>(1920, 1080), 16, false,false,false, &receiver);

	if (device == 0)
		return 1; // could not create selected driver.
	device->getCursorControl()->setVisible(false);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	scene::ICameraSceneNode* camera =
	smgr->addCameraSceneNodeFPS(0, 100.0f, .3f, ID_IsNotPickable, 0, 0, false, 3.f);
	camera->setPosition(core::vector3df(-75,75,40));
	camera->setTarget(core::vector3df(75,75,40));
	//camera->setUpVector(core::vector3df(0,0,1));

	scene::ITriangleSelector* selector = 0;
	smgr->setAmbientLight(video::SColorf(0.3,0.3,0.3,1));
	if (selector)
	{
		scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
			selector, camera, core::vector3df(30,50,30),
			core::vector3df(0,-10,0), core::vector3df(0,30,0));
		selector->drop(); // As soon as we're done with the selector, drop it.
		camera->addAnimator(anim);
		anim->drop();  // And likewise, drop the animator when we're done referring to it.
	}

	instruction savedVals;
	savedVals.A = 0;
	savedVals.B = 0;
	savedVals.X = 0;
	savedVals.Y = 0;
	savedVals.Z = 0;
	savedVals.G = 0;
	savedVals.E = 0;

	std::vector<instruction> instructions;
	instructions = ReadInGCode(savedVals);
	numberOfInstructions = instructions.size();

	driver->setTransform(video::ETS_WORLD,  core::matrix4());
	scene::IAnimatedMeshSceneNode* head = 0;
	head = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/hotendmock.stl"),
						0, ID_IsNotPickable);
	scene::IMeshBuffer *pBuffer;
	pBuffer = head->getMesh()->getMeshBuffer(0);

	head->setMaterialType(video::EMT_SOLID);
	head->setMaterialFlag(video::EMF_LIGHTING, true);
	
	head->setPosition(core::vector3df(5,5,1));
	head->updateAbsolutePosition();

	io::IWriteFile *out = device->getFileSystem()->createAndWriteFile("newone.stl");

	scene::IMeshWriter *writer = 0;
	writer = smgr->createMeshWriter(scene::EMWT_STL);
	writer->writeMesh(out,head->getMesh());


	video::SMaterial material;


	std::vector<video::S3DVertex> vertvect;
	int VertexCount = pBuffer->getVertexCount();
	video::S3DVertex *vertices = (video::S3DVertex *)pBuffer->getVertices();
	video::S3DVertex *trans	= new video::S3DVertex();
	for(int i = 0; i< VertexCount; i++){
		vertvect.push_back(vertices[i]);
	}

	for(int i =0; i <VertexCount; i++){
		for (int j = i+1; j<VertexCount; j++){
			if(vertvect.at(i).Pos.equals(vertvect.at(j).Pos)){
				vertvect.erase(vertvect.begin()+ j);
				j--;
				VertexCount = vertvect.size();
			}
		}
	}

	material.Lighting = true;
	selector = smgr->createTriangleSelector(head);
	head->setTriangleSelector(selector);
	selector->drop();


	scene::IAnimatedMeshSceneNode * obj = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/cube.stl"),0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	if (obj){
		obj->setPosition(core::vector3df(0,0,223));
		obj->setMaterialTexture(0, driver->getTexture("../../media/wall.bmp"));
		obj->setMaterialFlag(video::EMF_LIGHTING, false);
	}
	selector = smgr->createTriangleSelector(obj);
	obj->setTriangleSelector(selector);
	selector->drop();
		
	scene::IBillboardSceneNode * bill;
	scene::IBillboardSceneNode * bill2;
	//for(int i = 0; i< VertexCount  ; i++){
	//	bill = smgr->addBillboardSceneNode(head);
	//	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
	//	bill->setMaterialTexture(0, driver->getTexture("../../media/particle.bmp"));
	//	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	//	bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	//	bill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	//	bill->setID(ID_IsNotPickable); // This ensures that we don't accidentally ray-pick it
	//	bill->setPosition(vertices[i].Pos);
	//}
	//material.setTexture(0, 0);
	//material.Lighting = false;

	// Add a light, so that the unselected nodes aren't completely dark.
	scene::ILightSceneNode * light = smgr->addLightSceneNode(0, core::vector3df(-60,100,400),
		video::SColorf(1.0f,1.0f,1.0f,1.0f), 600.0f);
	light->setID(ID_IsNotPickable); // Make it an invalid target for selection.

	// Remember which scene node is highlighted
	scene::ISceneNode* highlightedSceneNode = 0;
	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
	int lastFPS = -1;

	// draw the selection triangle only as wireframe
	material.Wireframe=true;

	int inscount = 0;

	std::vector<Tline> grid;
	Tline gridline;
	for(int j = 0; j<=15; j++){
		for(int k = 0; k<=15; k++){
			gridline.start = core::vector3df(0,j*10,k*10);
			gridline.end = core::vector3df(150,j*10,k*10);
			grid.push_back(gridline);
		}
	}
	for(int i = 0; i<=15; i++){
		for(int k = 0; k<=15; k++){
			gridline.start = core::vector3df(i*10,0,k*10);
			gridline.end = core::vector3df(i*10,150,k*10);
			grid.push_back(gridline);
		}
	}
	for(int j = 0; j<=15; j++){
		for(int i = 0; i<=15;i++){
			gridline.start = core::vector3df(i*10,j*10,0);
			gridline.end = core::vector3df(i*10,j*10,150);
			grid.push_back(gridline);
		}
	}
	

	scene::ISceneNode * Printed  = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/cube.stl"),0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	//scene::ISceneNode * PrintedLine  = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/cube.stl"),0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	//PrintedLine->setVisible(true);
	Printed->setVisible(false);

	int nextpressR = 0;
	bool nextpressR2 = true;
	std::vector<core::vector3df> mycubes;
	int bigcount = 0;
	bool forwards = 0;
#ifdef vis
	while(device->run())
	if (device->isWindowActive())
	{
		

#else
	while(1)
	if (1)
	{
#endif
		
		
		head->updateAbsolutePosition();

		core::vector3df intersection;
		
		core::triangle3df hitTriangle;
		scene::ISceneNode * selectedSceneNode;
		core::line3d<f32> ray;
		core::vector3df  nextmove=core::vector3df (instructions[inscount].X,instructions[inscount].Y,instructions[inscount].Z);
		core::vector3df  prevmove;
		if(inscount>0){
			prevmove=core::vector3df (instructions[inscount-1].X,instructions[inscount-1].Y,instructions[inscount-1].Z);
		}
		else{
			prevmove=core::vector3df (0,0,0);
		}
		
		Tline vertline;
#ifdef coll
#ifdef MULTI
		#pragma omp  for
//		{
#endif
		for(int i = 0; i <VertexCount; i++){
				std::vector<core::vector3df> mycubes2;
				Tline vertline2;
				vertline2.start = vertices[i].Pos +prevmove;
				vertline2.end = vertices[i].Pos + nextmove;
				mycubes2 = findCubesOfInterest(vertline2);
				vertline2 = validateLine(vertline2);
				if(checkcollision(mycubes2,vertline2)){
					//std::cout << "impact";
				}

		}
#ifdef MULTI	
	//	}

#endif
#endif

		
#ifdef USESILLYCHECK
		for(int i =0 ; i < VertexCount; i++){
			ray.start =  vertices[i].Pos;
			ray.end = ray.start + nextmove;
			selectedSceneNode =
			collMan->getSceneNodeAndCollisionPointFromRay(
					ray,
					intersection, // This will be the position of the collision
					hitTriangle, // This will be the triangle hit in the collision
					IDFlag_IsPickable, // This ensures that only nodes that we have
							// set up to be pickable are considered
					Printed); // Check the entire scene (this is actually the implicit default)
			if(selectedSceneNode && notCollided)
				{
				std::cout << "we have a collision\n";
				notCollided = 0;
				bill = smgr->addBillboardSceneNode();
				bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
				bill->setMaterialTexture(0, driver->getTexture("../../media/particle.bmp"));
				bill->setMaterialFlag(video::EMF_LIGHTING, false);
				bill->setMaterialFlag(video::EMF_ZBUFFER, false);
				bill->setSize(core::dimension2d<f32>(40.0f, 40.0f));
				bill->setID(ID_IsNotPickable); // This ensures that we don't accidentally ray-pick it
				bill->setPosition(intersection);
				//bill->updateAbsolutePosition();

				bill2 = smgr->addBillboardSceneNode(head);
				bill2->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
				bill2->setMaterialTexture(0, driver->getTexture("../../media/particle.bmp"));
				bill2->setMaterialFlag(video::EMF_LIGHTING, false);
				bill2->setMaterialFlag(video::EMF_ZBUFFER, false);
				bill2->setSize(core::dimension2d<f32>(20.0f, 20.0f));
				bill2->setID(ID_IsNotPickable);
				bill2->setPosition(vertices[i].Pos);
				bill2->updateAbsolutePosition();
				if((selectedSceneNode->getID() & IDFlag_IsHighlightable) == IDFlag_IsHighlightable)
				{
					highlightedSceneNode = selectedSceneNode;

					// Highlighting in this case means turning lighting OFF for this node,
					// which means that it will be drawn with full brightness.
					highlightedSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);
				}
			}
		}

		if(selectedSceneNode)
		{
			bill->setPosition(intersection);

			// We need to reset the transform before doing our own rendering.
			driver->setTransform(video::ETS_WORLD, core::matrix4());
			driver->setMaterial(material);
			driver->draw3DTriangle(hitTriangle, video::SColor(0,255,0,0));

			// We can check the flags for the scene node that was hit to see if it should be
			// highlighted. The animated nodes can be highlighted, but not the Quake level mesh
			if((selectedSceneNode->getID() & IDFlag_IsHighlightable) == IDFlag_IsHighlightable)
			{
				highlightedSceneNode = selectedSceneNode;

				// Highlighting in this case means turning lighting OFF for this node,
				// which means that it will be drawn with full brightness.
				highlightedSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);
			}
		}
#endif

		
	
		 nextpress = 1;
		// We're all done drawing, so end the scene.
		Tline myLine;
		if(!receiver.IsKeyDown(irr::KEY_KEY_E) && nextpress == 1){
			if(inscount < numberOfInstructions - 1){

				if(instructions.at(inscount).A != CurrentA){
					revolveNodeInWorldSpace(head,instructions.at(inscount).A-CurrentA,core::vector3df(1,0,0),core::vector3df(BuildSize/2,BuildSize/2,40));
					CurrentA = instructions.at(inscount).A;
					//camera->setUpVector(core::vector3df(sin(core::DEGTORAD*CurrentA),0,cos(core::DEGTORAD*CurrentA)));
				}
				if(instructions.at(inscount).B != CurrentB){
					revolveNodeInWorldSpace(head,instructions.at(inscount).B-CurrentB,core::vector3df(0,1,0),core::vector3df(BuildSize/2,BuildSize/2,40));
					CurrentA = instructions.at(inscount).B;
				}
				//inscount++;
				if(instructions.at(inscount).G ==1  && inscount < numberOfInstructions - 2){
					head->setPosition(Headoffset(nextmove,head));
					head->updateAbsolutePosition();
					//core::vector3df  nextnextmove;
					//nextnextmove = core::vector3df (instructions.at(inscount).X,instructions.at(inscount).Y,instructions.at(inscount).Z);
					if(inscount ==0 ){
						myLine.end= nextmove;
						myLine.start = prevmove;
						allLines.push_back(myLine);
						Printnode *print =new Printnode(Printed, smgr,IDFlag_IsPickable ,prevmove,nextmove,core::vector3df(0,0,1),0.5,0.2);
					}
					else if((instructions.at(inscount).E - instructions.at(inscount-1).E) >0){
						myLine.end= nextmove;
						myLine.start = prevmove;
						allLines.push_back(myLine);
						Printnode *print =new Printnode(Printed, smgr,IDFlag_IsPickable ,prevmove,nextmove,core::vector3df(0,0,1),0.5,0.2);
					
					//if(instructions.at(inscount).Z>25 || instructions.at(inscount).Y>150 || instructions.at(inscount).Z<-1 ||instructions.at(inscount).Y<0||instructions.at(inscount).X>140){
					//	inscount =inscount;
					//}
					}
				}
				inscount++;
			}
			else{
				//if(bigcount % drawat == 
				
				//std::cout << "done";
			}
			nextpress = 0;
			
		}
#ifdef vis		
		bigcount++;
		if(bigcount % drawat == 0){
		
		//revolveNodeInLocalSpace(head,3,core::vector3df(0,1,0),core::vector3df(-5,5,0));
		driver->beginScene(true, true, 0);
		if(receiver.IsKeyDown(irr::KEY_KEY_R) && nextpressR ==0){
			nextpressR = 1;
		}
		if(!receiver.IsKeyDown(irr::KEY_KEY_R) && nextpressR ==1){
			nextpressR2 = !nextpressR2;
			nextpressR =0;
		}
		
		if(!receiver.IsKeyDown(irr::KEY_KEY_Q)){
			if(nextpressR2){

			for(int i = 0; i<grid.size();i++){
				driver->setTransform(video::ETS_WORLD,  core::matrix4());
				driver->draw3DLine(grid.at(i).start,grid.at(i).end,video::SColor(20,50,210,100));
			}

			for(int i = allLines.size()-1000; i<allLines.size();i++){
				driver->setTransform(video::ETS_WORLD,  core::matrix4());
				driver->draw3DLine(allLines.at(i).start,allLines.at(i).end,video::SColor(200,50,210,200));
			}
		}
		else{
			for(int i = 0; i<allLines.size();i++){
				driver->setTransform(video::ETS_WORLD,  core::matrix4());
				driver->draw3DLine(allLines.at(i).start,allLines.at(i).end,video::SColor(200,50,210,200));
			}
		}
			Printed->setVisible(true);
			Printed->setVisible(false);
			smgr->drawAll();
		}
		else{
			Printed->setVisible(false);
		}
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


std::vector<instruction> ReadInGCode(instruction savedVals){
	using namespace std;
	ifstream infile("IN.gcode");
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



/*
**/

