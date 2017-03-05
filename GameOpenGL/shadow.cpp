#include <GL/glew.h>
#include "shadow.hpp"

using namespace GameEngine;

ShadowMap::ShadowMap(const math::Vector3f& direction) : fbo(0), pipeline(Pipeline<OrthogonalProjection>(math::Vector3f(0.0f, 0.0f, 0.0f), &direction, math::Vector3f(0.0f, 1.0f, 0.0f), math::Matrix4f::Identity(), math::Matrix4f::Identity(), OrthogonalProjection()))
{
	Init(mWidthWindow, mHeightWindow, CriticalLog());
}
ShadowMap::~ShadowMap()
{
	if (glIsFramebuffer(fbo)) {
		glDeleteFramebuffers(1, &fbo);
	}

	if (shadowMaps[0]) {
		glDeleteTextures(mNumCascadeShadow, shadowMaps);
	}
}
void ShadowMap::Init(const unsigned& WindowWidth, const unsigned& WindowHeight, Log& log)
{
	glGenFramebuffers(1, &fbo);

	glGenTextures(mNumCascadeShadow, shadowMaps);

	for (unsigned i = 0; i < mNumCascadeShadow; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMaps[0], 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::stringstream out;
		out.setf(std::ios::hex, std::ios::basefield);
		out << "Frame Buffer Object error, status: 0x" << status;
		log(out);
		out.unsetf(std::ios::hex);
	}
}
void ShadowMap::Enable()
{
	glUseProgram(*GetProgram());
}
void ShadowMap::BindForWriting(const unsigned cascadeIndex)
{
	assert(cascadeIndex < mNumCascadeShadow);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMaps[cascadeIndex], 0);
}
void ShadowMap::BindForReading(const unsigned textureUnit0, const unsigned textureUnit1, const unsigned textureUnit2)
{
	glActiveTexture(textureUnit0);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[0]);

	glActiveTexture(textureUnit1);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[1]);

	glActiveTexture(textureUnit2);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[2]);
}
Program& ShadowMap::GetProgram()
{
	static Program program = Program(
		Shader("Data/Shaders/shadow.vs", TypeShader::VertexShader),
		Shader("Data/Shaders/shadow.fs", TypeShader::FragmentShader));
	return program;
}
const unsigned& ShadowMap::VPMatrixLocation()
{
	static unsigned mVPMatrixLocation = glGetUniformLocation(*GetProgram(), "vpMatrix");
	return mVPMatrixLocation;
}
const unsigned& ShadowMap::WorldMatrixLocation()
{
	static unsigned mWorldMatrixLocation = glGetUniformLocation(*GetProgram(), "wMatrix");
	return mWorldMatrixLocation;
}
void ShadowMap::SetVP(const math::Matrix4f& VP)
{
	glUniformMatrix4fv(VPMatrixLocation(), 1, GL_FALSE, VP.data());
}
Pipeline<OrthogonalProjection>& ShadowMap::GetPipeline()
{
	return pipeline;
}