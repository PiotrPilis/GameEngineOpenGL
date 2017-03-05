#ifndef TEXTUREHPP
#define TEXTUREHPP

#include <FreeImage.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "log.hpp"

namespace GameEngine
{
	class Texture
	{
	private:
		enum Type
		{
			NON,
			SINGLE,
			ARRAY
		};
	public:
		// Zawiera tymczasowe informacje o pikselach (zwalniane przy inicjowaniu tekstury)
		class Bitmap
		{
		public:
			Bitmap() = delete;
			Bitmap(const GLchar* filename);
			Bitmap(const GLchar* texturename, const GLuint width, const GLuint height, GLubyte* pixels, const GLuint bpp);
			Bitmap(FIBITMAP* dib);

			GLvoid ReloadBitmap(const GLchar* filename);

			const GLubyte GetPixel(const GLuint& x, const GLuint& y);
			const RGBQUAD GetPixels(const GLuint& x, const GLuint& y);
			BYTE* GetBits();
			GLvoid UpdateByte(const GLuint& x, const GLuint y, BYTE& pixel);

			const GLuint& Height();
			const GLuint& Width();
			const GLuint& Bpp();

			friend class Texture;

			static GLvoid Save(const GLchar* filename, const GLuint width, const GLuint height, BYTE* pixels, const GLuint bpp = 24, const FREE_IMAGE_FORMAT format = FIF_PNG);
			static FIBITMAP* ConvertFromRawBits(const GLuint width, const GLuint height, GLubyte* pixels, const GLuint bpp);

			static GLvoid Unload(const Bitmap& bitmap);
			static GLvoid Unload(std::vector<Bitmap>& bitmaps);

		private:
			// Pobieranie danych o pikselach
			GLvoid LoadBitmap();

			const GLchar* fileName;
			GLuint width, height, bpp;
			FIBITMAP *dib;
		};

		Texture();
		~Texture();

		// U¿ywana do wczytywania pojedyñczej tekstury do VRAM
		GLboolean Initialize(const Bitmap& bitmap, Log& log, bool texturefilteranisotropic = true, GLint texturewraps = GL_CLAMP_TO_EDGE, GLint texturewrapt = GL_CLAMP_TO_EDGE, GLint texturemagfilter = GL_LINEAR, GLint textureminfilter = GL_LINEAR_MIPMAP_LINEAR, GLint generatemipmap = GL_TRUE);
		// U¿ywana do wczytywania tablicy tekstur do VRAM
		GLboolean Initialize(std::vector<Bitmap>& bitmaps, Log& log, bool texturefilteranisotropic = true, GLint texturewraps = GL_CLAMP_TO_EDGE, GLint texturewrapt = GL_CLAMP_TO_EDGE, GLint texturemagfilter = GL_LINEAR, GLint textureminfilter = GL_LINEAR, GLint generatemipmap = GL_FALSE);

		void Bind() const;
	private:

		Type type;
		GLuint obj_texture;
	};
}

#endif