
#ifndef PRINTNODE_INCLUDED__
#define PRINTNODE_INCLUDED__
#include <irrlicht.h>
#include "types.h"
#include <vector>
#include "cubes.h"
#include "collision.h"



using namespace irr;


class Printnode : public scene::ISceneNode
{

	core::aabbox3d<f32> Box;
	video::S3DVertex Vertices[8];
	video::SMaterial Material;

	public:

	Printnode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,core::vector3df Start, core::vector3df End,core::vector3df Up,float Width, float Height)
		: scene::ISceneNode(parent, mgr, id){
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

		Box.reset(Vertices[0].Pos);
		for (s32 i=1; i<8; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
	};

	
	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	virtual void render()
	{
		u16 indices[] = {	3,2,0, 0,1,3, 5,1,0, 0,4,5, 3,1,5, 3,5,7, 5,4,6, 6,7,5, 0,2,4, 2,6,4, 2,3,7, 6,2,7	};
		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList(&Vertices[0], 8, &indices[0], 12, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
	}

	virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

	virtual u32 getMaterialCount() const
	{
		return 1;
	}

	virtual video::SMaterial& Printnode::getMaterial(u32 i)
	{
		return Material;
	}	

	

};

#endif