#ifndef _Pipeline_HPP_
#define _Pipeline_HPP_

#include <vector> 
#include <memory>
#include "math.hpp"
#include "config.hpp"
#include "log.hpp"


namespace GameEngine
{
	/*class Camera
	{
	private:
		Camera();
		Camera(const Camera& camera);
		~Camera();
		
	public:
		static Camera& Instance();

		// Aktualizacja macierzy widoku (jeœli to potrzebne) oraz macierzy Widoku * Projekcji
		void UpdateMatrix() const;
		void BindBuffer(const unsigned& program) const;

		void SetProjection(float Fovy, unsigned WidthWindow, unsigned HeightWindow, float Near, float Far);		// macierz perspektywiczna
		static math::Matrix4f GetOrthoProjection(float Left, float Right, float Top, float Bottom, float Far, float Near);

		// Translacja
		void SetPosition(const math::Vector3f& position);
		void SetPosition(const float& x, const float& y, const float& z);

		void Move(const math::Vector3f& vector);
		void Move(const float& x, const float& y, const float& z);

		void Zoom(float d);

		// Rotacja
		void SetOrientation(const Eigen::Quaternionf& orientation);
		void Rotate(const float& x, const float& y);
		void SetDirection(const math::Vector3f& direction);

		const math::Vector3f& Position() const;
		const Eigen::Quaternion<float, Eigen::DontAlign>& Orientation() const;
		math::Vector3f Direction() const;
		const Eigen::Matrix<float, 4, 4, Eigen::DontAlign>& VPm() const;
		const unsigned& WindowWidth() const;
		const unsigned& WindowHeight() const;


		friend class Light;
		friend class GameManager;
	private:
		// Pozycja i orientacja (obrót) kamery
		Eigen::Quaternion<float, Eigen::DontAlign> orientation;
		float verticalAngle, horizontalAngle;
		unsigned windowWidth, windowHeight;

		// Afiniczna pozycja kamery (coœ jak wektor z danej pozycji do zerowej (jednostkowy)) (jako macierz)
		// Mutable oznacza const dla zewnêtrznych i edytowalny dla lokalnych metod
		mutable Eigen::Transform<float, 3, 2, Eigen::DontAlign> viewMatrix;
		mutable math::Matrix4f projectionMatrix;

		mutable bool viewIsUptodate;

		math::Vector3f target;
		mutable struct
		{
			Eigen::Matrix<float, 4, 4, Eigen::DontAlign> viewProjectionMatrix;
			Eigen::Matrix<float, 4, 4, Eigen::DontAlign> viewMatrix;
			math::Vector3f position;
		} transform;

		unsigned UBO; // done


		std::vector<Light*> lights;
	};*/
	template<typename T>
	struct Type2Type
	{
		typedef	T	OriginalType;
	};

	/* Class Pipeline - pojemnik przechowuj¹cy macierze, same dane s¹ umiejscowiony gdzie indziej np. w klasie Camera*/
	struct PerspectiveProjection
	{
		PerspectiveProjection();
		PerspectiveProjection(const float& fovy, const float& windowWidth, const float& windowHeight, const float& lNear, const float& lFar);

		float fovy;
		float windowWidth;
		float windowHeight;
		float lNear;
		float lFar;
	};
	struct OrthogonalProjection
	{
		OrthogonalProjection();
		OrthogonalProjection(const float& left, const float& right, const float& bottom, const float& top, const float& lNear, const float& lFar);

		float left;
		float right;
		float bottom;
		float top;
		float lNear;
		float lFar;
	};

	/* Dostarcza wy³¹cznie metody do obliczania macierzy transformacji oraz mo¿e(!) zawieraæ te macierze*/
	template<typename PROJECTION> class Pipeline
	{
	public:
		Pipeline() { }

		/* Nie inicjalizuje macierz perspektywy - zak³ada, ¿e jest ju¿ zainicjalizowana!
		   referancja do position, target, up, view, viewprojection*/
		Pipeline(const math::Vector3f* position, const math::Vector3f* target, const math::Vector3f* up, math::Matrix4f* viewMatrix, math::Matrix4f* viewProjectionMatrix, const math::Matrix4f& projectionMatrix) :
			position(position, [](const math::Vector3f*) {}), target(target, [](const math::Vector3f*) {}), up(up, [](const math::Vector3f*) {}), viewMatrix(viewMatrix, [](math::Matrix4f*) {}), viewProjectionMatrix(viewProjectionMatrix, [](math::Matrix4f*) {}), projectionMatrix(projectionMatrix)
		{
			*viewMatrix.setIdentity();
			*viewProjectionMatrix.setIdentity();
		}

		/* Inicjalizuje macierz perspektywy
		referancja do position, target, up, view, viewprojection*/
		Pipeline(const math::Vector3f* position, const math::Vector3f* target, const math::Vector3f* up, math::Matrix4f* viewMatrix, math::Matrix4f* viewProjectionMatrix, const PROJECTION& projection) :
			position(position, [](const math::Vector3f*) {}), target(target, [](const math::Vector3f*) {}), up(up, [](const math::Vector3f*) {}), viewMatrix(viewMatrix, [](math::Matrix4f*) {}), viewProjectionMatrix(viewProjectionMatrix, [](math::Matrix4f*) {})
		{
			InitializeProjection(projection);
			(*viewMatrix).setIdentity();
			(*viewProjectionMatrix).setIdentity();
		}

		/* Nie inicjalizuje macierz perspektywy - zak³ada, ¿e jest ju¿ zainicjalizowana!
		brak referencij do position, target, up, view, viewprojection - tworzy nowe obiekty*/
		Pipeline(const math::Vector3f& position, const math::Vector3f& target, const math::Vector3f& up, const math::Matrix4f& viewMatrix, const math::Matrix4f& viewProjectionMatrix, const math::Matrix4f& projectionMatrix) :
			position(new math::Vector3f(position)), target(new math::Vector3f(target)), up(new math::Vector3f(up)), viewMatrix(new math::Matrix4f(viewMatrix)), viewProjectionMatrix(new math::Matrix4f(viewProjectionMatrix)), projectionMatrix(projectionMatrix)
		{
		}

		/* Inicjalizuje macierz perspektywy
		brak referencij do position, target, up, view, viewprojection - tworzy nowe obiekty*/
		Pipeline(const math::Vector3f& position, const math::Vector3f& target, const math::Vector3f& up, const math::Matrix4f& viewMatrix, const math::Matrix4f& viewProjectionMatrix, const PROJECTION& projection) :
			position(new math::Vector3f(position)), target(new math::Vector3f(target)), up(new math::Vector3f(up)), viewMatrix(new math::Matrix4f(viewMatrix)), viewProjectionMatrix(new math::Matrix4f(viewProjectionMatrix))
		{
			InitializeProjection(projection);
		}

		/* Inicjalizuje macierz perspektywy
		brak referencij do position, up, view, viewprojection - tworzy nowe obiekty; target - referencja*/
		Pipeline(const math::Vector3f& position, const math::Vector3f* target, const math::Vector3f& up, const math::Matrix4f& viewMatrix, const math::Matrix4f& viewProjectionMatrix, const PROJECTION& projection) :
			position(new math::Vector3f(position)), target(target, [](const math::Vector3f*) {}), up(new math::Vector3f(up)), viewMatrix(new math::Matrix4f(viewMatrix)), viewProjectionMatrix(new math::Matrix4f(viewProjectionMatrix))
		{
			InitializeProjection(projection);
		}

		void InitializeProjection(const PROJECTION& projection)
		{
			InitializeProjection(projection, Type2Type < PROJECTION >());
		}

	private:
		template < typename U >
		void InitializeProjection(const PROJECTION& projection, Type2Type<U>)
		{
			CriticalLog::Write("Undefined projection class", "Undefined projection class");
		}
		void InitializeProjection(const PROJECTION& projection, Type2Type<PerspectiveProjection>)
		{
			const float aspect = projection.windowWidth / projection.windowHeight;
			const float range = projection.lNear - projection.lFar;
			const float tanHalfFOV = tanf(ToRadian(projection.fovy / 2.0f));

			projectionMatrix.setIdentity();

			projectionMatrix(0, 0) = 1.0f / (tanHalfFOV * aspect);
			projectionMatrix(1, 1) = 1.0f / tanHalfFOV;
			projectionMatrix(2, 2) = (-projection.lNear - projection.lFar) / range;
			projectionMatrix(3, 2) = 1.0f;
			projectionMatrix(2, 3) = 2.0f * projection.lFar * projection.lNear / range;
			projectionMatrix(3, 3) = 0.0f;
		}
		void InitializeProjection(const PROJECTION& projection, Type2Type<OrthogonalProjection>)
		{
			projectionMatrix.setIdentity();

			projectionMatrix(0, 0) = 2.0f / (projection.right - projection.left);
			projectionMatrix(0, 3) = -(projection.right + projection.left) / (projection.right - projection.left);
			projectionMatrix(1, 1) = 2.0f / (projection.top - projection.bottom);
			projectionMatrix(1, 3) = -(projection.top + projection.bottom) / (projection.top - projection.bottom);
			projectionMatrix(2, 2) = 2.0f / (projection.lFar - projection.lNear);
			projectionMatrix(2, 3) = -(projection.lFar + projection.lNear) / (projection.lFar - projection.lNear);
		}

	public:
		void UpdateVTransform()
		{
			math::Matrix4f CameraTranslationTransform, CameraRotateTransform;

			CameraTranslationTransform.SetTranslationTransform(-(*position));
			CameraRotateTransform.SetCameraTransform(*target, *up);

			*viewMatrix = CameraRotateTransform * CameraTranslationTransform;
		}
		void UpdateVPTransform()
		{
			UpdateVTransform();
			*viewProjectionMatrix = projectionMatrix * (*viewMatrix);
			
		}
		void UpdateOnlyVPTransform()
		{
			*viewProjectionMatrix = projectionMatrix * (*viewMatrix);
		}
		
		const math::Matrix4f& ViewMatrix() const { return *viewMatrix; }
		const math::Matrix4f& ProjectionMatrix() const { return projectionMatrix; }
		const math::Matrix4f& ViewProjectionMatrix() const { return *viewProjectionMatrix; }

		friend class Camera;
	private:
		math::Matrix4f projectionMatrix;
		std::shared_ptr<math::Matrix4f> viewMatrix;

		std::shared_ptr<math::Matrix4f> viewProjectionMatrix;

		std::shared_ptr<const math::Vector3f> position;						// wskaŸnik jesli podano jako shared_ptr lub nowa wartoœæ jak podano jako wartoœæ
		std::shared_ptr<const math::Vector3f> target;
		std::shared_ptr<const math::Vector3f> up;
	};
	class Transform
	{
	public:
		Transform() = delete;
		Transform(unsigned& matrixLocation);

		math::Vector3f& Position();

		void SetMatrix();
		void SetMatrix(const unsigned& matrixLocation);
	private:
		inline void UpdateMatrix();

		math::Matrix4f worldMatrix;
		math::Vector3f position;

		unsigned& matrixLocation;
		bool transformIsUptodate;
	};
}
#endif