#include "mesh.hpp"
#include "tree.hpp"
#include "texture.hpp"

using namespace GameEngine;
using namespace std;

Mesh::Mesh() { }
Mesh::Mesh(const TypeMesh& type) : drawCount(0), VAO(0), VBO(0), transform(0)
{
	switch (type)
	{
	case TREE:
		transform = new Transform(Tree::GetWorldMatrixLocation());
		break;
	default:
		ErrorLog::Write("Wrong type of mesh!");
	}
}
Mesh::~Mesh()
{
	if (glIsVertexArray(VAO))
		glDeleteVertexArrays(1, &VAO);

	if (glIsBuffer(VBO))
		glDeleteBuffers(1, &VBO);
}
void Mesh::Load(vector<float>& vertexData)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData[0]) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(VERTEX_BO);
	glVertexAttribPointer(VERTEX_BO, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

	glEnableVertexAttribArray(TEXCOORD_BO);
	glVertexAttribPointer(TEXCOORD_BO, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(NORMAL_BO);
	glVertexAttribPointer(NORMAL_BO, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid*)(5 * sizeof(GLfloat)));

	glBindVertexArray(0);

	drawCount = vertexData.size() / 8;				// 3: vertex; 2: texcoord; 3: normal
	vertexData.clear();
}
void Mesh::Render()
{
	glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, NULL);
	glDrawArrays(GL_TRIANGLES, 0, drawCount);
}
void Mesh::LoadTexture(const std::string& src)
{
	Texture::Bitmap bitmap(("Data/Textures/" + src).c_str());
	texture.Initialize(bitmap, ErrorLog(), GL_FALSE, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_FALSE);
}
void Mesh::LoadTextures(const std::vector<std::string>& srcs)
{
	std::vector<Texture::Bitmap> bitmaps;
	for (unsigned i = 0; i < srcs.size(); ++i)
	{
		bitmaps.push_back(Texture::Bitmap(("Data/Textures/" + srcs[i]).c_str()));
	}
	
	texture.Initialize(bitmaps, ErrorLog(), GL_FALSE, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_FALSE);
}

void Mesh::SetPosition(const math::Vector3f& position) { transform->Position() = position; }
void Mesh::Move(const math::Vector3f& v) { transform->Position() += v; }
math::Vector3f& Mesh::Position() { return transform->Position(); }
Transform* Mesh::WorldMatrix() { return transform; }