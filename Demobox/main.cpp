#include "Application.h"
#include "ScreenHome.h"

#include <iostream>
#include <vector>

#include <Windows.h>

int main(int argc, char **argv)
{
	try
	{
		Demobox::Application *app = Demobox::Application::instance();
		app->pushScreen(new Demobox::ScreenHome());

		return app->run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	
	return 0; // never reached
}
