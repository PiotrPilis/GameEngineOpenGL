#ifndef _Camera_HPP_
#define _Camera_HPP_

#include <memory>
#include <vector>
#include "math.hpp"
#include "pipeline.hpp"

namespace GameEngine
{
	class Camera
	{
	private:
		Camera() = delete;
		Camera(unsigned windowWidth, unsigned windowHeight, const math::Vector3f& position = math::Vector3f(0.0f, 0.0f, 0.0f), const math::Vector3f& target = math::Vector3f(1.0f, 0.0f, 0.0f), const math::Vector3f& up = math::Vector3f(0.0f, 1.0f, 0.0f));
		~Camera();
	public:

		//void SetScreenSize(const unsigned& windowWidth, const unsigned& windowHeight); 

		void Move(const math::Vector3f& v);
		void SetPosition(const float& x, const float& y, const float& z);
		void SetPosition(const math::Vector3f& v);
		void Rotate(const float& x, const float& y);

		void OnMouse(const int& x, const int& y);
		void SetLastPositionMouse(const int& x, const int& y);

		void Update();
		void BindBuffer(const unsigned& program) const;

		const math::Vector3f& Position() const;
		const math::Matrix4f& ViewMatrix() const;
		const unsigned& WindowWidth() const;
		const unsigned& WindowHeight() const;
		const Pipeline<PerspectiveProjection>& GetPipeline() const;


		static Camera& Instance();
	private:
		unsigned windowWidth;
		unsigned windowHeight;

		math::Vector3f target;
		math::Vector3f up;
		struct
		{
			math::Matrix4f viewProjectionMatrix;
			math::Matrix4f viewMatrix;
			math::Vector3f position;
		} transform;

		float horizontalAngle;
		float verticalAngle;

		math::Vector2i lastPositionMouse;

		Pipeline<PerspectiveProjection> pipeline;

		bool viewIsUpToDate;
		unsigned UBO;
	};
}
#endif
