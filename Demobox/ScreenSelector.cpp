#include "ScreenSelector.h"

#include "Application.h"
#include "Database.h"
#include "GlContext.h"
#include "GlTextRenderer.h"
#include "ScreenFlow.h"
#include "ScreenList.h"

#include <iostream>
#include <GL/glew.h>

using namespace Demobox;

ScreenSelector::ScreenSelector(SelectionMode mode, SelectionFilter filter)
	: mode_(mode),
	  filter_(filter),
	  animCurrent_(0),
	  animNext_(0),
	  animMu_(1.0),
	  position_(0)
{
	// Construct title / breadcrumb
	generateBreadcrumb_();

	// populate
	if (mode_ == SelectionMode::DEMOS && filter_ == SelectionFilter::BY_GROUP_OR_AUTHOR)
		selectorItems_ = Database::instance()->getDemoGroups();

	if (mode_ == SelectionMode::DEMOS && filter_ == SelectionFilter::BY_PARTY)
		selectorItems_ = Database::instance()->getDemoParties();

	if (mode_ == SelectionMode::DEMOS && filter_ == SelectionFilter::BY_CATEGORY)
		selectorItems_ = Database::instance()->getDemoCategories();

	if (mode_ == SelectionMode::DEMOS && filter_ == SelectionFilter::BY_PLATFORM)
		selectorItems_ = Database::instance()->getDemoPlatforms();

	if (mode_ == SelectionMode::MUSIC && filter_ == SelectionFilter::BY_GROUP_OR_AUTHOR)
		selectorItems_ = Database::instance()->getMusicArtists();

	if (mode_ == SelectionMode::VIDEOS && filter_ == SelectionFilter::BY_CATEGORY)
		selectorItems_ = Database::instance()->getMovieGenres();
}

ScreenSelector::~ScreenSelector()
{
	
}

void ScreenSelector::generateBreadcrumb_()
{
	title_ = "Selection : ";

	switch (mode_)
	{
	case SelectionMode::DEMOS:
		title_.append("Demos - ");
		break;
	case SelectionMode::MUSIC:
		title_.append("Music - ");
		break;
	case SelectionMode::VIDEOS:
		title_.append("Movies - ");
		break;
	}

	switch (filter_)
	{
	case SelectionFilter::BY_ALBUM:
		title_.append("by album");
		break;
	case SelectionFilter::BY_CATEGORY:
		if (mode_ == SelectionMode::DEMOS)
			title_.append("by category");
		else
			title_.append("by genre");
		break;
	case SelectionFilter::BY_GROUP_OR_AUTHOR:
		if (mode_ == SelectionMode::DEMOS)
			title_.append("by group");
		else
			title_.append("by author");
		break;
	case SelectionFilter::BY_PARTY:
		title_.append("by party");
		break;
	case SelectionFilter::BY_PLATFORM:
		title_.append("by platform");
		break;
	case SelectionFilter::BY_YEAR:
		title_.append("by year");
		break;
	case SelectionFilter::NO_FILTER:
		title_.append("all");
		break;
	}
}

void ScreenSelector::attach(GlContext *ctx)
{
	generateBreadcrumb_();

	ctx->OnKeyPressed.connect([&](int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS && key == GLFW_KEY_ENTER)
		{
			if (mode_ == SelectionMode::DEMOS && filter_ == SelectionFilter::BY_GROUP_OR_AUTHOR)
			{
				ScreenFlow* demoScreen = new ScreenFlow("Demos", Database::instance()->getDemosByGroup(selectorItems_[position_]));

				title_.append(" (");
				title_.append(selectorItems_[position_]);
				title_.append(")");
				Application::instance()->pushScreen(demoScreen);
			}

			if (mode_ == SelectionMode::DEMOS && filter_ == SelectionFilter::BY_PARTY)
			{
				ScreenFlow* demoScreen = new ScreenFlow("Demos", Database::instance()->getDemosByParty(selectorItems_[position_]));

				title_.append(" (");
				title_.append(selectorItems_[position_]);
				title_.append(")");
				Application::instance()->pushScreen(demoScreen);
			}

			if (mode_ == SelectionMode::DEMOS && filter_ == SelectionFilter::BY_CATEGORY)
			{
				MediaItemDemo::Category cat;

				if (selectorItems_[position_] == "1k (or less) intro")
					cat = MediaItemDemo::Category::INTRO_1K_OR_LESS;
				if (selectorItems_[position_] == "4k intro")
					cat = MediaItemDemo::Category::INTRO_4K;
				if (selectorItems_[position_] == "64k intro")
					cat = MediaItemDemo::Category::INTRO_64K;
				if (selectorItems_[position_] == "Demo")
					cat = MediaItemDemo::Category::DEMO;
				if (selectorItems_[position_] == "Game")
					cat = MediaItemDemo::Category::GAME;
				if (selectorItems_[position_] == "Musicdisk")
					cat = MediaItemDemo::Category::MUSICDISK;
				if (selectorItems_[position_] == "Wild / animation")
					cat = MediaItemDemo::Category::WILD;

				ScreenFlow* demoScreen = new ScreenFlow("Demos", Database::instance()->getDemosByCategory(cat));

				title_.append(" (");
				title_.append(selectorItems_[position_]);
				title_.append(")");
				Application::instance()->pushScreen(demoScreen);
			}

			if (mode_ == SelectionMode::DEMOS && filter_ == SelectionFilter::BY_PLATFORM)
			{
				MediaItemDemo::Platform platform;

				if (selectorItems_[position_] == "Windows")
					platform = MediaItemDemo::Platform::WINDOWS;
				if (selectorItems_[position_] == "Linux")
					platform = MediaItemDemo::Platform::LINUX;
				if (selectorItems_[position_] == "Mac OS X")
					platform = MediaItemDemo::Platform::MAC_OS_X;
				if (selectorItems_[position_] == "MS-DOS")
					platform = MediaItemDemo::Platform::MS_DOS;
				if (selectorItems_[position_] == "Web browser")
					platform = MediaItemDemo::Platform::WEB_BROWSER;
				if (selectorItems_[position_] == "Amiga OCS/ECS")
					platform = MediaItemDemo::Platform::AMIGA_OCS_ECS;
				if (selectorItems_[position_] == "Amiga AGA")
					platform = MediaItemDemo::Platform::AMIGA_AGA;
				if (selectorItems_[position_] == "Atari ST / STe / STf")
					platform = MediaItemDemo::Platform::ATARI_ST;
				if (selectorItems_[position_] == "Atari Falcon")
					platform = MediaItemDemo::Platform::ATARI_FALCON;
				if (selectorItems_[position_] == "Commodore 64")
					platform = MediaItemDemo::Platform::C64;
				if (selectorItems_[position_] == "Atari VCS")
					platform = MediaItemDemo::Platform::ATARI_VCS;
				if (selectorItems_[position_] == "Amstrad CPC")
					platform = MediaItemDemo::Platform::AMSTRAD_CPC;
				if (selectorItems_[position_] == "ZX Spectrum")
					platform = MediaItemDemo::Platform::ZX_SPECTRUM;
				if (selectorItems_[position_] == "mIRC")
					platform = MediaItemDemo::Platform::MIRC;
				if (selectorItems_[position_] == "Nintendo NES")
					platform = MediaItemDemo::Platform::NINTENDO_NES;
				if (selectorItems_[position_] == "Nintendo SuperNES")
					platform = MediaItemDemo::Platform::NINTENDO_SUPER_NES;
				if (selectorItems_[position_] == "Nintendo 64")
					platform = MediaItemDemo::Platform::NINTENDO_64;
				if (selectorItems_[position_] == "Nintendo GameCube")
					platform = MediaItemDemo::Platform::NINTENDO_GAMECUBE;
				if (selectorItems_[position_] == "Nintendo Wii")
					platform = MediaItemDemo::Platform::NINTENDO_WII;
				if (selectorItems_[position_] == "Sega Master System")
					platform = MediaItemDemo::Platform::SEGA_MASTER_SYSTEM;
				if (selectorItems_[position_] == "Sega Megadrive / Genesis")
					platform = MediaItemDemo::Platform::SEGA_MEGADRIVE;
				if (selectorItems_[position_] == "Sega Gamegear")
					platform = MediaItemDemo::Platform::SEGA_GAMEGEAR;
				if (selectorItems_[position_] == "Sony Playstation")
					platform = MediaItemDemo::Platform::SONY_PSX;
				if (selectorItems_[position_] == "Sony Playstation 2")
					platform = MediaItemDemo::Platform::SONY_PS2;
				if (selectorItems_[position_] == "Sony Playstation 3")
					platform = MediaItemDemo::Platform::SONY_PS3;
				if (selectorItems_[position_] == "Microsoft Xbox")
					platform = MediaItemDemo::Platform::MICROSOFT_XBOX;
				if (selectorItems_[position_] == "Microsoft Xbox 360")
					platform = MediaItemDemo::Platform::MICROSOFT_XBOX_360;

				ScreenFlow* demoScreen = new ScreenFlow("Demos", Database::instance()->getDemosByPlatform(platform));

				title_.append(" (");
				title_.append(selectorItems_[position_]);
				title_.append(")");
				Application::instance()->pushScreen(demoScreen);
			}

			if (mode_ == SelectionMode::MUSIC && filter_ == SelectionFilter::BY_GROUP_OR_AUTHOR)
			{
				ScreenList* musicScreen = new ScreenList("Music", Database::instance()->getMusicSongsByArtist(selectorItems_[position_]));

				title_.append(" (");
				title_.append(selectorItems_[position_]);
				title_.append(")");
				Application::instance()->pushScreen(musicScreen);
			}

			if (mode_ == SelectionMode::VIDEOS && filter_ == SelectionFilter::BY_CATEGORY)
			{
				ScreenList* videoScreen = new ScreenList("Movies", Database::instance()->getMoviesByGenre(selectorItems_[position_]));

				title_.append(" (");
				title_.append(selectorItems_[position_]);
				title_.append(")");
				Application::instance()->pushScreen(videoScreen);
			}
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		{
			Application::instance()->popScreen();
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_UP)
		{
			animCurrent_ = position_;
			--position_;
			if (position_ < 0)
				position_ = 0;
			animNext_ = position_;
			animMu_ = 0.0;
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
		{
			animCurrent_ = position_;
			++position_;
			if (position_ >= selectorItems_.size())
				position_ = selectorItems_.size() - 1;
			animNext_ = position_;
			animMu_ = 0.0;
		}
	});
}

void ScreenSelector::detach(GlContext *ctx)
{
	ctx->OnKeyPressed.disconnectAll();
}

void ScreenSelector::initialize()
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

void ScreenSelector::display(GlContext *ctx)
{
	glPushMatrix();
	glTranslated(-1.85, 0.9, 0.0);
	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 192);
	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::RIGHT);
	GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0 / 960.0f, 1.0 / 960.0f, title_, 0.9f);
	glPopMatrix();

	GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 96);
	GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::CENTER);
	glTranslated(0.0, 0.0, 0.0);

	double mu = -cos(animMu_ * 3.1415926) * 0.5 + 0.5;
	double animPosition = (1.0 - mu) * double(animCurrent_) + mu * double(animNext_);

	int i = 0;
	for (std::string& item : selectorItems_)
	{
		if (i < position_ - 3 || i > position_ + 4)
		{
			++i;
			continue;
		}

		glPushMatrix();

		float animDelta = float(i) - float(animPosition);
		float alpha = 1.0f / (1.0 + sqrtf(fabs(float(animDelta) * 5.0f)));

		glTranslated(0.0, -0.15 * animDelta, 0.0);
		GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0 / 960.0f, 1.0 / 960.0f, item, 0.9f * alpha);

		glPopMatrix();
		++i;
	}

	animMu_ += ctx->delta() * 6.0;
	if (animMu_ > 1.0)
		animMu_ = 1.0;
}

std::string ScreenSelector::breadcrumb()
{
	return title_;
}
