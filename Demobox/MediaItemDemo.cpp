#include "MediaItemDemo.h"
#include "Application.h"
#include "MediaInterface.h"

#include <iostream>
#include <thread>
#include <Windows.h>

using namespace Demobox;

MediaItemDemo::MediaItemDemo(const std::string& title, const std::string& author,
	                         Category category,        Platform platform,
							 const std::string& path,  const std::string& exeName,
							 const std::string& party /* = "" */, uint32_t year /* = 0 */, uint32_t rank /* = 0 */)
	: category_(category),
	  platform_(platform),
	  path_(path),
	  exeName_(exeName),
	  party_(party),
	  year_(year),
	  rank_(rank),
	  usesLauncher_(false)
{
	title_  = title;
	author_ = author;
	screenshot_ = nullptr;
}

MediaItemDemo::~MediaItemDemo()
{
}

MediaItemDemo::Category MediaItemDemo::category() const
{
	return category_;
}

MediaItemDemo::Platform MediaItemDemo::platform() const
{
	return platform_;
}

std::string MediaItemDemo::path() const
{
	return path_;
}

std::string MediaItemDemo::exeName() const
{
	return exeName_;
}

std::string MediaItemDemo::party() const
{
	return party_;
}

uint32_t MediaItemDemo::year() const
{
	return year_;
}

uint32_t MediaItemDemo::rank() const
{
	return rank_;
}

void MediaItemDemo::setLauncherUsageParameters(const std::string& windowTitle, const std::string& startButton)
{
	launcherWindowTitle_ = windowTitle;
	launcherStartButton_ = startButton;
	usesLauncher_ = true;
}

void MediaItemDemo::start()
{
	std::cout << "Launching " << title_ << " (by " << author_ << ")" << std::endl;
	MediaInterface::instance()->stop();

	std::thread processThread([=]() {
		Sleep(1500);
		Application::instance()->showModal("Hold on...", "Demo is running", "");
		Application::instance()->forceHideWindow();

		// Step 1 : launch the demo executable
		std::string canonicalPath = path_ + "/" + exeName_;
		STARTUPINFO startupinfo;
		PROCESS_INFORMATION processinformation;
		ZeroMemory(&startupinfo, sizeof(STARTUPINFO));
		startupinfo.cb = sizeof(startupinfo);
		ZeroMemory(&processinformation, sizeof(PROCESS_INFORMATION));

		char  fullPath[256]; char fullExeName[256]; char *filename;
		GetFullPathName(canonicalPath.c_str(), 256, fullExeName, &filename);
		GetFullPathName(canonicalPath.c_str(), 256, fullPath, &filename);
		filename[0] = '\0';

		CreateProcess(fullExeName, fullExeName, nullptr, nullptr, false, DETACHED_PROCESS, nullptr, fullPath, &startupinfo, &processinformation);
		
		if (usesLauncher_)
		{
			// Wait for the launcher's window to open
			HWND launcherWindowHandle = nullptr;
			do
			{
				Sleep(10);
				launcherWindowHandle = FindWindow(nullptr, launcherWindowTitle_.c_str());
			} while (launcherWindowHandle == nullptr);

			std::cout << " >> Window \"" << launcherWindowTitle_ << "\" opened." << std::endl;

			// Now the window is opened for sure and we know its handle
			// so we need to find the launch button and click it.
			HWND launcherButtonHandle = FindWindowEx(launcherWindowHandle, nullptr, "Button", launcherStartButton_.c_str());
			if (launcherButtonHandle)
			{
				SendMessage(launcherButtonHandle, BM_CLICK, 0, 0);
				std::cout << " >> Button \"" << launcherStartButton_ << "\" clicked." << std::endl;
			}
		}

		std::cout << " >> Demo should now start." << std::endl;

		WaitForSingleObject(processinformation.hProcess, 3600000);
		CloseHandle(processinformation.hProcess);
		CloseHandle(processinformation.hThread);
		std::cout << " >> Demo has ended." << std::endl;

		Application::instance()->forceShowWindow();
		Application::instance()->hideModal();
		Sleep(1200);
		Application::instance()->popScreen();
	});

	processThread.detach();
}
