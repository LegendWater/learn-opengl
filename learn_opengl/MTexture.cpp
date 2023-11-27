#include "MTexture.h"
#include <sys/stat.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif // !STB_IMAGE_IMPLEMENTATION

MTexture2D::MTexture2D(int width, int height, TexFormat format, unsigned char* data)
	:id(-1), width(width), height(height), format(format)
{
	if (width <= 0 || height <= 0) {
		id = -1;
		return;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, (int)format, width, height, 0, (int)format, GL_UNSIGNED_BYTE, data);
	if (data) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}

MTexture2D::MTexture2D(const char* path)
	:id(-1), width(-1), height(-1), format(TexFormat::RGB)
{
	struct stat filestat;
	if (stat(path, &filestat) == -1) { //文件不存在
		std::printf(__FUNCTION__"(): texture file not exist\n");
		return;
	}

	int channel;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* image_data = stbi_load(path, &width, &height, &channel, 0);

	format = channel == 4 ? TexFormat::RGBA : TexFormat::RGB;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, (int)format, width, height, 0, (int)format, GL_UNSIGNED_BYTE, image_data);
	if (image_data) {
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image_data);
	}

}

MTexture2D::MTexture2D(const MTexture2D& tex)
	:id(tex.id), width(tex.width), height(tex.height), format(tex.format)
{
}

MTexture2D::~MTexture2D()
{
	glDeleteTextures(1, &id);
}

inline GLuint MTexture2D::getId()
{
	return id;
}

inline int MTexture2D::getWidth()
{
	return width;
}

inline int MTexture2D::getHeight()
{
	return height;
}

inline TexFormat MTexture2D::getFormat()
{
	return format;
}

void MTexture2D::updateData(unsigned char* data)
{
	//不检查data是否有效
	glBindTexture(GL_TEXTURE_2D, id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, (int)format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void MTexture2D::updateRangeData(int x, int y, int width, int height, unsigned char* data)
{
	if (x < 0 || x > width - 1 || y < 0 || y > height - 1 || width > this->width || height > this->height) {
		std::printf(__FUNCTION__"(): input param invalid\n");
		return;
	}
	glBindTexture(GL_TEXTURE_2D, id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, (int)format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void MTexture2D::newTex(unsigned char* data)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, (int)format, width, height, 0, (int)format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void MTexture2D::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}
