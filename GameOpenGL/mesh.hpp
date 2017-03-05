#ifndef MESHHPP
#define MESHHPP

#include "math.hpp"
#include "texture.hpp"
#include "pipeline.hpp"
#include "shader.hpp"

namespace GameEngine
{
	class Mesh
	{
	private:
		Mesh();
	public:
		enum TypeMesh
		{
			TREE
		};

		Mesh(const TypeMesh& type);
		~Mesh();
		void Load(std::vector<float>& vertexData);												// without indices
		void LoadTexture(const std::string& src);
		void LoadTextures(const std::vector<std::string>& srcs);
		void Render();																			// calling to single render - multi rendering at tree.h etc. TODO

		void SetPosition(const math::Vector3f& position);
		void Move(const math::Vector3f& v);
		math::Vector3f& Position();
		Transform* WorldMatrix();

		Texture texture;
	private:
		unsigned VBO;
		unsigned VAO;

		unsigned drawCount;

		Transform* transform;

		enum
		{
			VERTEX_BO,
			TEXCOORD_BO,
			NORMAL_BO,
			INDICES_BO,
		};
	};
}
#endif