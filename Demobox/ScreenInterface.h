#pragma once

#include <string>

namespace Demobox
{
	class GlContext;

	class ScreenInterface
	{
	public:
		virtual void attach(GlContext*) = 0;
		virtual void detach(GlContext*) = 0;

		virtual void initialize() = 0;
		virtual void display(GlContext*) = 0;

		virtual std::string breadcrumb() = 0;
	};
}
