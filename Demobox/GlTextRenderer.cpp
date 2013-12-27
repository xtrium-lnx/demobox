#include "GlTextRenderer.h"
#include "GlShader.h"

#include <exception>
#include <GL/glew.h>

using namespace Demobox;

const char* fontVS =
"varying vec2 v_tc;"
"void main() { gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xy, 0.0, 1.0); v_tc = gl_Vertex.zw; }";

const char* fontFS =
"varying vec2 v_tc;"
"uniform sampler2D u_glyph;"
"uniform float u_alpha;"
"void main() { gl_FragColor = vec4(vec3(1.0), texture2D(u_glyph, v_tc).r * u_alpha); }";

GlTextRenderer* GlTextRenderer::instance_ = nullptr;

GlTextRenderer::GlTextRenderer()
	: currentFace_(nullptr),
	  alignment_(Alignment::CENTER)
{
	if (FT_Init_FreeType(&freetype_))
		throw new std::exception("Unable to initialize FreeType.");

	glEnable(GL_TEXTURE);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &glyphTextureId_);
	glBindTexture(GL_TEXTURE_2D, glyphTextureId_);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenBuffers(1, &glyphVBOId_);

	fontShader_ = new GlShader();
	fontShader_->setVertexShader(fontVS);
	fontShader_->setFragmentShader(fontFS);
	fontShader_->build();

	fontShader_->bind();
	  fontShader_->setUniform("u_glyph", 0);
	fontShader_->release();
}

GlTextRenderer::~GlTextRenderer()
{
	glDeleteTextures(1, &glyphTextureId_);
}

GlTextRenderer* GlTextRenderer::instance()
{
	if (!instance_)
		instance_ = new GlTextRenderer();

	return instance_;
}

void GlTextRenderer::useFont(const std::string& name, uint32_t size)
{
	if (faces_.count(name))
	{
		if (faces_[name].count(size))
		{
			currentFace_ = faces_[name][size];
			return; // A face for this font and size already exists
		}
	}

	currentFace_ = nullptr;

	if (FT_New_Face(freetype_, name.c_str(), 0, &currentFace_))
		throw new std::exception(std::string("Unable to load font" + name).c_str());

	faces_[name][size] = currentFace_;

	FT_Set_Pixel_Sizes(currentFace_, 0, size);
}

void GlTextRenderer::setAlignment(Alignment alignment)
{
	alignment_ = alignment;
}

float GlTextRenderer::write(float xInit, float yInit, float sx, float sy, const std::string& text, float alpha)
{
	const char* p = text.c_str();

	float maxW = 0.0f;
	float maxH = 0.0f;

	for (; *p; p++)
	{
		if (FT_Load_Char(currentFace_, *p, FT_LOAD_RENDER))
			continue;

		FT_GlyphSlot g = currentFace_->glyph;

		maxW += float(g->bitmap.width) * sx;

		float h = float(g->bitmap.rows)  * sy;
		if (h > maxH)
			maxH = h;
	}

	p = text.c_str();

	float xx = 0.0f;
	float yy = -maxH * 0.5f;

	switch (alignment_)
	{
	case Alignment::LEFT:
		xx = -maxW;
		break;

	case Alignment::CENTER:
		xx = -maxW * 0.5f;
		break;

	case Alignment::RIGHT:
		xx = 0.0f;
		break;
	}

	fontShader_->bind();
	fontShader_->setUniform("u_alpha", alpha);

	for (; *p; p++)
	{
		if (FT_Load_Char(currentFace_, *p, FT_LOAD_RENDER))
			continue;

		FT_GlyphSlot g = currentFace_->glyph;

		float x = xx + float(g->bitmap_left) * sx;
		float y = -yy - float(g->bitmap_top) * sy;
		float w = float(g->bitmap.width) * sx;
		float h = float(g->bitmap.rows)  * sy;

		glEnable(GL_TEXTURE);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, glyphTextureId_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		float quad[16] = {
			x,     -y - h, 0.0, 1.0,
			x + w, -y - h, 1.0, 1.0,
			x + w, -y,     1.0, 0.0,
			x,     -y,     0.0, 0.0
		};

		xx += float(g->advance.x >> 6) * sx;
		yy += float(g->advance.y >> 6) * sy;

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glBindBuffer(GL_ARRAY_BUFFER, glyphVBOId_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_DYNAMIC_DRAW);
		glVertexPointer(4, GL_FLOAT, 0, nullptr);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	fontShader_->release();

	return maxW / sx;
}
