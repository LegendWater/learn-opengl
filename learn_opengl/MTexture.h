#pragma once
#ifndef MTEXTURE_H
#define MTEXTURE_H

#include <gltools.h>

enum class TexFormat
{
	RGB = GL_RGB,
	RGBA = GL_RGBA
};

class MTexture2D
{
private:
	GLuint id;
	int width;
	int height;
	TexFormat format;

public:
	MTexture2D(int width, int height, TexFormat format, unsigned char* data = nullptr);
	MTexture2D(const char* path);
	MTexture2D(const MTexture2D& tex);
	~MTexture2D();

	inline GLuint getId();
	inline int getWidth();
	inline int getHeight();
	inline TexFormat getFormat();

	void updateData(unsigned char* data);
	void updateRangeData(int x, int y, int width, int height, unsigned char* data);
	void newTex(unsigned char* data);

	void bind();
};
#endif // !MTEXTURE_H
