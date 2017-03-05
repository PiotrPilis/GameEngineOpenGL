#include <Eigen\LU>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;
#include <sstream>

#include "terrain.hpp"
#include "config.hpp"
#include "pipeline.hpp"

using namespace Eigen;

namespace GameEngine
{
	PerspectiveProjection::PerspectiveProjection() : fovy(mFovy), windowWidth(mWidthWindow), windowHeight(mHeightWindow), lNear(mNear), lFar(mFar) { }
	PerspectiveProjection::PerspectiveProjection(const float& fovy, const float& windowWidth, const float& windowHeight, const float& lNear, const float& lFar) : fovy(fovy), windowWidth(windowWidth), windowHeight(windowHeight), lNear(lNear), lFar(lFar) { }
	OrthogonalProjection::OrthogonalProjection() : left(-10.0), right(10.0), bottom(-10.0), top(10.0), lNear(mNear), lFar(mFar) { }
	OrthogonalProjection::OrthogonalProjection(const float& left, const float& right, const float& bottom, const float& top, const float& lNear, const float& lFar) : left(left), right(right), bottom(bottom), top(top), lNear(lNear), lFar(lFar) { }


	Transform::Transform(unsigned& matrixLocation) : position(math::Vector3f(0.0f, 0.0f, 0.0f)), transformIsUptodate(true), matrixLocation(matrixLocation)
	{

	}
	inline void Transform::UpdateMatrix()
	{
		if (transformIsUptodate)
		{
			Eigen::Transform<float, 3, 2, Eigen::DontAlign> mat;
			mat.setIdentity();
			mat.translation() = mat.linear()*position;
			worldMatrix = mat.matrix();
			transformIsUptodate = false;
		}
	}
	void Transform::SetMatrix()
	{
		UpdateMatrix();
		glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, worldMatrix.data());
	}
	void Transform::SetMatrix(const unsigned& matrixLocation)
	{
		UpdateMatrix();
		glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, worldMatrix.data());
	}
	math::Vector3f& Transform::Position()
	{
		return position;
	}

}

