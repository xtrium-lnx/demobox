#pragma once

#include "ScreenInterface.h"

#include <cstdint>
#include <string>

namespace Demobox
{
	class ScreenMediaPlayer
		: public ScreenInterface
	{
	public:
		ScreenMediaPlayer();
		~ScreenMediaPlayer();

		void attach(GlContext* ctx);
		void detach(GlContext* ctx);

		void initialize();
		void display(GlContext* ctx);

		std::string breadcrumb() { return std::string("Now playing"); }

		static void setNowPlaying(const std::string& title, const std::string& author);

		static double npStartTime_;

	private:
		uint32_t videoTextureId_;

		static std::string npTitle_;
		static std::string npAuthor_;
	};
}
