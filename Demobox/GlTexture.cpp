#include "GlTexture.h"

#include <GL/glew.h>
#include <PVRTexture.h>

using namespace Demobox;

GlTexture* GlTexture::sBoundTextures_[32] = { 0 };

GlTexture::GlTexture(const std::string& filename)
	: unit_(0),
	  textureId_(0)
{
	pvrtexture::CPVRTexture cTexture(filename.c_str());

	glGenTextures(1, &textureId_);
	glBindTexture(GL_TEXTURE_2D, textureId_);

	pvrtexture::uint32 internalFormat, format, type;
	cTexture.getOGLFormat(internalFormat, format, type);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, cTexture.getWidth(), cTexture.getHeight(), 0, format, type, cTexture.getDataPtr());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

GlTexture::~GlTexture()
{
	release();
	glDeleteTextures(1, &textureId_);
}

void GlTexture::bind(uint32_t unit /* = 0 */)
{
	if (sBoundTextures_[unit] == this)
		return;

	unit_ = unit;
	glActiveTexture(GL_TEXTURE0 + unit_);
	glBindTexture(GL_TEXTURE_2D, textureId_);

	sBoundTextures_[unit_] = this;
}

void GlTexture::release()
{
	if (sBoundTextures_[unit_] != this)
		return;

	glActiveTexture(GL_TEXTURE0 + unit_);
	glBindTexture(GL_TEXTURE_2D, 0);

	sBoundTextures_[unit_] = nullptr;
}
