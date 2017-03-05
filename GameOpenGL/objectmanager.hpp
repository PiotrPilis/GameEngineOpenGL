#ifndef _OBJECTMANAGER_HPP_
#define _OBJECTMANAGER_HPP_

#include "log.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "tree.hpp"
#include "camera.hpp"
#include <vector>

namespace GameEngine
{
	class ObjectManager
	{
	private:
		ObjectManager();
		~ObjectManager();
	public:
		void Render();
		void RenderForShadow(const unsigned& worldLocation);
		void Initialize(Camera& camera);

		static ObjectManager& Instance();
	private:
		void LoadObjects();
		void PushCube();

		std::vector<Mesh> trees;

		Camera* camera;
	};
}
#endif