#include "light.hpp"
#include "log.hpp"
#include "camera.hpp"

#include <iostream>

GameEngine::Light::Light() : shadowMap(light.position), lightViewIsUptodate(false), shadowIsUptodate(false)
{
	transformMatrix.setIdentity();
	for (unsigned i = 0; i < mNumCascadeShadow; ++i)
		light.shadowProjectionMatrix[i].setIdentity();

	Init();

	/*zmiana klasy camera>>  Camera::Instance().lights.push_back(this);*/
}
GameEngine::Light::~Light()
{
	glDeleteBuffers(1, &lightLocation);
}
void GameEngine::Light::Init()
{
	light.color = math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	light.position = math::Vector3f(0.0f, 1.0f, -2.5f);
	light.brightnessRange = math::Vector2f(0.5, 1.0);

	light.position.normalize();

	glGenBuffers(1, &lightLocation);
	glBindBuffer(GL_UNIFORM_BUFFER, lightLocation);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(light), &light, GL_DYNAMIC_DRAW);

	if (lightLocation == 0)
		CriticalLog::Write("Can't load light", "Uniform is null (Light)");
}
void GameEngine::Light::BindBuffer(const unsigned& program, const unsigned& blockIndex) const
{
	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "Light"), blockIndex);
	glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, lightLocation);
}
void GameEngine::Light::UpdateLight() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, lightLocation);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(light), &light);
}
void GameEngine::Light::SetPosition(const math::Vector3f& position)
{
	light.position = position;
	light.position.normalize();
	lightViewIsUptodate = shadowIsUptodate = false;
	// nie potrzebna aktualizacja uniformów, bo ustawiaj¹ siê flagi do zaaktualizowania - zaaktualizuje siê w pêtli g³ównej programu
}
void GameEngine::Light::SetColor(const math::Vector3f& color)
{
	light.color.x() = color.x();
	light.color.y() = color.y();
	light.color.z() = color.z();
	lightViewIsUptodate = false;
}
void GameEngine::Light::SetBrightnessRange(const math::Vector2f& brightnessRange)
{
	light.brightnessRange = brightnessRange;
	lightViewIsUptodate = false;
}
void GameEngine::Light::UpdateLightAndShadow()
{
	if (!shadowIsUptodate || 1)
	{
		math::Matrix4f camInv = Camera::Instance().ViewMatrix().inverse();
		if (!lightViewIsUptodate || 1)
			shadowMap.GetPipeline().UpdateVTransform();

		const math::Matrix4f& lightMatrix = shadowMap.GetPipeline().ViewMatrix();

		float aspect = static_cast<float>(Camera::Instance().WindowWidth()) / Camera::Instance().WindowHeight();
		float tanHalfHFOV = tanf(ToRadian(mFovy / 2.0f)) * aspect;
		float tanHalfVFOV = tanf(ToRadian((mFovy ) / 2.0f));

		for (unsigned i = 0; i < mNumCascadeShadow; ++i) {
			float cascadedShadowBegin = mCascadedShadowEnd[i];
			float cascadedShadowEnd = mCascadedShadowEnd[i + 1];

			float xn = cascadedShadowBegin * tanHalfHFOV;
			float xf = cascadedShadowEnd * tanHalfHFOV;
			float yn = cascadedShadowBegin * tanHalfVFOV;
			float yf = cascadedShadowEnd * tanHalfVFOV;


			math::Vector4f frustumCorners[8] = {
				// near face
				math::Vector4f(xn,   yn, cascadedShadowBegin, 1.0),
				math::Vector4f(-xn,  yn, cascadedShadowBegin, 1.0),
				math::Vector4f(xn,  -yn, cascadedShadowBegin, 1.0),
				math::Vector4f(-xn, -yn, cascadedShadowBegin, 1.0),

				// far face
				math::Vector4f(xf,   yf, cascadedShadowEnd, 1.0),
				math::Vector4f(-xf,  yf, cascadedShadowEnd, 1.0),
				math::Vector4f(xf,  -yf, cascadedShadowEnd, 1.0),
				math::Vector4f(-xf, -yf, cascadedShadowEnd, 1.0)
			};

			math::Vector4f frustumCornersLight[8];										// rogi frustry uwzglêdniaj¹ce macierz widoku kamery oraz orientacje œwiat³a

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();

			for (unsigned j = 0; j < 8; ++j)
			{
				math::Vector4f vW = camInv * frustumCorners[j];
				frustumCornersLight[j] = lightMatrix * vW;

				minX = fmin(minX, frustumCornersLight[j].x());
				maxX = fmax(maxX, frustumCornersLight[j].x());
				minY = fmin(minY, frustumCornersLight[j].y());
				maxY = fmax(maxY, frustumCornersLight[j].y());
				minZ = fmin(minZ, frustumCornersLight[j].z());
				maxZ = fmax(maxZ, frustumCornersLight[j].z());
			}

			shadowMap.GetPipeline().InitializeProjection(OrthogonalProjection(minX, maxX, minY, maxY, minZ, maxZ));					// jesli bêdzie coœ Ÿle wyœwietlaæ mo¿e trzeba zamieniæ osie ...?!
			shadowMap.GetPipeline().UpdateOnlyVPTransform();
			light.shadowProjectionMatrix[i] = shadowMap.GetPipeline().ViewProjectionMatrix();

		}

		shadowIsUptodate = true;
	}
	if (!lightViewIsUptodate || 1)
	{
		UpdateLight();
		lightViewIsUptodate = true;
	}
	
}
const math::Matrix4f& GameEngine::Light::ShadowProjectionMatrix(const unsigned cascadeIndex)
{
	assert(cascadeIndex < mNumCascadeShadow);
	return light.shadowProjectionMatrix[cascadeIndex];
}

void GameEngine::Light::InitializeCascadeEndClipSpace(const unsigned& program, unsigned* uniformsLocation)
{
	for (unsigned i = 0; i < mNumCascadeShadow; ++i)
	{
		std::stringstream sout;
		sout << "cascadeEndClipSpace[" << i << "]";
		uniformsLocation[i] = glGetUniformLocation(program, sout.str().c_str());

		math::Vector4f vClip = Camera::Instance().GetPipeline().ProjectionMatrix() * math::Vector4f(0.0f, 0.0f, mCascadedShadowEnd[i + 1], 1.0f);
		glUniform1f(uniformsLocation[i], vClip.z());
	}

}

const math::Vector3f& GameEngine::Light::GetDirection() const
{
	return light.position;
}
