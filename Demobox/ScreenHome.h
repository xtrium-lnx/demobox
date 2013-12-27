#pragma once

#include "ScreenInterface.h"

#include <cstdint>
#include <map>

namespace Demobox
{
	class ScreenHome
		: public ScreenInterface
	{
	public:
		ScreenHome();
		~ScreenHome();

		void attach(GlContext* ctx);
		void detach(GlContext* ctx);

		void initialize();
		void display(GlContext* ctx);

		std::string breadcrumb() { return std::string("Home"); }

	private:
		int32_t                                                  position_, subPosition_;
		std::map< std::string, std::map<std::string, ScreenInterface*> > children_;

		int                                                      animCurrent_, animNext_;
		double                                                   animMu_;

		int                                                      subAnimCurrent_, subAnimNext_;
		double                                                   subAnimMu_;
	};
}
