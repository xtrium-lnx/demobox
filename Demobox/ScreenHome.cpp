#include "ScreenHome.h"
#include "GlContext.h"
#include "GlTextRenderer.h"

#include "Application.h"
#include "Database.h"
#include "MediaItemDemo.h"
#include "ScreenFlow.h"
#include "ScreenList.h"
#include "ScreenSelector.h"

using namespace Demobox;

ScreenHome::ScreenHome()
	: position_(0),
	  subPosition_(0),
	  animCurrent_(0),
	  animNext_(0),
	  animMu_(1.0),
	  subAnimCurrent_(0),
	  subAnimNext_(0),
	  subAnimMu_(1.0)
{ // Populate the home screen...
	{ // Demo menu and submenu
		std::map<std::string, ScreenInterface*> demoScreens;
		demoScreens["All"] = new ScreenFlow("Demos", Database::instance()->getDemos());
		demoScreens["By group"] = new ScreenSelector(ScreenSelector::SelectionMode::DEMOS, ScreenSelector::SelectionFilter::BY_GROUP_OR_AUTHOR);
		demoScreens["By category"] = new ScreenSelector(ScreenSelector::SelectionMode::DEMOS, ScreenSelector::SelectionFilter::BY_CATEGORY);
		demoScreens["By demoparty"] = new ScreenSelector(ScreenSelector::SelectionMode::DEMOS, ScreenSelector::SelectionFilter::BY_PARTY);
		demoScreens["By platform"] = new ScreenSelector(ScreenSelector::SelectionMode::DEMOS, ScreenSelector::SelectionFilter::BY_PLATFORM);
		children_["Demos"] = demoScreens;
	}

	{ // Music menu and submenu
		std::map<std::string, ScreenInterface*> musicScreens;
		musicScreens["All"] = new ScreenList("Music", Database::instance()->getMusicSongs());
		musicScreens["By artist"] = new ScreenSelector(ScreenSelector::SelectionMode::MUSIC, ScreenSelector::SelectionFilter::BY_GROUP_OR_AUTHOR);
		//musicScreens["By album"] = nullptr;
		children_["Music"] = musicScreens;
	}

	{ // Movie menu and submenu
		std::map<std::string, ScreenInterface*> movieScreens;
		movieScreens["All"] = new ScreenList("Movies", Database::instance()->getMovies());
		movieScreens["By genre"] = new ScreenSelector(ScreenSelector::SelectionMode::VIDEOS, ScreenSelector::SelectionFilter::BY_CATEGORY);
		children_["Movies"] = movieScreens;
	}

	{ // Settings - no submenu here, so we fake one.
		std::map<std::string, ScreenInterface*> settingsScreens;
		settingsScreens[""] = nullptr; // No submenu = 1 single, invisible item
		children_["Settings"] = settingsScreens;
	}
}

ScreenHome::~ScreenHome()
{
}

void ScreenHome::attach(GlContext* ctx)
{
	ctx->OnKeyPressed.connect([=](int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		{
			ctx->RequestWindowClose();
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_ENTER)
		{
			int i = 0;
			for (auto it = children_.begin(); it != children_.end(); it++)
			{
				if (i == position_)
				{
					int j = 0;
					for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
					{
						if (j == subPosition_)
						{
							if (it2->second)
								Application::instance()->pushScreen(it2->second);
						}
						++j;
					}
				}
				++i;
			}
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_LEFT)
		{
			animCurrent_ = position_;
			--position_;
			if (position_ < 0)
				position_ = 0;
			animNext_ = position_;
			animMu_ = 0.0;

			subPosition_ = 0;
			subAnimCurrent_ = 0;
			subAnimNext_ = 0;
			subAnimMu_ = 1.0;
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT)
		{
			animCurrent_ = position_;
			++position_;
			if (position_ >= children_.size())
				position_ = children_.size() - 1;
			animNext_ = position_;
			animMu_ = 0.0;

			subPosition_ = 0;
			subAnimCurrent_ = 0;
			subAnimNext_ = 0;
			subAnimMu_ = 1.0;
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_UP)
		{
			subAnimCurrent_ = subPosition_;
			--subPosition_;
			if (subPosition_ < 0)
				subPosition_ = 0;
			subAnimNext_ = subPosition_;
			subAnimMu_ = 0.0;
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
		{
			int size = 0;
			int i = 0;
			for (auto it = children_.begin(); it != children_.end(); it++)
			{
				if (i == position_)
				{
					size = it->second.size();
					break;
				}
				++i;
			}
			subAnimCurrent_ = subPosition_;
			++subPosition_;
			if (subPosition_ >= size)
				subPosition_ = size - 1;
			subAnimNext_ = subPosition_;
			subAnimMu_ = 0.0;
		}
	});
}

void ScreenHome::detach(GlContext* ctx)
{
	ctx->OnKeyPressed.disconnectAll();
}

void ScreenHome::initialize()
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

void ScreenHome::display(GlContext* ctx)
{
	float mu = -cos(animMu_ * 3.1415926) * 0.5 + 0.5;
	float animPosition = (1.0 - mu) * double(animCurrent_) + mu * double(animNext_);

	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::CENTER);
	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 256);

	int i = 0;
	for (auto it = children_.begin(); it != children_.end(); it++)
	{
		glPushMatrix();

		float animDelta = float(i) - float(animPosition);
		float alpha = 1.0f / (1.0 + sqrtf(fabs(float(animDelta) * 5.0f)));

		glTranslated(animDelta * 1.3f, 0.0, abs(animDelta));

		GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0f / 940.0f, 1.0f / 940.0f, it->first, alpha * 0.9f);

		++i;
		glPopMatrix();
	}

	std::map<std::string, ScreenInterface*> currentSubmenu;
	float xDelta = 0.0f;
	i = 0;
	for (auto it = children_.begin(); it != children_.end(); it++)
	{
		float xd = float(i) - float(animPosition);
		if (i == position_)
		{
			currentSubmenu = it->second;
			xDelta = float(i) - float(animPosition);
			break;
		}
		++i;
	}

	if (currentSubmenu.size() > 0)
	{
		float subMu = -cos(subAnimMu_ * 3.1415926f) * 0.5 + 0.5;
		float subAnimPosition = (1.0f - subMu) * float(subAnimCurrent_) + subMu * float(subAnimNext_);

		i = 0;
		GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 112);
		for (auto it = currentSubmenu.begin(); it != currentSubmenu.end(); it++)
		{
			glPushMatrix();

			glTranslated(xDelta * 1.3f, -0.25, fabs(xDelta) * 0.3);

			float subAnimDelta = float(i) - float(subAnimPosition);
			float alpha = (1.0f - fabs(xDelta)) / (1.0 + fabs(float(subAnimDelta) * 4.0f));


			if (subAnimDelta <= -1.0f)
			{
				subAnimDelta = subAnimDelta - 1.25f;
			}
			else if (subAnimDelta <= 0.0f && subAnimDelta > -1.0f)
			{
				subAnimDelta = subAnimDelta * 2.25f;
			}

			glTranslated(0.0, -subAnimDelta * 0.2f, 0.0);

			GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::CENTER);
			GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0f / 940.0f, 1.0f / 940.0f, it->first, alpha * 0.9f);

			++i;
			glPopMatrix();
		}
	}

	animMu_ += ctx->delta() * 6.0;
	if (animMu_ > 1.0)
		animMu_ = 1.0;

	subAnimMu_ += ctx->delta() * 6.0;
	if (subAnimMu_ > 1.0)
		subAnimMu_ = 1.0;
}
