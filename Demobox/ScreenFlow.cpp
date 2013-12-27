#include "ScreenFlow.h"

#include "Application.h"
#include "GlContext.h"
#include "GlTextRenderer.h"
#include "GlTexture.h"
#include "MediaItem.h"
#include "ScreenMediaPlayer.h"

#include <GL/glew.h>

using namespace Demobox;

ScreenFlow::ScreenFlow(const std::string& breadcrumb)
	: position_(0),
	  animCurrent_(0),
	  animNext_(0),
	  animMu_(1.0)
{
	breadcrumb_ = breadcrumb;
}

ScreenFlow::ScreenFlow(const std::string& breadcrumb, const std::vector<MediaItem*>& items)
	: position_(0),
	  animCurrent_(0),
	  animNext_(0),
	  animMu_(1.0)
{
	breadcrumb_ = breadcrumb;
	addMediaItems(items);
}


ScreenFlow::~ScreenFlow()
{
}

void ScreenFlow::attach(GlContext *ctx)
{
	ctx->OnKeyPressed.connect([&](int key, int scancode, int action, int mods) {
		if (action != GLFW_PRESS && action != GLFW_REPEAT)
			return;

		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			Application::instance()->popScreen();
			break;

		case GLFW_KEY_ENTER:
			items_[position_]->start();
			ScreenMediaPlayer::setNowPlaying(items_[position_]->title(), items_[position_]->author());
			Application::instance()->pushScreen(new ScreenMediaPlayer());
			break;

		case GLFW_KEY_LEFT:
			animCurrent_ = position_;
			--position_;
			if (position_ < 0)
				position_ = 0;
			animNext_ = position_;
			animMu_ = 0.0;
			break;

		case GLFW_KEY_RIGHT:
			animCurrent_ = position_;
			++position_;
			if (position_ >= items_.size())
				position_ = items_.size() - 1;
			animNext_ = position_;
			animMu_ = 0.0;
			break;

		default:
			break;
		}
	});

	ctx->OnMouseWheel.connect([&](double xOfs, double yOfs) {
		if (yOfs < 0.0)
		{
			animCurrent_ = position_;
			++position_;
			if (position_ >= items_.size())
				position_ = items_.size() - 1;
			animNext_ = position_;
			animMu_ = 0.0;
		}
		else if (yOfs > 0.0)
		{
			animCurrent_ = position_;
			--position_;
			if (position_ < 0)
				position_ = 0;
			animNext_ = position_;
			animMu_ = 0.0;
		}
	});
}

void ScreenFlow::detach(GlContext *ctx)
{
	ctx->OnKeyPressed.disconnectAll();
	ctx->OnMouseWheel.disconnectAll();
}

void ScreenFlow::initialize()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, WIDTH / float(HEIGHT), 0.01f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, -0.25, 2.5, 0.0, -0.25, 0.0, 0.0, 1.0, 0.0);

	glEnable(GL_TEXTURE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ScreenFlow::display(GlContext *ctx)
{
	glPushMatrix();
	glTranslated(-1.85, 0.65, 0.0);
	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 192);
	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::RIGHT);
	GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0 / 960.0f, 1.0 / 960.0f, breadcrumb(), 0.9f);
	glPopMatrix();

	//

	int32_t leftLimit = position_ - 4; leftLimit = (leftLimit < 0) ? 0 : leftLimit;
	int32_t rightLimit = position_ + 5; rightLimit = (rightLimit >= items_.size()) ? items_.size() : rightLimit;

	double mu = -cos(animMu_ * 3.1415926) * 0.5 + 0.5;
	double animPosition = (1.0 - mu) * double(animCurrent_) + mu * double(animNext_);

	for (int32_t i = leftLimit; i < rightLimit; i++)
	{
		glPushMatrix();

		double animDelta = double(i) - double(animPosition);

		glTranslated(animDelta, 0.0, 0.0);

		double rotMax = fmax(-1.0, fmin(1.0, animDelta));

		{
			glTranslated(animDelta * 0.25, 0.0, -abs(animDelta));
			glRotatef(-rotMax * 45.0f, 0.0f, 1.0f, 0.0f);
		}

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		GlTexture* demoScreenshot = items_[i]->screenshot();
		if (demoScreenshot)
			demoScreenshot->bind(0);

		glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f / (1.0 + sqrtf(fabs(float(animDelta) * 5.0f))));
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.96f, -0.6f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(0.96f, -0.6f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(0.96f, 0.6f);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.96f, 0.6f);
		glEnd();

		if (demoScreenshot)
			demoScreenshot->release();

		glPopMatrix();
	}

	//

	glTranslated(-1.1, -0.8, 0.0);

	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 96);
	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::RIGHT);
	GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0f / 960.0f, 1.0f / 960.0f, std::string("Title: ").append(items_[position_]->title()), 0.9f);
	glTranslated(0.0, -0.15, 0.0);
	GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0f / 960.0f, 1.0f / 960.0f, std::string("Author: ").append(items_[position_]->author()), 0.9f);

	animMu_ += ctx->delta() * 5.0;
	if (animMu_ > 1.0)
		animMu_ = 1.0;
}

void ScreenFlow::addMediaItem(MediaItem* item)
{
	items_.push_back(item);
}

void ScreenFlow::addMediaItems(const std::vector<MediaItem*>& items)
{
	items_.insert(items_.end(), items.begin(), items.end());
}

std::string ScreenFlow::breadcrumb()
{
	return breadcrumb_;
}
