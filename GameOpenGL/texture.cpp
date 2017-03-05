#include <string>
#include "texture.hpp"
#include "log.hpp"
#include "config.hpp"
#include <iostream>
namespace GameEngine
{
	Texture::Bitmap::Bitmap(const GLchar* filename) : dib(0), width(0), height(0), bpp(0), fileName(filename)
	{
		LoadBitmap();
	}
	Texture::Bitmap::Bitmap(const GLchar* texturename, const GLuint width, const GLuint height, GLubyte* pixels, const GLuint bpp) : dib(0), width(width), height(height), bpp(bpp), fileName(texturename)
	{
		if (width == 0 || height == 0)
		{
			ErrorLog::Write("Wrong width or height: '" + std::string(fileName) + "'");
			return;
		}
		dib = ConvertFromRawBits(width, height, pixels, bpp);

		if (width > GL_MAX_TEXTURE_SIZE) this->width = GL_MAX_TEXTURE_SIZE;
		if (height > GL_MAX_TEXTURE_SIZE) this->height = GL_MAX_TEXTURE_SIZE;

		GLuint OriginalWidth = width;
		GLuint OriginalHeight = height;

		if (!GLEW_ARB_texture_non_power_of_two)
		{
			this->width = 1 << static_cast<GLint>(floor((log(static_cast<GLfloat>(width)) / log(2.0f)) + 0.5f));
			this->height = 1 << static_cast<GLint>(floor((log(static_cast<GLfloat>(height)) / log(2.0f)) + 0.5f));
		}

		if (width != OriginalWidth || height != OriginalHeight)
		{
			FIBITMAP *rdib = FreeImage_Rescale(dib, width, height, FILTER_BICUBIC);
			FreeImage_Unload(dib);
			dib = rdib;
		}
	}
	Texture::Bitmap::Bitmap(FIBITMAP* dib) : dib(dib), fileName("")
	{
		if (dib != NULL)
		{
			width = FreeImage_GetWidth(dib);
			height = FreeImage_GetHeight(dib);

			if (width == 0 || height == 0)
			{
				FreeImage_Unload(dib);
				ErrorLog::Write("Wrong width or height: '" + std::string(fileName) + "'");
				return;
			}

			bpp = FreeImage_GetBPP(dib);

			if (width > GL_MAX_TEXTURE_SIZE) width = GL_MAX_TEXTURE_SIZE;
			if (height > GL_MAX_TEXTURE_SIZE) height = GL_MAX_TEXTURE_SIZE;

			GLuint OriginalWidth = width;
			GLuint OriginalHeight = height;

			if (!GLEW_ARB_texture_non_power_of_two)
			{
				width = 1 << static_cast<GLint>(floor((log(static_cast<GLfloat>(width)) / log(2.0f)) + 0.5f));
				height = 1 << static_cast<GLint>(floor((log(static_cast<GLfloat>(height)) / log(2.0f)) + 0.5f));
			}

			if (width != OriginalWidth || height != OriginalHeight)
			{
				FIBITMAP *rdib = FreeImage_Rescale(dib, width, height, FILTER_BICUBIC);
				FreeImage_Unload(dib);
				dib = rdib;
			}
		}
		else
		{
			ErrorLog::Write("Can't create bitmap: '" + std::string(fileName) + "'");
		}
	}
	GLvoid Texture::Bitmap::LoadBitmap()
	{
		FreeImage_Unload(dib);
		FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(fileName);

		if (fif == FIF_UNKNOWN)
			fif = FreeImage_GetFIFFromFilename(fileName);

		if (fif == FIF_UNKNOWN)
		{
			ErrorLog::Write("Can't load bitmap: '" + std::string(fileName) + "'");
			return;
		}
		if (FreeImage_FIFSupportsReading(fif))
			dib = FreeImage_Load(fif, fileName);

		if (dib != NULL)
		{
			width = FreeImage_GetWidth(dib);
			height = FreeImage_GetHeight(dib);

			if (width == 0 || height == 0)
			{
				FreeImage_Unload(dib);
				ErrorLog::Write("Wrong width or height: '" + std::string(fileName) + "'");
				return;
			}

			bpp = FreeImage_GetBPP(dib);

			if (width > GL_MAX_TEXTURE_SIZE) width = GL_MAX_TEXTURE_SIZE;
			if (height > GL_MAX_TEXTURE_SIZE) height = GL_MAX_TEXTURE_SIZE;

			GLuint OriginalWidth = width;
			GLuint OriginalHeight = height;

			if (!GLEW_ARB_texture_non_power_of_two)
			{
				width = 1 << static_cast<GLint>(floor((log(static_cast<GLfloat>(width)) / log(2.0f)) + 0.5f));
				height = 1 << static_cast<GLint>(floor((log(static_cast<GLfloat>(height)) / log(2.0f)) + 0.5f));
			}

			if (width != OriginalWidth || height != OriginalHeight)
			{
				FIBITMAP *rdib = FreeImage_Rescale(dib, width, height, FILTER_BICUBIC);
				FreeImage_Unload(dib);
				dib = rdib;
			}
		}
		else
		{
			ErrorLog::Write("Can't load bitmap: '" + std::string(fileName) + "'");
		}
	}
	GLvoid Texture::Bitmap::ReloadBitmap(const GLchar* filename)
	{
		fileName = filename;
		if (dib == NULL)
			FreeImage_Unload(dib);
		width = height = bpp = 0;
		LoadBitmap();
	}
	const GLubyte Texture::Bitmap::GetPixel(const GLuint& x, const GLuint& y)
	{
		GLubyte value = 0;
		FreeImage_GetPixelIndex(dib, x, y, &value);
		return value;
	}
	const RGBQUAD Texture::Bitmap::GetPixels(const GLuint& x, const GLuint& y)
	{
		RGBQUAD value;
		FreeImage_GetPixelColor(dib, x, y, &value);

		return value;
	}
	BYTE* Texture::Bitmap::GetBits()
	{
		return FreeImage_GetBits(dib);
	}
	GLvoid Texture::Bitmap::UpdateByte(const GLuint& x, const GLuint y, BYTE& pixel)
	{
		GetBits()[(x + y * width)*bpp/8] = pixel;
	}

	GLvoid Texture::Bitmap::Unload(std::vector<Bitmap>& bitmaps)
	{
		for (GLuint i = 0; i < bitmaps.size(); ++i)
			FreeImage_Unload(bitmaps[i].dib);
		bitmaps.clear();

	}
	GLvoid Texture::Bitmap::Unload(const Bitmap& bitmap)
	{
		FreeImage_Unload(bitmap.dib);
	}

	const GLuint& Texture::Bitmap::Height() { return height; }
	const GLuint& Texture::Bitmap::Width() { return width; }
	const GLuint& Texture::Bitmap::Bpp() { return bpp; }

	Texture::Texture() : obj_texture(0), type(NON) { }
	Texture::~Texture() 
	{
		glDeleteTextures(1, &obj_texture);
	}

	GLboolean Texture::Initialize(const Bitmap& bitmap, Log& log, bool texture_filter_anisotropic, GLint texture_wrap_s, GLint texture_wrap_t, GLint texture_mag_filter, GLint texture_min_filter, GLint generate_mipmap)
	{
		if (bitmap.dib == NULL)
		{
			log("Data of texture does not exist : " + std::string(bitmap.fileName));
			return false;
		}

		GLenum Format = 0;
		GLenum internalFormat = 0;
		switch (bitmap.bpp)
		{
		case 32:
			Format = GL_BGRA;
			internalFormat = GL_RGBA8;
			break;
		case 24:
			Format = GL_BGR;
			internalFormat = GL_RGBA8;
			break;
		case 8:
			Format = GL_ALPHA;
			internalFormat = GL_ALPHA8;
			break;
		}

		if (Format == 0)
		{
			log("Unsupported texture format : " + std::string(bitmap.fileName));
			FreeImage_Unload(bitmap.dib);
			return false;
		}

		glGenTextures(1, &(obj_texture));
		glBindTexture(GL_TEXTURE_2D, (obj_texture));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_wrap_t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_mag_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_min_filter);

		if (GLEW_EXT_texture_filter_anisotropic && texture_filter_anisotropic)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, generate_mipmap);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, bitmap.width, bitmap.height, 0, Format, GL_UNSIGNED_BYTE, FreeImage_GetBits(bitmap.dib));
		glBindTexture(GL_TEXTURE_2D, 0);

		type = SINGLE;
		return GL_TRUE;
	}
	GLboolean Texture::Initialize(std::vector<Bitmap>& bitmaps, Log& log, bool texture_filter_anisotropic, GLint texture_wrap_s, GLint texture_wrap_t, GLint texture_mag_filter, GLint texture_min_filter, GLint generate_mipmap)
	{
		if (bitmaps.size() < 1)
			return false;

		for (GLuint i = 0; i < bitmaps.size(); ++i)
		{
			if (bitmaps[i].dib == NULL)
			{
				log("Data of texture does not exist : '" + std::string(bitmaps[i].fileName) + "'");
				return false;
			}
		}

		GLenum Format = 0;
		GLenum internalFormat = 0;
		switch (bitmaps[0].bpp)
		{
		case 32:
			Format = GL_BGRA;
			internalFormat = GL_RGBA8;
			break;
		case 24:
			Format = GL_BGR;
			internalFormat = GL_RGBA8;
			break;
		case 8:
			Format = GL_ALPHA;
			internalFormat = GL_ALPHA8;
			break;
		}

		if (Format == 0)
		{
			log("Unsupported texture format : " + std::string(bitmaps[0].fileName));
			return false;
		}
		for (GLuint i = 1; i < bitmaps.size(); ++i)
		{
			if (bitmaps[0].bpp != bitmaps[i].bpp || bitmaps[0].width != bitmaps[i].width || bitmaps[0].height != bitmaps[i].height)
			{
				log("Incompatible textures format : " + std::string(bitmaps[i].fileName));
				return false;
			}
		}

		glGenTextures(1, &obj_texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, obj_texture);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, bitmaps[0].width, bitmaps[0].height, bitmaps.size(), 0,
			Format, GL_UNSIGNED_BYTE, NULL);
		for (GLuint i = 0; i < bitmaps.size(); ++i)
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, bitmaps[0].width, bitmaps[0].height, 1,
				Format, GL_UNSIGNED_BYTE, FreeImage_GetBits(bitmaps[i].dib));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, texture_wrap_s);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, texture_wrap_t);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, texture_mag_filter);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, texture_min_filter);

		if (GLEW_EXT_texture_filter_anisotropic && texture_filter_anisotropic)
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, generate_mipmap);
		glBindTexture(GL_TEXTURE_2D, 0);

		type = ARRAY;
		return GL_TRUE;
	}
	void Texture::Bind() const
	{
		if (type == Type::ARRAY)
			glBindTexture(GL_TEXTURE_2D_ARRAY, obj_texture);
		else if (type == Type::SINGLE)
			glBindTexture(GL_TEXTURE_2D, obj_texture);
	}

	GLvoid Texture::Bitmap::Save(const GLchar* filename, const GLuint width, const GLuint height, BYTE* pixels, const GLuint bpp, const FREE_IMAGE_FORMAT format)
	{
		if (pixels == NULL)
			ErrorLog::Write("Buffer is NULL '" + std::string(filename) + "'");
		else
		{
			FIBITMAP* Image = FreeImage_ConvertFromRawBits(pixels, width, height, width*(bpp / 8), bpp, 0x0000FF, 0x00FF00, 0xFF0000, GL_FALSE);
			FreeImage_Save(format, Image, filename, 0);
		}
	}
	FIBITMAP* Texture::Bitmap::ConvertFromRawBits(const GLuint width, const GLuint height, BYTE* pixels, const GLuint bpp)
	{
		if (pixels == NULL)
			ErrorLog::Write("Buffer is NULL");
		return FreeImage_ConvertFromRawBits(pixels, width, height, width * bpp / 8, bpp, 0x0000FF, 0x00FF00, 0xFF0000, GL_FALSE);
	}
}