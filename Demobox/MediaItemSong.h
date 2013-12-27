#pragma once

#include "MediaItem.h"
#include <string>

namespace Demobox
{
	class MediaItemSong
		: public MediaItem
	{
	public:
		MediaItemSong(const std::string& path);
		~MediaItemSong();

		std::string genre() const;
		std::string path() const;

		void start();

	private:
		std::string genre_;
		std::string path_;
	};
}
