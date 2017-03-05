#ifndef _MATH_HPP_
#define _MATH_HPP_
#include <Eigen/Geometry>

#include <iostream>

namespace math
{
	using namespace Eigen;

	#define	M_PI	3.14159265358979323846

	#define ToRadian(x) (float)(((x) * M_PI / 180.0f))

	typedef	Eigen::Matrix<float, 3, 3, Eigen::DontAlign>	Matrix3f;
	//typedef	Eigen::Matrix<float, 4, 4, Eigen::DontAlign>	Matrix4f;
	typedef	Eigen::Matrix<float, 2, 1, Eigen::DontAlign>	Vector2f;
	//typedef	Eigen::Matrix<float, 3, 1, Eigen::DontAlign>	Vector3f;
	typedef	Eigen::Matrix<float, 4, 1, Eigen::DontAlign>	Vector4f;
	typedef	Eigen::Matrix<double, 2, 1, Eigen::DontAlign>	Vector2d;
	typedef	Eigen::Matrix<int, 2, 1, Eigen::DontAlign>	Vector2i;
	typedef	Eigen::Matrix<unsigned, 2, 1, Eigen::DontAlign>	Vector2u;
	typedef	Eigen::Matrix<unsigned, 3, 1, Eigen::DontAlign>	Vector3u;
	typedef	Eigen::Matrix<unsigned char, 2, 1, Eigen::DontAlign>	Vector2b;
	typedef	Eigen::Matrix<float, 1, 1, Eigen::DontAlign>	Pointf;

	class Vector3f : public Matrix<float, 3, 1, Eigen::DontAlign>
	{
	public:
		using Matrix<float, 3, 1, Eigen::DontAlign>::Matrix;

		inline void Rotate(const float& angle, const Vector3f& axis)
		{
			*this = AngleAxisf(ToRadian(angle), axis) * (*this);
		}

		Vector3f Rotated(const float& angle, const Vector3f& axis) const
		{
			return AngleAxisf(ToRadian(angle), axis) * (*this);
		}
	};

	class Matrix4f : public Matrix<float, 4, 4, Eigen::DontAlign>
	{
	public:
		using Matrix<float, 4, 4, Eigen::DontAlign>::Matrix;

		inline void SetTranslationTransform(const float& x, const float& y, const float& z)
		{
			(*this).setIdentity();
			(*this)(0, 3) = x;
			(*this)(1, 3) = y;
			(*this)(2, 3) = z;
		}
		inline void SetTranslationTransform(const Vector3f& position)
		{
			(*this).setIdentity();
			(*this)(0, 3) = position.x();
			(*this)(1, 3) = position.y();
			(*this)(2, 3) = position.z();
		}
		inline void SetCameraTransform(Vector3f target, Vector3f up)
		{
			target.normalize();
			up.normalize();
			up = up.cross(target);
			Vector3f v = target.cross(up);

			(*this).setIdentity();
			(*this)(0, 0) = up.x();     (*this)(0, 1) = up.y();     (*this)(0, 2) = up.z();
			(*this)(1, 0) = v.x();      (*this)(1, 1) = v.y();      (*this)(1, 2) = v.z();
			(*this)(2, 0) = target.x(); (*this)(2, 1) = target.y(); (*this)(2, 2) = target.z();
		}
	};

}

#endif