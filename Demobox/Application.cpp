#include "Application.h"
#include "Database.h"
#include "GlContext.h"
#include "GlShader.h"
#include "GlTextRenderer.h"
#include "GlTexture.h"
#include "ScreenInterface.h"
#include "MediaInterface.h"
#include "ScreenMediaPlayer.h"

#include <iostream>

#define WINDOW_TITLE "Demobox v0.1.0 \"Alkama\""

using namespace Demobox;

/* ************************************************************************* */

const char* backdropVS =
"void main() { gl_Position = gl_Vertex; gl_TexCoord[0] = gl_MultiTexCoord0; }";


const char* backdropFS =
"uniform float u_time;"
"uniform sampler2D u_l1;"
"vec2 u_vignette = vec2(1.4, -1.5);"

"vec4 make_bubbles()"
"{"
	"vec4 l1 = texture2D(u_l1, fract(gl_TexCoord[0].xy + vec2(0.0, u_time*0.1)));"
	"vec4 l2 = texture2D(u_l1, fract(gl_TexCoord[0].xy * 3.0 + vec2(0.0, u_time*0.15)))*0.8;"
	"vec4 l3 = texture2D(u_l1, fract(gl_TexCoord[0].xy * 6.0 + vec2(0.0, u_time*0.2)))*0.6;"
	"return mix(mix(l1, l2, l2.a), l3, l3.a);"
"}"

"void main()"
"{"
	"vec2 tc = gl_TexCoord[0].xy + vec2(sin(u_time + gl_TexCoord[0].y * 6.0), cos((u_time + gl_TexCoord[0].x * 5.0) * 0.7 + 0.4))*0.1;"
	"vec3 background = vec3(0.15, 0.45, 0.55) * (tc.y * 0.4 + 0.6);"
	"vec3 center = vec3(0.7, 0.0, 0.7) * 1.5;"

	"float ratio = (sin(distance(tc, vec2(0.5)) * 3.141596 * 2.25) * 0.5 + 0.5);"
	"ratio = smoothstep(0.0, 1.0, sqrt(ratio));"
	"vec3 background2 = mix(background, center, 1.0-ratio);"
	"background = 0.2*background2 + 0.8*background;"

	"float sdx = (gl_TexCoord[0].y - 0.5 + sin(gl_TexCoord[0].x * 10.0 + u_time) * 0.03 + cos(gl_TexCoord[0].x * 10.0 - u_time * 0.4 + 0.3) * 0.01);"
	"float dx = (1.0-(abs(sdx)) * 8.0);"
	"float p = (dx*0.1 > ((sin(gl_TexCoord[0].x * 8.0 + u_time * 0.1)*0.5+0.5) * 0.8 - 0.2)*0.1) ? (pow(abs(sdx*6.8), 32.0)+0.1) : 0.0;"

	"float vignette = clamp(u_vignette.x + distance(gl_TexCoord[0].xy, vec2(0.5)) * u_vignette.y, 0.0, 1.0);"
	"vignette = smoothstep(0.0, 1.0, vignette);"

	"vec4 bubble = make_bubbles();"
	"bubble = 0.03*bubble * bubble.a;"

	"gl_FragColor = vec4((bubble.rgb + background + vec3(p)*0.2) * vignette, 1.0);"
"}";

/* ************************************************************************* */

Application* Application::instance_ = nullptr;

Application::Application()
{
	/* OpenGL context creation */
	context_         = new GlContext(0, 0, WINDOW_TITLE, Demobox::ContextMode::FULLSCREEN);
	//context_ = new GlContext(800, 480, WINDOW_TITLE, Demobox::ContextMode::WINDOWED);

	/* Settings retrieval and DB filling */
	Database::instance()->initialize("assets/demos", "assets/music", "assets/movies");


	/* Background creation */
	backdropBubbles_ = new GlTexture("assets/bubbles.pvr");
	backdrop_        = new GlShader();
	backdrop_->setVertexShader(backdropVS);
	backdrop_->setFragmentShader(backdropFS);
	backdrop_->build();
	backdrop_->setUniform("u_l1", 0);

	modalIsDisplayed_ = false;
	modalAlpha_       = 0.0f;
	windowShouldForceShow_ = false;
	windowShouldForceHide_ = false;

	splashScreen_ = new GlTexture("assets/splash.pvr");
}

Application::~Application()
{
	delete context_;
}

Application* Application::instance()
{
	if (!instance_)
		instance_ = new Application();

	return instance_;
}

void Application::pushScreen(ScreenInterface* scr)
{
	if (!screenStack_.empty())
		screenStack_.back()->detach(context_);

	screenStack_.push_back(scr);
	screenStack_.back()->attach(context_);

	context_->OnKeyPressed.connect([&](int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS && key == GLFW_KEY_F10)
			hideModal();

		if (action == GLFW_PRESS && key == GLFW_KEY_F9)
			pushScreen(new ScreenMediaPlayer());

		if (action == GLFW_PRESS && key == GLFW_KEY_F8)
			ScreenMediaPlayer::npStartTime_ = now();
	});
}

void Application::popScreen()
{
	screenStack_.back()->detach(context_);
	screenStack_.pop_back();

	if (!screenStack_.empty())
		screenStack_.back()->attach(context_);

	context_->OnKeyPressed.connect([&](int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS && key == GLFW_KEY_F10)
			hideModal();

		if (action == GLFW_PRESS && key == GLFW_KEY_F9)
			pushScreen(new ScreenMediaPlayer());

		if (action == GLFW_PRESS && key == GLFW_KEY_F8)
			ScreenMediaPlayer::npStartTime_ = now();
	});
}

int Application::run()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	do
	{
		if (windowShouldForceShow_)
		{
			windowShouldForceShow_ = false;
			context_->forceShowWindow();
		}

		if (windowShouldForceHide_)
		{
			windowShouldForceHide_ = false;
			context_->forceHideWindow();
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_TEXTURE);
		glEnable(GL_TEXTURE_2D);

		// Render background animation
		backdropBubbles_->bind(0);
		backdrop_->bind();
		backdrop_->setUniform("u_time", float(context_->time()));
		glBegin(GL_QUADS);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f);
		glEnd();
		backdrop_->release();
		backdropBubbles_->release();

		// Render breadcrumbs
		GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 32);
		GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::RIGHT);
		glTranslated(-0.94, 0.9, 0.0);
		for (ScreenInterface* scr : screenStack_)
		{
			float alpha = std::string(scr->breadcrumb()) == std::string(screenStack_.back()->breadcrumb()) ? 0.8f : 0.4f;

			if (alpha == 0.4f)
				glScaled(0.8, 0.8, 0.8);
			float px = GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0f / 960.0f, 1.0f / 540.0f, std::string(scr->breadcrumb()), alpha) / 960.0f;

			if (alpha == 0.4f)
				glScaled(1.25, 1.25, 1.25);
			glTranslated(px * 1.5f, 0.0, 0.0);
		}

		// Render screen contents
		glClear(GL_DEPTH_BUFFER_BIT);

		screenStack_.back()->initialize();
		screenStack_.back()->display(context_);

		if (modalIsDisplayed_)
		{
			modalAlpha_ += context_->delta() * 2.5f;
			if (modalAlpha_ > 1.0f)
				modalAlpha_ = 1.0f;
		}
		else
		{
			modalAlpha_ -= context_->delta() * 2.5f;
			if (modalAlpha_ < 0.0f)
				modalAlpha_ = 0.0f;
		}

		if (modalAlpha_ > 0.0f)
		{
			float mAlpha = (-cos(modalAlpha_ * 3.14159265f) * 0.5f + 0.5f) * 0.8f;

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glDisable(GL_TEXTURE);
			glDisable(GL_TEXTURE_2D);

			glBegin(GL_QUADS);
			  glColor4f(0.0f, 0.0f, 0.0f, mAlpha);
			  glVertex2f(-1.0f, -1.0f);
			  glVertex2f(1.0f, -1.0f);
			  glVertex2f(1.0f, 1.0f);
			  glVertex2f(-1.0f, 1.0f);
			glEnd();

			glTranslated(0.0, 0.2, 0.0);
			GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 48);
			GlTextRenderer::instance()->setAlignment(GlTextRenderer::Alignment::CENTER);
			GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0f / 960.0f, 1.0f / 540.0f, modalTitle_, mAlpha);

			glTranslated(-0.03, -0.2, 0.0);
			GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 80);
			GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0f / 960.0f, 1.0f / 540.0f, modalText1_, mAlpha);

			glTranslated(0.0, -0.15, 0.0);
			GlTextRenderer::instance()->useFont("assets/fonts/corbel.ttf", 60);
			GlTextRenderer::instance()->write(0.0f, 0.0f, 1.0f / 960.0f, 1.0f / 540.0f, modalText2_, mAlpha);
		}

		backdrop_->setUniform("u_time", float(context_->time()));

		if (context_->time() < 4.0)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			float splashAlpha = 0.5 * (4.0 - context_->time());
			splashAlpha = fmin(1.0, fmax(0.0, splashAlpha));
			splashAlpha = -cos(splashAlpha * 3.1415926) * 0.5 + 0.5;

			splashScreen_->bind();
			glBegin(GL_QUADS);
			glColor4f(1.0f, 1.0f, 1.0f, splashAlpha);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
			glEnd();
			splashScreen_->release();
		}

		context_->SwapBuffers();
	} while ( !(context_->ShouldClose()) );

	return 0;
}

void Application::showModal(const std::string& title, const std::string& text1, const std::string& text2)
{
	modalTitle_ = title;
	modalText1_ = text1;
	modalText2_ = text2;
	modalIsDisplayed_ = true;
}

void Application::hideModal()
{
	modalIsDisplayed_ = false;
}

void Application::forceShowWindow()
{
	windowShouldForceShow_ = true;
}

void Application::forceHideWindow()
{
	windowShouldForceHide_ = true;
}

double Application::now()
{
	return context_->time();
}

double Application::delta()
{
	return context_->delta();
}
