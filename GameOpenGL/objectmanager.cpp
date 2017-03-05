#include "objectmanager.hpp"
#include "terrain.hpp"
#include "light.hpp"

using namespace GameEngine;
using namespace std;

ObjectManager::ObjectManager()// : camera(0)
{
}
ObjectManager::~ObjectManager()
{

}
void ObjectManager::LoadObjects()
{
	PushCube(); PushCube(); PushCube(); PushCube(); PushCube(); PushCube();
	trees[5].Position() = math::Vector3f(220.0, 250.0, 30.0);
	trees[4].Position() = math::Vector3f(88.0, 70.0, 45.0);
	trees[3].Position() = math::Vector3f(20.0, 50.0, 5.0);
	trees[2].Position() = math::Vector3f(18.0, 70.0, 20.0);
	trees[1].Position() = math::Vector3f(20.0, 20.0, 20.0);
	 trees[0].Position() = math::Vector3f(20.0, 22.0, 5.0);
	NormalLog::Write("Cubes initialized!");
}
void ObjectManager::Initialize(Camera& camera)
{
	static unsigned numCalls = 0;
	if (!numCalls)
	{
		LoadObjects();
		trees[0].LoadTexture("wooden-crate.jpg");
		this->camera = &camera;

		Tree::Instance().InitializeProgram();
		this->camera->BindBuffer(*Tree::GetProgram());

		if (!Terrain::light)
			ErrorLog::Write("Light is not initialized!");
		else
			Terrain::light->BindBuffer(*Tree::GetProgram(), 1);


		stringstream sout;
		sout <<  trees.size() << "  trees loaded!";
		NormalLog::Write(sout);
		numCalls = 1;
	}
}
void ObjectManager::Render()
{
	if ( trees.size())
	{
		// temporary - w tree musisz zrobiæ renderowanie instancyjne!
		glUseProgram(*Tree::GetProgram());

		glActiveTexture(GL_TEXTURE0);
		trees[0].texture.Bind();
		Terrain::light->shadowMap.BindForReading(GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3);

		for (unsigned i = 0; i < trees.size(); ++i)
		{
			trees[i].WorldMatrix()->SetMatrix();
			trees[i].Render();
		}
	}
}
void ObjectManager::RenderForShadow(const unsigned& worldLocation)
{
	// temporary - w tree musisz zrobiæ renderowanie instancyjne!
	for (unsigned i = 0; i < trees.size(); ++i)
	{
		trees[i].WorldMatrix()->SetMatrix(worldLocation);
		trees[i].Render();
	}
}
ObjectManager& ObjectManager::Instance()
{
	static ObjectManager singleton;
	return singleton;
}

void ObjectManager::PushCube()
{
	// initializing cube
	vector<float> vertexData = {
		//  X     Y     Z       U     V          Normal
		// bottom
		 -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		  1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		 -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		 -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		  1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		  1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,

		// top
		 1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		 1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,

		// front
		 -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		  1.0f,-1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 -1.0f,-1.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		  1.0f,-1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,

		// back
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f,-1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		 1.0f, 1.0f,-1.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f,-1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,

		// left
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,

		// right
		 1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		 1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		 1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f
	};
	const float Factor = 5.0;
	for (unsigned i = 0; i < vertexData.size()/8; ++i)
	{ 
		vertexData[i * 8] *= Factor;
		vertexData[i * 8 + 1] *= Factor;
		vertexData[i * 8 + 2] *= Factor;
	}
		

	 trees.push_back(Mesh(Mesh::TypeMesh::TREE));
	 trees[ trees.size()-1].Load(vertexData);
}
