#ifndef SHADERHPP
#define SHADERHPP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#include "log.hpp"

namespace GameEngine
{
	enum TypeShader
	{
		VertexShader,
		TessellationControlShader,
		TessellationEvaluationShader,
		FragmentShader,
		GeometryShader
	};

	class Shader
	{
	public:
		Shader(const GLchar* filename, TypeShader type);
		~Shader();

		const GLuint& operator* ();
	private:
		GLchar* LoadShaderSource(const GLchar* filename);
		void LoadShader(const GLchar* filename);

		GLuint objshader;
	};
	class Program
	{
	public:
		Program();
		Program(Shader& SH1);
		Program(Shader& SH1, Shader& SH2);
		Program(Shader& SH1, Shader& SH2, Shader& SH3);
		Program(Shader& SH1, Shader& SH2, Shader& SH3, Shader& SH4);
		Program(Shader& SH1, Shader& SH2, Shader& SH3, Shader& SH4, Shader& SH5);
		~Program();

		void Initialize(Shader& SH1);
		void Initialize(Shader& SH1, Shader& SH2);
		void Initialize(Shader& SH1, Shader& SH2, Shader& SH3);
		void Initialize(Shader& SH1, Shader& SH2, Shader& SH3, Shader& SH4);
		void Initialize(Shader& SH1, Shader& SH2, Shader& SH3, Shader& SH4, Shader& SH5);

		const GLuint& operator*() const;
	private:
		void CheckShader(Shader& shader);

		GLuint objprogram;
	};
}

#endif