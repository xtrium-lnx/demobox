#include "ScreenList.h"

#include "Application.h"
#include "GlContext.h"
#include "GlTextRenderer.h"
#include "GlTexture.h"
#include "MediaItem.h"
#include "ScreenMediaPlayer.h"

#include <GL/glew.h>

using namespace Demobox;

ScreenList::ScreenList(const std::string& breadcrumb)
	: position_(0),
	  animCurrent_(0),
	  animNext_(0),
	  animMu_(1.0)
{
	breadcrumb_ = breadcrumb;
}

ScreenList::ScreenList(const std::string& breadcrumb, const std::vector<MediaItem*>& items)
	: position_(0),
	  animCurrent_(0),
	  animNext_(0),
	  animMu_(1.0)
{
	breadcrumb_ = breadcrumb;
	addMediaItems(items);
}

ScreenList::~ScreenList()
{
}

void ScreenList::attach(GlContext *ctx)
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

		case GLFW_KEY_UP:
			animCurrent_ = position_;
			--position_;
			if (position_ < 0)
				position_ = 0;
			animNext_ = position_;
			animMu_ = 0.0;
			break;

		case GLFW_KEY_DOWN:
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

void ScreenList::detach(GlContext *ctx)
{
	ctx->OnKeyPressed.disconnectAll();
	ctx->OnMouseWheel.disconnectAll();
}

void ScreenList::initialize()
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

void ScreenList::display(GlContext *ctx)
{
	glPushMatrix();
	glTranslated(-1.85, 0.9, 0.0);
	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 192);
	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::RIGHT);
	GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0 / 960.0f, 1.0 / 960.0f, breadcrumb(), 0.9f);
	glPopMatrix();

	//

	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 96);
	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::CENTER);
	glTranslated(0.0, 0.0, 0.0);

	double mu = -cos(animMu_ * 3.1415926) * 0.5 + 0.5;
	double animPosition = (1.0 - mu) * double(animCurrent_) + mu * double(animNext_);

	int i = 0;
	for (MediaItem* s : items_)
	{
		if (i < position_ - 5 || i > position_ + 7)
		{
			++i;
			continue;
		}

		glPushMatrix();

		float animDelta = float(i) - float(animPosition);
		float alpha = 1.0f / (1.0 + sqrtf(fabs(float(animDelta) * 5.0f)));

		std::string mediaText;
		mediaText.append(s->author());
		mediaText.append(" - ");
		mediaText.append(s->title());
		mediaText.resize(64);

		glTranslated(0.0, -0.15 * animDelta, 0.0);
		GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0 / 960.0f, 1.0 / 960.0f, mediaText, 0.9f * alpha);

		glPopMatrix();
		++i;
	}

	animMu_ += ctx->delta() * 6.0;
	if (animMu_ > 1.0)
		animMu_ = 1.0;
}

void ScreenList::addMediaItem(MediaItem* item)
{
	items_.push_back(item);
}

void ScreenList::addMediaItems(const std::vector<MediaItem*>& items)
{
	items_.insert(items_.end(), items.begin(), items.end());
}

std::string ScreenList::breadcrumb()
{
	return breadcrumb_;
}
