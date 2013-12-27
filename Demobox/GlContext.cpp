#pragma comment(lib, "glew32s")

#include "GlContext.h"

#include <exception>
#include <Windows.h>

using namespace Demobox;

GlContext* GlContext::sContext_ = nullptr;

void GlContext::onResized_(GLFWwindow* window, int w, int h)
{
	sContext_->OnResized(w, h);
}

void GlContext::onKeyPressed_(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	sContext_->OnKeyPressed(key, scancode, action, mods);
}

void GlContext::onMouseMoved_(GLFWwindow* window, double x, double y)
{
	sContext_->OnMouseMoved(x, y);
}

void GlContext::onMouseWheel_(GLFWwindow* window, double x, double y)
{
	sContext_->OnMouseWheel(x, y);
}

void GlContext::onMouseClick_(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		sContext_->OnMousePressed(button, mods);
	else
		sContext_->OnMouseReleased(button, mods);
}

GlContext::GlContext(int32_t width, int32_t height, const std::string& windowTitle, ContextMode contextMode)
	: width_(width),
	  height_(height),
	  contextMode_(contextMode)
{
	if (sContext_ != nullptr)
		throw std::exception("An OpenGL context has already been created.");

	sContext_ = this;

	if (!glfwInit())
		throw std::exception("Unable to initialize GLFW.");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 16);

	if (contextMode_ == ContextMode::WINDOWED)
		window_ = glfwCreateWindow(width_, height_, windowTitle.c_str(), nullptr, nullptr);
	else
	{
		//int screenXpos, screenYpos;
		//glfwGetMonitorPos(glfwGetPrimaryMonitor(), &screenXpos, &screenYpos);
		const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwWindowHint(GLFW_DECORATED, false);
		glfwWindowHint(GLFW_VISIBLE,   false);

		width_  = vidmode->width;
		height_ = vidmode->height;

		window_ = glfwCreateWindow(vidmode->width, vidmode->height, windowTitle.c_str(), nullptr, nullptr);
		glfwSetWindowPos(window_, 0, 0);
		glfwShowWindow(window_);
	}

	if (window_ == nullptr)
	{
		glfwTerminate();
		throw std::exception("GLFW was unable to open a context window.");
	}

	glfwMakeContextCurrent(window_);

	glfwSetWindowSizeCallback(window_, GlContext::onResized_);
	glfwSetKeyCallback(window_, GlContext::onKeyPressed_);
	glfwSetCursorPosCallback(window_, onMouseMoved_);
	glfwSetMouseButtonCallback(window_, onMouseClick_);
	glfwSetScrollCallback(window_, onMouseWheel_);

	glewExperimental = true;
	glewInit();

	startTime_ = glfwGetTime();
	lastFrameTime_ = startTime_;
	delta_ = 0.0;
}


GlContext::~GlContext()
{
	glfwTerminate();
}

bool GlContext::ShouldClose()
{
	return glfwWindowShouldClose(window_);
}

void GlContext::RequestWindowClose()
{
	glfwSetWindowShouldClose(window_, GL_TRUE);
}

void GlContext::forceShowWindow()
{
	glfwHideWindow(window_);
	Sleep(5000);
	glfwPollEvents();
	glfwShowWindow(window_);
}

void GlContext::forceHideWindow()
{
	glfwHideWindow(window_);
	glfwPollEvents();
	glfwHideWindow(window_);
}

void GlContext::SwapBuffers()
{
	double currentTime_ = glfwGetTime();
	delta_ = currentTime_ - lastFrameTime_;
	lastFrameTime_ = currentTime_;

	glfwPollEvents();
	glfwSwapBuffers(window_);
}

double GlContext::time()
{
	return glfwGetTime() - startTime_;
}

double GlContext::delta()
{
	return delta_;
}
