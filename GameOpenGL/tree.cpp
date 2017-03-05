#include "tree.hpp"
#include "terrain.hpp"
#include <vector>

using namespace GameEngine;
using namespace std;

Tree::Tree() { }
void Tree::InitializeProgram()
{
	static unsigned numCalls = 0;
	if (!numCalls)
	{
		glUseProgram(*GetProgram());
		glUniform1i(glGetUniformLocation(*Tree::GetProgram(), "textures"), 0);
		glUniform1i(glGetUniformLocation(*Tree::GetProgram(), "shadowMap[0]"), 1);
		glUniform1i(glGetUniformLocation(*Tree::GetProgram(), "shadowMap[1]"), 2);
		glUniform1i(glGetUniformLocation(*Tree::GetProgram(), "shadowMap[2]"), 3);

		Terrain::light->InitializeCascadeEndClipSpace(*Tree::GetProgram(), cascadeEndClipSpaceLocation);

		NormalLog::Write("Program (tree) initialized!");

		numCalls = 1;
	}
}
Program& Tree::GetProgram()
{
	static Program program = Program(Shader(
		"Data/Shaders/tree.vs", TypeShader::VertexShader),
		Shader("Data/Shaders/tree.fs", TypeShader::FragmentShader));
	return program;
}
Tree& Tree::Instance()
{
	static Tree tree;
	return tree;
}
unsigned& Tree::GetWorldMatrixLocation()
{
	static unsigned matrixLocation = glGetUniformLocation(*Tree::GetProgram(), "modelMatrix");
	return matrixLocation;
}