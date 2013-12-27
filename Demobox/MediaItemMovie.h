#pragma once

#include "MediaItem.h"
#include <string>

namespace Demobox
{
	class MediaItemMovie
		: public MediaItem
	{
	public:
		MediaItemMovie(const std::string& path);
		~MediaItemMovie();

		std::string filename();
		std::string path();

		void setGenre(const std::string& genre);
		std::string genre() const;

		void start();

	private:
		std::string path_;
		std::string filename_;

		std::string genre_;
	};
}
