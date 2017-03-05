#ifndef _SHADOW_HPP_
#define _SHADOW_HPP_

#include <GLFW/glfw3.h>
#include "log.hpp"
#include "shader.hpp"
#include "pipeline.hpp"
#include "config.hpp"

namespace GameEngine
{
	// Kaskadowa mapa cieni
	class ShadowMap			
	{
	public:
		ShadowMap() = delete;
		ShadowMap(const math::Vector3f& direction);
		~ShadowMap();

		void Init(const unsigned& WindowWidth, const unsigned& WindowHeight, Log& log = CriticalLog());

		// Aktywacja programu ShadowMap
		void Enable();

		void BindForWriting(const unsigned cascadeIndex);
		void BindForReading(const unsigned textureUnit0, const unsigned textureUnit1, const unsigned textureUnit2);

		// Funkcja pobieraj¹ca obiekt programu ShadowMap (singleton)
		static Program& GetProgram();

		void SetVP(const math::Matrix4f& VP);

		Pipeline<OrthogonalProjection>& GetPipeline();

		// Lokalizacja zmiennej jednolitej dla programu cieni - ka¿dy obiekt musi mieæ do niego dostêp
		static const unsigned& WorldMatrixLocation();					
	private:
		unsigned fbo;
		unsigned shadowMaps[mNumCascadeShadow];

		Pipeline<OrthogonalProjection> pipeline;

		static const unsigned& VPMatrixLocation();
		
	};
}
#endif