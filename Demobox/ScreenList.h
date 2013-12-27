#pragma once

#include <cstdint>
#include <vector>
#include "ScreenInterface.h"

namespace Demobox
{
	class GlContext;
	class MediaItem;

	class ScreenList
		: public ScreenInterface
	{
	public:
		ScreenList(const std::string& breadcrumb);
		ScreenList(const std::string& breadcrumb, const std::vector<MediaItem*>& items);
		~ScreenList();

		void attach(GlContext *ctx);
		void detach(GlContext *ctx);

		void initialize();
		void display(GlContext *ctx);

		void addMediaItem(MediaItem* item);
		void addMediaItems(const std::vector<MediaItem*>& items);

		std::string breadcrumb();

	private:
		std::vector<MediaItem*> items_;
		int32_t                 position_;
		int32_t                 animCurrent_, animNext_;
		double                  animMu_;

		std::string             breadcrumb_;
	};
}
