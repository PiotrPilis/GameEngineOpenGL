#include <Eigen\LU>
#include <Eigen/Geometry>
#include <fstream>
#include "terrain.hpp"
#include <gl/glut.h>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <sstream>


#include <iostream>

#define	EXT_HEIGHT_MAP	"/height.png"
#define	EXT_MAP_SETTING	"/setting.ini"

#define	EXT_TEXTURES_SETTING	"/textures.ini"
#define	EXT_TEXTURESID_LIST	"/textures.ini"

#define	EXT_PATH_MAP	"/path.png"

/*GameEngine::DebugVector::DebugVector(Terrain* terrain) : terrain(terrain)
{
	VS = new Shader("Data/Shaders/line.vs", TypeShader::VertexShader);
	GS = new Shader("Data/Shaders/line.gs", TypeShader::GeometryShader);
	FS = new Shader("Data/Shaders/line.fs", TypeShader::FragmentShader);

	PO = new Program();
	PO->Initialize(*VS, *GS, *FS);
	glUseProgram(**PO);
	//terrain->camera->BindBuffer(**PO);

	glUniform4f(glGetUniformLocation(**PO, "terrainInfo"), static_cast<GLfloat>(terrain->terrainInfo), static_cast<GLfloat>(terrain->numHeightMaps.x()), static_cast<GLfloat>(terrain->numHeightMaps.y()), static_cast<GLfloat>(terrain->maxHeight));

	GLfloat step = ChunkSize / DetailChunk;
	glUniform2f(glGetUniformLocation(**PO, "step"), step, static_cast<float>(DetailChunk + 1));

	glUniform1i(glGetUniformLocation(**PO, "heightMap"), 0);

	offsetUniform = glGetUniformLocation(**PO, "offset");

}
GameEngine::DebugVector::~DebugVector()
{
	delete VS;
	delete GS;
	delete FS;
	delete PO;

	glDeleteVertexArrays(1, &AO);
	glDeleteBuffers(1, &bufferObjects);
}
void GameEngine::DebugVector::Render()
{
	glLineWidth(2.0);
	glUseProgram(**PO);
	glBindVertexArray(terrain->terrainAO);
	glActiveTexture(GL_TEXTURE0);
	terrain->heightMaps.Bind();

	GLfloat startOffsetX = terrain->camera->Position().x() - terrain->startOffset;
	GLfloat startOffsetY = terrain->camera->Position().y() - terrain->startOffset;

	for (GLuint x = 0; x < NumRowChunks; ++x)
	{
		for (GLuint y = 0; y < NumRowChunks; ++y)
		{
			math::Vector2f offset = math::Vector2f(startOffsetX + x * (ChunkSize), startOffsetY + y * (ChunkSize));
			glUniform2f(offsetUniform, offset.x(), offset.y());
			glDrawElements(GL_POINTS, terrain->numIndices, GL_UNSIGNED_INT, 0);
		}
	}

}*/

GameEngine::Shader* GameEngine::Terrain::terrainVS = 0;
GameEngine::Shader* GameEngine::Terrain::terrainCS = 0;
GameEngine::Shader* GameEngine::Terrain::terrainES = 0;
GameEngine::Shader* GameEngine::Terrain::terrainFS = 0;
GameEngine::Program* GameEngine::Terrain::terrainPO = 0;

GameEngine::Light* GameEngine::Terrain::light = 0;

GameEngine::Terrain::Terrain() : camera(0), visibleChunks(0), numIndices(0), offsetUniform(0), terrainInfo(0), heightMapHeight(0), heightMapWidth(0), maxHeight(0), numTextures(0)
{
}
GameEngine::Terrain::Terrain(Camera* camera, const std::string& dir) : visibleChunks(0), numIndices(0), offsetUniform(0), terrainInfo(0), heightMapHeight(0), heightMapWidth(0), maxHeight(0), numTextures(0)
{
	CreateInstances(camera, dir);

	//dv = new DebugVector(this);
}
GameEngine::Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &terrainAO);
	glDeleteBuffers(NUM_BO, bufferObjects);
	glDeleteBuffers(1, &offsetUniform);
}


GLvoid GameEngine::Terrain::CreateInstances(Camera* camera, const std::string& dir)
{
	NormalLog::Write("Loading terrain...");
	GameEngine::Terrain::camera = camera;
	
	// --> Loading
	LoadSetting(dir);
	LoadHeightMaps(dir);
	LoadTextures(dir);
	LoadPathMaps(dir);
	// <--

	// --> Program
	if (!glIsProgram(**terrainPO))
		CriticalLog::Write("Failed to load terrain", "terrainPO is not a program!");
	glUseProgram(**terrainPO);
	camera->BindBuffer(**terrainPO);
	light->BindBuffer(**terrainPO, 1);
#ifndef TerrainRenderMultiTask
	offsetUniform = glGetUniformLocation(**terrainPO, "offset");
#else
	glGenBuffers(1, &offsetUniform);
	glBindBuffer(GL_UNIFORM_BUFFER, offsetUniform);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(math::Vector4f) * NumChunks, NULL, GL_DYNAMIC_DRAW);
	glUniformBlockBinding(**terrainPO, glGetUniformBlockIndex(**terrainPO, "Offset"), 2);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, offsetUniform);
#endif

	// x -> gêstoœæ texCoord - y -> numHeightMaps.x - z -> numHeightMaps.y - w -> maxHeight
	glUniform4f(glGetUniformLocation(**terrainPO, "terrainInfo"), static_cast<GLfloat>(terrainInfo), static_cast<GLfloat>(numHeightMaps.x()), static_cast<GLfloat>(numHeightMaps.y()), static_cast<GLfloat>(maxHeight));

	// x -> ile tekstur ma obs³u¿yæ - y -> od jakiej tekstury ID
	GLuint* arrayV = new GLuint[pathMapInfo.size() * 2];
	for (GLuint i = 0; i < pathMapInfo.size(); ++i)
	{
		arrayV[i * 2] = pathMapInfo[i].x();
		arrayV[i * 2 + 1] = pathMapInfo[i].y();
		std::cout << "[" << i << "]: " << arrayV[i * 2] << " : " << arrayV[i * 2 + 1] << std::endl;
	}
	glUniform2uiv(glGetUniformLocation(**terrainPO, "pathMapInfo"), pathMapInfo.size() * 2, arrayV);
	delete[] arrayV;

	float* farrayV = new float[pathMapFactor.size()];
	for (unsigned i = 0; i < pathMapFactor.size(); ++i)
		farrayV[i] = pathMapFactor[i];
	glUniform1fv(glGetUniformLocation(**terrainPO, "pathMapFactor"), pathMapFactor.size(), farrayV);
	delete[] farrayV;

	GLuint numTexturesID = 0;
	for (GLuint y = 0; y < numHeightMaps.y(); ++y)
		for (GLuint x = 0; x < numHeightMaps.x(); ++x)
			numTexturesID += texturesID[x + y * numHeightMaps.x()].size();
	arrayV = new GLuint[numTexturesID];
	GLuint k = 0;
	for (GLuint y = 0; y < numHeightMaps.y(); ++y)
		for (GLuint x = 0; x < numHeightMaps.x(); ++x)
			for (GLuint i = 0; i < texturesID[x + y * numHeightMaps.x()].size(); ++i)
			{
				arrayV[k] = texturesID[x + y * numHeightMaps.x()][i];
				++k;
			}
	glUniform1uiv(glGetUniformLocation(**terrainPO, "texturesID"), numTexturesID, arrayV);
	delete[] arrayV;

	GLint* iarrayV = new GLint[normalsID.size()];
	for (GLuint i = 0; i < normalsID.size(); ++i)
	{
		iarrayV[i] = normalsID[i];
	}
	glUniform1iv(glGetUniformLocation(**terrainPO, "normalID"), normalsID.size(), iarrayV);
	delete[] iarrayV;
	normalsID.clear();

	iarrayV = new GLint[displacementsID.size()];
	for (GLuint i = 0; i < displacementsID.size(); ++i)
	{
		iarrayV[i] = displacementsID[i];
	}
	glUniform1iv(glGetUniformLocation(**terrainPO, "displacementID"), displacementsID.size(), iarrayV);
	delete[] iarrayV;
	displacementsID.clear();

	glUniform1i(glGetUniformLocation(**terrainPO, "heightMap"), 0);
	glUniform1i(glGetUniformLocation(**terrainPO, "textures"), 1);
	glUniform1i(glGetUniformLocation(**terrainPO, "pathMap"), 2);
	glUniform1i(glGetUniformLocation(**terrainPO, "normalMap"), 3);
	glUniform1i(glGetUniformLocation(**terrainPO, "displacementMap"), 4);
	glUniform1i(glGetUniformLocation(**terrainPO, "shadowMap[0]"), 5);
	glUniform1i(glGetUniformLocation(**terrainPO, "shadowMap[1]"), 6);
	glUniform1i(glGetUniformLocation(**terrainPO, "shadowMap[2]"), 7);

	Terrain::light->InitializeCascadeEndClipSpace(**terrainPO, cascadeEndClipSpaceLocation);
	// <--

	// --> BO
	glGenVertexArrays(1, &terrainAO);
	glBindVertexArray(terrainAO);
	glGenBuffers(NUM_BO, bufferObjects);

	numIndices = DetailChunk * DetailChunk * 4;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjects[INDEX]);																	// TODO: Usuñ indeksy prymitywa
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numIndices, NULL, GL_STATIC_DRAW);

	GLuint* address = (GLuint*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * numIndices,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	for (GLuint x = 0; x < DetailChunk; ++x)
	{
		for (GLuint y = 0; y < DetailChunk; ++y)
		{
			address[(x + y*DetailChunk) * 4] = x + y*(DetailChunk + 1);						// x     -> y
			address[(x + y*DetailChunk) * 4 + 1] = x + 1 + y*(DetailChunk + 1);				// x + 1 -> y
			address[(x + y*DetailChunk) * 4 + 2] = x + 1 + (y + 1)*(DetailChunk + 1);		// x + 1 -> y + 1
			address[(x + y*DetailChunk) * 4 + 3] = x + (y + 1)*(DetailChunk + 1);			// x     -> y + 1
		}
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(math::Vector2f) * (DetailChunk + 1) * (DetailChunk + 1), NULL, GL_STATIC_DRAW);

	math::Vector2f* vertexAddr = (math::Vector2f*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(math::Vector2f) * (DetailChunk + 1) * (DetailChunk + 1),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	GLfloat step = ChunkSize / DetailChunk;
	for (GLuint x = 0; x <= DetailChunk; ++x)
	{
		for (GLuint y = 0; y <= DetailChunk; ++y)
		{
			vertexAddr[x + y * (DetailChunk + 1)] = math::Vector2f((x - static_cast<GLfloat>(DetailChunk)/2.0f) * step, (y - static_cast<GLfloat>(DetailChunk) / 2.0f) * step);
		}
	}

	// A po za³adowaniu usuwam wskaŸnik ¿eby nie mo¿na by³o ponownie za³adowaæ!
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glEnableVertexAttribArray(VERTEX);
	glVertexAttribPointer(VERTEX, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// <--


	GLenum glError;
	if ((glError = glGetError()) != GL_NO_ERROR) {
		std::stringstream out;
		out << "Failed to load terrain: '" << gluErrorString(glError) << "'";
		CriticalLog::Write("Failed to load terrain!", out.str());
	}
	
	mainOffset = math::Vector2f(camera->Position().x(), camera->Position().y());
	cameraPosToUpdateOffset[0] = math::Vector2f(mainOffset.x() + ChunkSize * (UpdateChunkOffsetFactor + 0.5f), mainOffset.y() + ChunkSize * (UpdateChunkOffsetFactor + 0.5f));
	cameraPosToUpdateOffset[1] = math::Vector2f(mainOffset.x() - ChunkSize * (UpdateChunkOffsetFactor + 0.5f), mainOffset.y() - ChunkSize * (UpdateChunkOffsetFactor + 0.5f));
}
GLvoid GameEngine::Terrain::LoadSetting(const std::string& dir)
{
	std::string SettingPath = dir + EXT_MAP_SETTING;
	std::ifstream fin(SettingPath);
	if (!fin)
		CriticalLog::Write("Failed to load terrain!", "Failed to load terrain setting (1001) '" + SettingPath);
	std::string tmp;
	getline(fin, tmp);
	if (tmp != "MapSetting")
		CriticalLog::Write("Failed to load terrain!", "Wrong structure of terrain setting (1010) '" + SettingPath + "'");

	fin >> tmp >> numHeightMaps.x() >> numHeightMaps.y() >> tmp >> terrainInfo >> tmp >> maxHeight;

	fin.close();
}


GLvoid GameEngine::Terrain::LoadHeightMaps(const std::string& dir)
{
	if (!numHeightMaps.x() || !numHeightMaps.y() || numHeightMaps.x() > 256 || numHeightMaps.y() > 256)
		CriticalLog::Write("Failed to load terrain", "Failed to load setting (1100) '" + dir + EXT_MAP_SETTING + "'");

	std::vector<Texture::Bitmap>* bitMaps = new std::vector<Texture::Bitmap>();
	bitMaps->reserve(numHeightMaps.x() * numHeightMaps.y());

	for (GLuint y = 0; y < numHeightMaps.y(); ++y)
		for (GLuint x = 0; x < numHeightMaps.x(); ++x)
		{
			std::stringstream out;
			out << dir << "/" << (x + y * numHeightMaps.x()) << EXT_HEIGHT_MAP;
			bitMaps->push_back(Texture::Bitmap(out.str().c_str()));
		}

	heightMapHeight = (*bitMaps)[0].Height();
	heightMapWidth = (*bitMaps)[0].Width();
	heightMaps.Initialize(*bitMaps, CriticalLog(), false, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, GL_FALSE);

	Texture::Bitmap::Unload(*bitMaps);
	delete bitMaps;
}
GLvoid GameEngine::Terrain::LoadTextures(const std::string& dir)
{
	std::string SettingPath = dir + EXT_TEXTURES_SETTING;
	std::ifstream fin(SettingPath);
	if (!fin)
		CriticalLog::Write("Failed to load terrain!", "Failed to load textures setting (1002) '" + SettingPath);
	std::string tmp;
	getline(fin, tmp);
	if (tmp != "TexturesSetting")
		CriticalLog::Write("Failed to load terrain!", "Wrong structure of textures setting (1020) '" + SettingPath + "'");

	fin >> tmp >> numTextures;
	std::vector<Texture::Bitmap> bitmaps;
	std::vector<Texture::Bitmap> normalmaps;
	std::vector<Texture::Bitmap> displacementmaps;

	bitmaps.reserve(numTextures);
	normalsID.reserve(numTextures);
	displacementsID.reserve(numTextures);

	std::getline(fin, tmp);
	for (GLuint i = 0; i < numTextures; ++i)
	{
		std::getline(fin, tmp);

		std::vector<std::string> strs;
		boost::split(strs, tmp, boost::is_any_of("\t "));
		if (strs.size() > 1)
		{
			if (strs[1] != "NULL")
			{
				normalsID.push_back(normalmaps.size());
				normalmaps.push_back(Texture::Bitmap(Texture::Bitmap(("Data/" + strs[1]).c_str())));
			}

			if (strs.size() > 2)
			{
				displacementsID.push_back(displacementmaps.size());
				displacementmaps.push_back(Texture::Bitmap(Texture::Bitmap(("Data/" + strs[2]).c_str())));
			}
			else
				displacementsID.push_back(-1);
		}
		else
			normalsID.push_back(-1);

		bitmaps.push_back(Texture::Bitmap(("Data/" + strs[0]).c_str()));
	}
	textures.Initialize(bitmaps, CriticalLog(), GL_FALSE, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_FALSE);
	normals.Initialize(normalmaps, CriticalLog(), GL_FALSE, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_FALSE);
	displacements.Initialize(displacementmaps, CriticalLog(), GL_FALSE, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_FALSE);

	fin.close();

#ifdef GAME_EDITOR

//#else
	Texture::Bitmap::Unload(bitmaps);
	Texture::Bitmap::Unload(normalmaps);
	Texture::Bitmap::Unload(displacementmaps);
#endif
}
GLvoid GameEngine::Terrain::LoadPathMaps(const std::string& dir)
{
	LoadTexturesID(dir);

	GLuint LastNumTexturesID = 0;
	pathMapInfo.reserve(numHeightMaps.x() * numHeightMaps.y());
	std::vector<Texture::Bitmap> bitMaps;
	for (GLuint y = 0; y < numHeightMaps.y(); ++y)
		for (GLuint x = 0; x < numHeightMaps.x(); ++x)
		{
			std::stringstream out;
			out << dir << "/" << (x + y * numHeightMaps.x()) << EXT_PATH_MAP;
			bitMaps.push_back(Texture::Bitmap(out.str().c_str()));

			// x -> ile tekstur ma obs³u¿yæ - y -> od jakiej tekstury ID
			pathMapInfo.push_back(math::Vector2u(texturesID[x + y * numHeightMaps.x()].size() - 1, LastNumTexturesID));


			unsigned numTexturesperHeightMap = texturesID[x + y * numHeightMaps.x()].size();
			// Opisuje przedzia³y path map'y - co ile jest 100% dana tekstura	  1: 0%; 2: 33%; 3: 66%; 4: 100%,        dla num = 4
			pathMapFactor.push_back(1.0f / (numTexturesperHeightMap - 1));

			LastNumTexturesID += numTexturesperHeightMap;
		}
	pathMaps.Initialize(bitMaps, CriticalLog(), false, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_FALSE);
#ifdef GAME_EDITOR
	Texture::Bitmap::Unload(bitMaps);
#endif
}

GLvoid GameEngine::Terrain::LoadTexturesID(const std::string& dir)
{
	texturesID.reserve(numHeightMaps.x() * numHeightMaps.y());

	for (GLuint y = 0; y < numHeightMaps.y(); ++y)
		for (GLuint x = 0; x < numHeightMaps.x(); ++x)
		{
			std::stringstream sout;
			sout << dir << "/" << (x + y * numHeightMaps.x()) << EXT_TEXTURESID_LIST;
			std::ifstream fin(sout.str());
			if (!fin)
				CriticalLog::Write("Failed to load terrain!", "Failed to open textures ID setting (1003) '" + sout.str());
			std::string tmp;
			getline(fin, tmp);
			if (tmp != "TexturesID")
				CriticalLog::Write("Failed to load terrain!", "Wrong structure of textures ID setting (1030) '" + sout.str() + "'");

			GLuint numTexturesID;
			fin >> tmp >> numTexturesID;

			texturesID.push_back(std::vector<GLuint>());
			texturesID[x + y * numHeightMaps.x()].reserve(numTexturesID);
			for (GLuint i = 0; i < numTexturesID; ++i)
			{
				GLuint ID;
				fin >> tmp >> ID;
				texturesID[x + y * numHeightMaps.x()].push_back(ID);
			}
			fin.close();
		}
}

GLboolean GameEngine::Terrain::Cull(const math::Vector2f& offset)
{
	if (offset.x() < 0 || offset.y() < 0 || offset.x() > numHeightMaps.x() * terrainInfo || offset.y() > numHeightMaps.y() * terrainInfo)
		return GL_FALSE;

	GLint outOfBound[6] = { 0, 0, 0, 0, 0, 0 };

	const math::Matrix4f& camVPm = Camera::Instance().GetPipeline().ViewProjectionMatrix();
	math::Vector4f clipCoord = camVPm * math::Vector4f(offset.x() + HalfChunkSize, offset.y() + HalfChunkSize, maxHeight, 1.0f);
	if (clipCoord.x() > clipCoord.w()) outOfBound[0]++;
	if (clipCoord.x() < -clipCoord.w()) outOfBound[1]++;
	if (clipCoord.y() > clipCoord.w()) outOfBound[2]++;
	if (clipCoord.y() < -clipCoord.w()) outOfBound[3]++;
	if (clipCoord.z() > clipCoord.w()) outOfBound[4]++;
	if (clipCoord.z() < -clipCoord.w()) outOfBound[5]++;

	clipCoord = camVPm * math::Vector4f(offset.x() + HalfChunkSize, offset.y() + HalfChunkSize, 0.0f, 1.0f);
	if (clipCoord.x() > clipCoord.w()) outOfBound[0]++;
	if (clipCoord.x() < -clipCoord.w()) outOfBound[1]++;
	if (clipCoord.y() > clipCoord.w()) outOfBound[2]++;
	if (clipCoord.y() < -clipCoord.w()) outOfBound[3]++;
	if (clipCoord.z() > clipCoord.w()) outOfBound[4]++;
	if (clipCoord.z() < -clipCoord.w()) outOfBound[5]++;



	clipCoord = camVPm * math::Vector4f(offset.x() - HalfChunkSize, offset.y() + HalfChunkSize, maxHeight, 1.0f);
	if (clipCoord.x() > clipCoord.w()) outOfBound[0]++;
	if (clipCoord.x() < -clipCoord.w()) outOfBound[1]++;
	if (clipCoord.y() > clipCoord.w()) outOfBound[2]++;
	if (clipCoord.y() < -clipCoord.w()) outOfBound[3]++;
	if (clipCoord.z() > clipCoord.w()) outOfBound[4]++;
	if (clipCoord.z() < -clipCoord.w()) outOfBound[5]++;

	clipCoord = camVPm * math::Vector4f(offset.x() - HalfChunkSize, offset.y() + HalfChunkSize, 0.0f, 1.0f);
	if (clipCoord.x() > clipCoord.w()) outOfBound[0]++;
	if (clipCoord.x() < -clipCoord.w()) outOfBound[1]++;
	if (clipCoord.y() > clipCoord.w()) outOfBound[2]++;
	if (clipCoord.y() < -clipCoord.w()) outOfBound[3]++;
	if (clipCoord.z() > clipCoord.w()) outOfBound[4]++;
	if (clipCoord.z() < -clipCoord.w()) outOfBound[5]++;



	clipCoord = camVPm * math::Vector4f(offset.x() - HalfChunkSize, offset.y() - HalfChunkSize, maxHeight, 1.0f);
	if (clipCoord.x() > clipCoord.w()) outOfBound[0]++;
	if (clipCoord.x() < -clipCoord.w()) outOfBound[1]++;
	if (clipCoord.y() > clipCoord.w()) outOfBound[2]++;
	if (clipCoord.y() < -clipCoord.w()) outOfBound[3]++;
	if (clipCoord.z() > clipCoord.w()) outOfBound[4]++;
	if (clipCoord.z() < -clipCoord.w()) outOfBound[5]++;

	clipCoord = camVPm * math::Vector4f(offset.x() - HalfChunkSize, offset.y() - HalfChunkSize, 0.0f, 1.0f);
	if (clipCoord.x() > clipCoord.w()) outOfBound[0]++;
	if (clipCoord.x() < -clipCoord.w()) outOfBound[1]++;
	if (clipCoord.y() > clipCoord.w()) outOfBound[2]++;
	if (clipCoord.y() < -clipCoord.w()) outOfBound[3]++;
	if (clipCoord.z() > clipCoord.w()) outOfBound[4]++;
	if (clipCoord.z() < -clipCoord.w()) outOfBound[5]++;



	clipCoord = camVPm * math::Vector4f(offset.x() + HalfChunkSize, offset.y() - HalfChunkSize, maxHeight, 1.0f);
	if (clipCoord.x() > clipCoord.w()) outOfBound[0]++;
	if (clipCoord.x() < -clipCoord.w()) outOfBound[1]++;
	if (clipCoord.y() > clipCoord.w()) outOfBound[2]++;
	if (clipCoord.y() < -clipCoord.w()) outOfBound[3]++;
	if (clipCoord.z() > clipCoord.w()) outOfBound[4]++;
	if (clipCoord.z() < -clipCoord.w()) outOfBound[5]++;

	clipCoord = camVPm * math::Vector4f(offset.x() + HalfChunkSize, offset.y() - HalfChunkSize, 0.0f, 1.0f);
	if (clipCoord.x() > clipCoord.w()) outOfBound[0]++;
	if (clipCoord.x() < -clipCoord.w()) outOfBound[1]++;
	if (clipCoord.y() > clipCoord.w()) outOfBound[2]++;
	if (clipCoord.y() < -clipCoord.w()) outOfBound[3]++;
	if (clipCoord.z() > clipCoord.w()) outOfBound[4]++;
	if (clipCoord.z() < -clipCoord.w()) outOfBound[5]++;



	for (GLint i = 0; i < 6; ++i)
		if (outOfBound[i] == 8) return GL_FALSE;
		
	return GL_TRUE;
}

void GameEngine::Terrain::UpdateCameraOffset()
{
	if (camera->Position().x() > cameraPosToUpdateOffset[0].x())
	{
		mainOffset.x() += ChunkSize;
		cameraPosToUpdateOffset[0].x() += ChunkSize;
		cameraPosToUpdateOffset[1].x() += ChunkSize;
	}
	else if (camera->Position().x() < cameraPosToUpdateOffset[1].x())
	{
		mainOffset.x() -= ChunkSize;
		cameraPosToUpdateOffset[0].x() -= ChunkSize;
		cameraPosToUpdateOffset[1].x() -= ChunkSize;
	}
	if (camera->Position().y() > cameraPosToUpdateOffset[0].y())
	{
		mainOffset.y() += ChunkSize;
		cameraPosToUpdateOffset[0].y() += ChunkSize;
		cameraPosToUpdateOffset[1].y() += ChunkSize;
	}
	else if (camera->Position().y() < cameraPosToUpdateOffset[1].y())
	{
		mainOffset.y() -= ChunkSize;
		cameraPosToUpdateOffset[0].y() -= ChunkSize;
		cameraPosToUpdateOffset[1].y() -= ChunkSize;
	}
}

void GameEngine::Terrain::UpdateOffset()
{
	UpdateCameraOffset();

	glBindBuffer(GL_UNIFORM_BUFFER, offsetUniform);
	math::Vector2f* offsets = (math::Vector2f*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(math::Vector4f) * NumChunks,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	visibleChunks = 0;
	for (int y = -HalfNumRowChunks; y <= HalfNumRowChunks; ++y)
	{
		for (int x = -HalfNumRowChunks; x <= HalfNumRowChunks; ++x)
		{
			//math::Vector2f offset = math::Vector2f(mainOffset.x() + x * ChunkSize, mainOffset.y() + y * ChunkSize);
			if (1)//Cull(offset))
			{
				//offsets[visibleChunks * 2] = offset;
				//visibleChunks++;
			}
		}
	}
	
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GameEngine::Terrain::Render()
{
#ifdef TerrainPolygonModeLine
		glPolygonMode(GL_FRONT, GL_LINE);
#endif
		glUseProgram(**terrainPO);
		glBindVertexArray(terrainAO);
		glActiveTexture(GL_TEXTURE0);
		heightMaps.Bind();
		glActiveTexture(GL_TEXTURE1);
		textures.Bind();
		glActiveTexture(GL_TEXTURE2);
		pathMaps.Bind();
		glActiveTexture(GL_TEXTURE3);
		normals.Bind();
		light->shadowMap.BindForReading(GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7);

#ifdef TerrainRenderMultiTask
		UpdateOffset();
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawElementsInstanced(GL_PATCHES, numIndices, GL_UNSIGNED_INT, NULL, 9);
#else
		UpdateCameraOffset();

		visibleChunks = 0;
		for (int x = -HalfNumRowChunks; x <= HalfNumRowChunks; ++x)
		{
			for (int y = -HalfNumRowChunks; y <= HalfNumRowChunks; ++y)
			{
				math::Vector2f offset = math::Vector2f(mainOffset.x() + x * ChunkSize, mainOffset.y() + y * ChunkSize);
				++visibleChunks;
				glUniform2f(offsetUniform, offset.x(), offset.y());

				glPatchParameteri(GL_PATCH_VERTICES, 4);
				glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, NULL);
			}
		}
#endif
#ifdef TerrainPolygonModeLine
		glPolygonMode(GL_FRONT, GL_FILL);
#endif

		glBindVertexArray(0);

		// Rysuje wektory normalnych, tangentów, bitangentów - jest inicjowane wczeœniej, na koneic usuñ to...
		//dv->Render();
}

void GameEngine::Terrain::RenderForShadow()
{
	for (int x = -HalfNumRowChunks; x <= HalfNumRowChunks; ++x)
	{
		for (int y = -HalfNumRowChunks; y <= HalfNumRowChunks; ++y)
		{
			math::Vector2f offset = math::Vector2f(mainOffset.x() + x * ChunkSize, mainOffset.y() + y * ChunkSize);
			glUniform2f(offsetUniform, offset.x(), offset.y());

			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);
		}
	}
	glUniform2f(offsetUniform, 0.0f, 0.0f);					// ¿eby nie by³o przesuniêcia dla innych obiektów
}