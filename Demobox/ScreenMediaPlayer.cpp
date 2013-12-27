#include "ScreenMediaPlayer.h"
#include "Application.h"
#include "GlContext.h"
#include "GlTextRenderer.h"
#include "MediaInterface.h"

#include <iostream>

using namespace Demobox;

std::string ScreenMediaPlayer::npTitle_     = std::string("");
std::string ScreenMediaPlayer::npAuthor_    = std::string("");
double      ScreenMediaPlayer::npStartTime_ = 0.0;

ScreenMediaPlayer::ScreenMediaPlayer()
{
	glGenTextures(1, &videoTextureId_);
	glBindTexture(GL_TEXTURE_2D, videoTextureId_);
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, MediaInterface::instance()->videoImage_);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);
}

ScreenMediaPlayer::~ScreenMediaPlayer()
{
	glDeleteTextures(1, &videoTextureId_);
}

void ScreenMediaPlayer::setNowPlaying(const std::string& title, const std::string& author)
{
	npTitle_ = title;
	npAuthor_ = author;

	npStartTime_ = Application::instance()->now();
}

void ScreenMediaPlayer::attach(GlContext* ctx)
{
	ctx->OnKeyPressed.connect([&](int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		{
			Application::instance()->popScreen();
		}
	});
}

void ScreenMediaPlayer::detach(GlContext* ctx)
{
	ctx->OnKeyPressed.disconnectAll();
}

void ScreenMediaPlayer::initialize()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, WIDTH / float(HEIGHT), 0.01f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 2.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glEnable(GL_TEXTURE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ScreenMediaPlayer::display(GlContext* ctx)
{
	if (MediaInterface::instance()->hasVideo())
	{
		MediaInterface::instance()->handleVideo([&](unsigned char *data, uint32_t width, uint32_t height) {
			glBindTexture(GL_TEXTURE_2D, videoTextureId_);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glBindTexture(GL_TEXTURE_2D, videoTextureId_);
			glBegin(GL_QUADS);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f);
				glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		});
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE);
	
	glClear(GL_DEPTH_BUFFER_BIT);

	float alpha = 1.0f;

	if (MediaInterface::instance()->hasVideo())
	{
		double timeSinceStarted = Application::instance()->now() - npStartTime_;

		timeSinceStarted = (timeSinceStarted - 3.0) / 3.0;

		if (timeSinceStarted < 0.0)
			timeSinceStarted = 0.0;

		if (timeSinceStarted > 1.0)
			timeSinceStarted = 1.0;

		timeSinceStarted = 1.0 - timeSinceStarted;

		timeSinceStarted = -cos(timeSinceStarted * 3.14159265) * 0.5 + 0.5;
		alpha = timeSinceStarted;
	}

	glPushMatrix();
	glTranslated(-1.85, 0.9, 0.0);
	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 192);
	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::RIGHT);
	GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0 / 960.0f, 1.0 / 960.0f, "Now playing...", alpha);
	glPopMatrix();

	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, alpha * 0.12f);
	glVertex2f(-3.0f, -0.95f);
	glVertex2f(3.0f, -0.95f);
	glVertex2f(3.0f, -0.35f);
	glVertex2f(-3.0f, -0.35f);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(0.1f, 0.15f, 0.25f, alpha * 0.3f);
	glVertex2f(-3.0f, -0.94f);
	glVertex2f(3.0f, -0.94f);
	glVertex2f(3.0f, -0.36f);
	glVertex2f(-3.0f, -0.36f);
	glEnd();

	glTranslated(-1.7, -0.52, 0.0);
	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 192);
	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::RIGHT);
	GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0 / 960.0f, 1.0 / 960.0f, npTitle_, alpha * 0.9f);

	glTranslated(0.015, -0.22, 0.0);
	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 96);
	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::RIGHT);
	GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0 / 960.0f, 1.0 / 960.0f, npAuthor_, alpha * 0.9f);
}
