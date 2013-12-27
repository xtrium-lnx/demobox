#pragma once

#include <cstdint>
#include <vector>
#include "ScreenInterface.h"

namespace Demobox
{
	class GlContext;
	class MediaItem;

	class ScreenFlow
		: public ScreenInterface
	{
	public:
		ScreenFlow(const std::string& breadcrumb);
		ScreenFlow(const std::string& breadcrumb, const std::vector<MediaItem*>& items);
		~ScreenFlow();

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
