#pragma once

#include "ScreenInterface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Demobox
{
	class ScreenSelector
		: public ScreenInterface
	{
	public:
		enum class SelectionMode
		{
			DEMOS = 0,
			MUSIC,
			VIDEOS
		};

		enum class SelectionFilter
		{
			BY_GROUP_OR_AUTHOR,
			BY_ALBUM,
			BY_PARTY,
			BY_YEAR,
			BY_PLATFORM,
			BY_CATEGORY,
			NO_FILTER
		};

		ScreenSelector(SelectionMode mode, SelectionFilter filter);
		~ScreenSelector();

		void attach(GlContext *ctx);
		void detach(GlContext *ctx);

		void initialize();
		void display(GlContext *ctx);

		std::string breadcrumb();

	private:
		SelectionMode mode_;
		SelectionFilter filter_;

		std::string title_;

		std::vector<std::string> selectorItems_;

		int32_t position_;
		int32_t animCurrent_, animNext_;
		float animMu_;

		void generateBreadcrumb_();
	};
}

