#pragma once

#include <string>

namespace Demobox
{
	class GlTexture;

	class MediaItem
	{
	public:
		MediaItem();
		~MediaItem();

		std::string title()  const;
		std::string author() const;

		void setScreenshot(GlTexture* screenshot);
		GlTexture*  screenshot();

		virtual void start() = 0;

	protected:
		std::string title_;
		std::string author_;

		GlTexture*  screenshot_;
	};
}
