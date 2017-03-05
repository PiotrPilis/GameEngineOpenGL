#ifndef _LIGHT_HPP_
#define _LIGHT_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "math.hpp"
#include "shadow.hpp"
#include "config.hpp"

namespace GameEngine
{
	class Light						// Na razie ta klasa zosta³a dostosowana wy³¹cznie pod œwiat³o kierunkowe - g³ównie chodzi o cieñ
	{
	public:
		Light();
		~Light();

		// -> Light
		void BindBuffer(const unsigned& program, const unsigned& blockIndex) const;

		void SetPosition(const math::Vector3f& position);
		void SetColor(const math::Vector3f& color);
		void SetBrightnessRange(const math::Vector2f& brightnessRange);

		const math::Vector3f& GetDirection() const;
		// <-
	private:
		void UpdateLight() const;

		void Init();

		unsigned lightLocation;

		mutable struct LightStruct
		{
			math::Matrix4f shadowProjectionMatrix[mNumCascadeShadow];

			math::Vector3f position;
			float dummy;
			math::Vector4f color;
			math::Vector2f brightnessRange;
		} light;



		// -> Shadow
	public:
		void UpdateLightAndShadow();
		const math::Matrix4f& ShadowProjectionMatrix(const unsigned cascadeIndex);
		
		static void InitializeCascadeEndClipSpace(const unsigned& program, unsigned* uniformsLocation);


		ShadowMap shadowMap;

		friend class Camera;
	private:
		math::Matrix4f transformMatrix;												// w tym przypadku wy³¹cznie macierz rotacji œwiat³a

		bool lightViewIsUptodate;
		bool shadowIsUptodate;
		// <-
	};
	/* before editing
		class Light
	{
	public:
		struct LightStruct
		{
			math::Vector4f Color;
			math::Vector4f Direction;

			GLfloat SpecularPower;

			GLfloat AmbientIntensity;
			GLfloat DiffuseIntensity;

			GLfloat SpecularIntensity;
		};

		Light();
		~Light();

		GLvoid BindBuffer(const GLuint& program, const GLuint& blockIndex) const;
		GLvoid SetDirectionalLight(const LightStruct& directionalLight);
		GLvoid MoveDirectionalLight(const math::Vector3f& v);

		GLvoid UpdateMatrix() const;

		LightStruct& DirectionalLight();
	private:
		GLvoid Init();

		LightStruct directionalLight;

		GLuint directionalLightLocation;
	};*/
}
#endif
