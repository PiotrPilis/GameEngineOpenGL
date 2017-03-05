#include <fstream>
#include "shader.hpp"

using namespace std;

namespace GameEngine
{
	Shader::Shader(const GLchar* filename, TypeShader type)
	{
		switch (type)
		{
		case TypeShader::VertexShader:
			objshader = glCreateShader(GL_VERTEX_SHADER);
			break;
		case TypeShader::FragmentShader:
			objshader = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		case TypeShader::TessellationControlShader:
			objshader = glCreateShader(GL_TESS_CONTROL_SHADER);
			break;
		case TypeShader::TessellationEvaluationShader:
			objshader = glCreateShader(GL_TESS_EVALUATION_SHADER); 
			break;
		case TypeShader::GeometryShader:
			objshader = glCreateShader(GL_GEOMETRY_SHADER);
			break;
		default:
			break;
		}
		LoadShader(filename);
	}
	Shader::~Shader()
	{
		glDeleteShader(objshader);
	}
	GLchar* Shader::LoadShaderSource(const GLchar* filename)
	{
		ifstream fin(filename, std::ios::binary | std::ios::ate);
		if (!fin)
			CriticalLog::Write("Could not open " + string(filename), "Could not open " + string(filename));

		fin.seekg(0, std::ios::end);
		GLuint fSize = static_cast< GLuint >(fin.tellg());
		fin.seekg(0, std::ios::beg);

		GLchar* databuff = new char[fSize + 1];
		databuff[fSize] = '\0';

		fin.read(databuff, fSize);

		return databuff;
	}
	void Shader::LoadShader(const GLchar* filename)
	{
		const GLchar* source = LoadShaderSource(filename);
		if (source == NULL)
			return;

		glShaderSource(objshader, 1, &source, NULL);
		glCompileShader(objshader);
		delete[] source;

		GLint status;
		glGetShaderiv(objshader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			GLchar log[10000];
			glGetShaderInfoLog(objshader, 10000, NULL, log);
			CriticalLog::Write("Failed to compile shader: " + string(filename), "Failed to compile shader '" + string(filename) + string("': ") + string(log));
		}
	}
	const GLuint& Shader::operator* () { return objshader; }

	Program::Program() : objprogram(glCreateProgram()) { }
	Program::Program(Shader& SH1)
	{
		Initialize(SH1);
	}
	Program::Program(Shader& SH1, Shader& SH2) : objprogram(glCreateProgram())
	{
		Initialize(SH1, SH2);
	}
	Program::Program(Shader& SH1, Shader& SH2, Shader& SH3) : objprogram(glCreateProgram())
	{
		Initialize(SH1, SH2, SH3);
	}
	Program::Program(Shader& SH1, Shader& SH2, Shader& SH3, Shader& SH4) : objprogram(glCreateProgram())
	{
		Initialize(SH1, SH2, SH3, SH4);
	}
	Program::Program(Shader& SH1, Shader& SH2, Shader& SH3, Shader& SH4, Shader& SH5) : objprogram(glCreateProgram())
	{
		Initialize(SH1, SH2, SH3, SH4, SH5);
	}
	Program::Program::~Program() { glDeleteProgram(objprogram); }
	void Program::Initialize(Shader& SH1)
	{
		glAttachShader(objprogram, *SH1);

		glLinkProgram(objprogram);
		GLint status;
		glGetProgramiv(objprogram, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			GLchar log[10000];
			glGetProgramInfoLog(objprogram, 10000, NULL, log);
			CriticalLog::Write("Failed to link shaders", "Failed to link shaders: " + string(log));
		}
	}
	void Program::Initialize(Shader& SH1, Shader& SH2)
	{
		CheckShader(SH1);
		CheckShader(SH2);
		glAttachShader(objprogram, *SH1);
		glAttachShader(objprogram, *SH2);

		glLinkProgram(objprogram);
		GLint status;
		glGetProgramiv(objprogram, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			GLchar log[10000];
			glGetProgramInfoLog(objprogram, 10000, NULL, log);
			CriticalLog::Write("Failed to link shaders", "Failed to link shaders: " + string(log));
		}
	}
	void Program::Initialize(Shader& SH1, Shader& SH2, Shader& SH3)
	{
		CheckShader(SH1);
		CheckShader(SH2);
		CheckShader(SH3);
		glAttachShader(objprogram, *SH1);
		glAttachShader(objprogram, *SH2);
		glAttachShader(objprogram, *SH3);

		glLinkProgram(objprogram);
		GLint status;
		glGetProgramiv(objprogram, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			GLchar log[10000];
			glGetProgramInfoLog(objprogram, 10000, NULL, log);
			CriticalLog::Write("Failed to link shaders", "Failed to link shaders: " + string(log));
		}
	}
	void Program::Initialize(Shader& SH1, Shader& SH2, Shader& SH3, Shader& SH4)
	{
		CheckShader(SH1);
		CheckShader(SH2);
		CheckShader(SH3);
		CheckShader(SH4);
		glAttachShader(objprogram, *SH1);
		glAttachShader(objprogram, *SH2);
		glAttachShader(objprogram, *SH3);
		glAttachShader(objprogram, *SH4);

		glLinkProgram(objprogram);
		GLint status;
		glGetProgramiv(objprogram, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			GLchar log[10000];
			glGetProgramInfoLog(objprogram, 10000, NULL, log);
			CriticalLog::Write("Failed to link shaders", "Failed to link shaders: " + string(log));
		}
	}
	void Program::Initialize(Shader& SH1, Shader& SH2, Shader& SH3, Shader& SH4, Shader& SH5)
	{
		CheckShader(SH1);
		CheckShader(SH2);
		CheckShader(SH3);
		CheckShader(SH4);
		CheckShader(SH5);
		glAttachShader(objprogram, *SH1);
		glAttachShader(objprogram, *SH2);
		glAttachShader(objprogram, *SH3);
		glAttachShader(objprogram, *SH4);
		glAttachShader(objprogram, *SH5);

		glLinkProgram(objprogram);
		GLint status;
		glGetProgramiv(objprogram, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			GLchar log[10000];
			glGetProgramInfoLog(objprogram, 10000, NULL, log);
			CriticalLog::Write("Failed to link shaders", "Failed to link shaders: " + string(log));
		}
	}
	void Program::CheckShader(Shader& shader)
	{
		if (&shader == NULL)
		{
			CriticalLog::Write("Failed to attach shader", "Failed to attach shader");
		}
	}
	const GLuint& Program::operator*() const
	{
		return objprogram;
	}
}