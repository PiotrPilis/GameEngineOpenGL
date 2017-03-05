#ifndef _TERRAIN_HPP_
#define _TERRAIN_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "texture.hpp"
#include "math.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "config.hpp"

namespace GameEngine
{
	class Terrain
	{
	public:
		enum BUFFEROBJECT
		{
			INDEX = 0,
			VERTEX,
			OFFSET,
			NUM_BO
		};
		Terrain();
		Terrain(Camera* camera, const std::string& dir);
		~Terrain();

		// Obowi¹zek rêcznego wywo³ania podczas u¿ycia konstruktora domyœlnego (tylko)!
		GLvoid CreateInstances(Camera* camera, const std::string& dir);

		void Render();
		void RenderForShadow();

		void SetActiveTerrain();
	private:
		GLuint heightMapHeight;
		GLuint heightMapWidth;

		GLboolean Cull(const math::Vector2f& offset);
		GLvoid LoadSetting(const std::string& dir);
		GLvoid LoadHeightMaps(const std::string& dir);

		GLvoid LoadTextures(const std::string& dir);

		GLvoid LoadPathMaps(const std::string& dir);
		GLvoid LoadTexturesID(const std::string& dir);

		Texture pathMaps;

		std::vector<math::Vector2u> pathMapInfo;				// x -> ile tekstur ma obs³u¿yæ - y -> od jakiej tekstury ID
		std::vector<float> pathMapFactor;						// Opisuje przedzia³y path map'y - co ile jest 100% dana tekstura	  100% 66% 33% 0% i next tekstura
		std::vector<std::vector<GLuint>> texturesID;			// Legenda path map'y 

		GLuint numTextures;
		Texture textures;
		Texture normals;
		Texture displacements;

		std::vector<GLint> normalsID;
		std::vector<GLint> displacementsID;

		// -> textures / heightmaps etc.
		math::Vector2u numHeightMaps;
		GLuint terrainInfo;					// Opisuje jak gêsto bêdzie nak³adana height map'a
		GLuint maxHeight;					// Maksymalna wysokoœæ terenu

		Texture heightMaps;
		// <-

		// -> instances
		GLuint terrainAO;
		GLuint bufferObjects[NUM_BO];

		Camera* camera;
		GLuint numIndices;

		//GLint offsetUniform;
		unsigned offsetUniform;
		math::Vector2f mainOffset;
		math::Vector2f cameraPosToUpdateOffset[2];

		unsigned cascadeEndClipSpaceLocation[mNumCascadeShadow];

		void UpdateCameraOffset();
		void UpdateOffset();
		// <-
	public:
		GLint visibleChunks;

		static Shader* terrainVS;
		static Shader* terrainCS;
		static Shader* terrainES;
		static Shader* terrainFS;
		static Program* terrainPO;

		static Light* light;

		friend class DebugVector;
		//DebugVector* dv;
	};

	class DebugVector
	{
	public:
		DebugVector(Terrain* terrain);
		~DebugVector();

		void Render();
	private:
		Shader* VS;
		Shader* GS;
		Shader* FS;
		Program* PO;

		Terrain* terrain;

		GLuint bufferObjects, AO, offsetUniform;
	};
}

#endif