#include "Camera.hpp"
#include "config.hpp"
#include <iostream>

namespace GameEngine
{
	Camera::Camera(unsigned windowWidth, unsigned windowHeight, const math::Vector3f& position, const math::Vector3f& target, const math::Vector3f& up) : 
		windowWidth(windowWidth), windowHeight(windowHeight), target(target), up(up),
		pipeline(&this->transform.position, &this->target, &this->up, &transform.viewMatrix, &transform.viewProjectionMatrix, PerspectiveProjection(mFovy, windowWidth, windowHeight, mNear, mFar)), viewIsUpToDate(true)
	{
		assert(windowHeight != 0 || windowWidth != 0);

		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(transform), &transform, GL_DYNAMIC_DRAW);
	}
	Camera::~Camera()
	{
		glDeleteBuffers(1, &UBO);
	}
	void Camera::Move(const math::Vector3f& v)
	{
		transform.position += v;

		viewIsUpToDate = true;
	}
	void Camera::SetPosition(const float& x, const float& y, const float& z)
	{
		transform.position = Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(x, y, z);

		viewIsUpToDate = true;
	}
	void Camera::SetPosition(const math::Vector3f& v)
	{
		transform.position = v;

		viewIsUpToDate = true;
	}
	void Camera::Rotate(const float& x, const float& y)
	{
		verticalAngle += x;
		horizontalAngle += y;

		viewIsUpToDate = true;
	}
	void Camera::OnMouse(const int& x, const int& y)
	{
		math::Vector2i Delta = math::Vector2i(x, y) - lastPositionMouse;

		horizontalAngle += static_cast<float>(Delta.x()) / windowWidth;
		verticalAngle += static_cast<float>(Delta.y()) / windowHeight;

		lastPositionMouse = math::Vector2i(x, y);

		viewIsUpToDate = true;
	}
	void Camera::SetLastPositionMouse(const int& x, const int& y)
	{
		lastPositionMouse = math::Vector2i(x, y);
	}
	void Camera::Update()
	{
		if (viewIsUpToDate)
		{
			math::Vector3f verticalAxis(0.0f, 0.0f, 1.0f); //(0.0f, 1.0f, 0.0f) - zly obrocone bylo

			math::Vector3f newTarget(1.0f, 0.0f, 0.0f);
			newTarget.Rotate(verticalAngle, verticalAxis);
			newTarget.normalize();

			math::Vector3f horizontalAxis = verticalAxis.cross(newTarget);		// obrót w lokalnej przestrzeni 
			horizontalAxis.normalize();
			newTarget.Rotate(horizontalAngle, horizontalAxis);

			target = newTarget;
			target.normalize();

			up = target.cross(horizontalAxis);
			up.normalize();

			pipeline.UpdateVPTransform();

			glBindBuffer(GL_UNIFORM_BUFFER, UBO);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(transform), &transform);

			viewIsUpToDate = false;
		}
	}
	void Camera::BindBuffer(const unsigned& program) const
	{
		glUniformBlockBinding(program, glGetUniformBlockIndex(program, "Transform"), 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	}

	const math::Vector3f& Camera::Position() const { return transform.position; }
	const math::Matrix4f& Camera::ViewMatrix() const { return transform.viewMatrix; }
	const unsigned& Camera::WindowWidth() const { return windowWidth; }
	const unsigned& Camera::WindowHeight() const { return windowHeight; }
	const Pipeline<PerspectiveProjection>& Camera::GetPipeline() const { return pipeline; }

	Camera& Camera::Instance()
	{
		static Camera camera(mWidthWindow, mHeightWindow);
		return camera;
	}
}
