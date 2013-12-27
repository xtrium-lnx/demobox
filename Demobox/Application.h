#pragma once

#include <string>
#include <vector>

#define WIDTH  800
#define HEIGHT 480

namespace Demobox
{
	class ScreenInterface;
	class GlContext;
	class GlShader;
	class GlTexture;

	class Application
	{
	public:
		static Application* instance();
		~Application();

		void pushScreen(ScreenInterface* scr);
		void popScreen();

		void showModal(const std::string& title, const std::string& text1, const std::string& text2);
		void hideModal();

		void forceShowWindow();
		void forceHideWindow();

		double now();
		double delta();

		int run();

	private:
		Application();
		static Application* instance_;

		std::vector<ScreenInterface*> screenStack_;
		GlContext*            context_;

		GlShader*             backdrop_;
		GlTexture*            backdropBubbles_;
		GlTexture*            splashScreen_;

		std::string           modalTitle_;
		std::string           modalText1_;
		std::string           modalText2_;
		bool                  modalIsDisplayed_;
		float                 modalAlpha_;

		bool                  windowShouldForceShow_;
		bool                  windowShouldForceHide_;
	};
}

