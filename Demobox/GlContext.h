#pragma once

#include <cstdint>
#include <string>

#define GLEW_STATIC
#define GLEW_EXPERIMENTAL
#include <gl/glew.h>
#include <glfw/glfw3.h>

#include "xtd_Signal.h"

struct GLFWwindow;

namespace Demobox
{
	enum class ContextMode
	{
		WINDOWED   = 0,
		FULLSCREEN
	};

	class GlContext
	{
	public:
		xtd::signal<int32_t, int32_t>                   OnResized;
		xtd::signal<int32_t, int32_t, int32_t, int32_t> OnKeyPressed;
		xtd::signal<double, double>                     OnMouseMoved;
		xtd::signal<double, double>                     OnMouseWheel;
		xtd::signal<int32_t, int32_t>                   OnMousePressed;
		xtd::signal<int32_t, int32_t>                   OnMouseReleased;

		GlContext(int32_t width, int32_t height, const std::string& windowTitle, ContextMode contextMode);
		~GlContext();

		void SwapBuffers();

		bool ShouldClose();
		void RequestWindowClose();
		void forceShowWindow();
		void forceHideWindow();

		double time();
		double delta();

	private:
		static GlContext* sContext_;

		static void onResized_(GLFWwindow* window, int32_t w, int32_t h);
		static void onKeyPressed_(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
		static void onMouseMoved_(GLFWwindow* window, double x, double y);
		static void onMouseWheel_(GLFWwindow* window, double x, double y);
		static void onMouseClick_(GLFWwindow* window, int32_t button, int32_t action, int32_t mods);

		int32_t width_;
		int32_t height_;
		ContextMode contextMode_;

		GLFWwindow *window_;

		double startTime_;
		double lastFrameTime_;
		double delta_;
	};
}
