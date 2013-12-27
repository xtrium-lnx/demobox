#pragma once

#include "MediaItem.h"

#include <cstdint>
#include <string>
#include <thread>

namespace Demobox
{
	class GlTexture;

	class MediaItemDemo
		: public MediaItem
	{
	public:
		enum class Category : uint32_t
		{
			INTRO_1K_OR_LESS = 0,
			INTRO_4K,
			INTRO_64K,
			DEMO,
			GAME,
			MUSICDISK,
			WILD
		};

		enum class Platform : uint32_t
		{
			WINDOWS = 0,
			LINUX,
			MAC_OS_X,
			MS_DOS,
			WEB_BROWSER,

			AMIGA_OCS_ECS,
			AMIGA_AGA,
			ATARI_ST,
			ATARI_FALCON,

			C64,
			ATARI_VCS,
			AMSTRAD_CPC,
			ZX_SPECTRUM,

			MIRC,

			NINTENDO_NES,
			NINTENDO_SUPER_NES,
			NINTENDO_64,
			NINTENDO_GAMECUBE,
			NINTENDO_WII,

			SEGA_MASTER_SYSTEM,
			SEGA_MEGADRIVE,
			SEGA_GAMEGEAR,

			SONY_PSX,
			SONY_PS2,
			SONY_PS3,

			MICROSOFT_XBOX,
			MICROSOFT_XBOX_360
		};

		MediaItemDemo(const std::string& title, const std::string& author,
			          Category category,        Platform platform,
					  const std::string& path, const std::string& exeName,
					  const std::string& party = "", uint32_t year = 0, uint32_t rank = 0);
		~MediaItemDemo();

		Category    category() const;
		Platform    platform() const;

		std::string path()     const;
		std::string exeName()  const;
		std::string party()    const;
		uint32_t    year()     const;
		uint32_t    rank()     const;

		void        setLauncherUsageParameters(const std::string& windowTitle, const std::string& startButton);
		void        start();

	private:
		Category    category_;
		Platform    platform_;

		std::string party_;
		uint32_t    year_;
		uint32_t    rank_;

		std::string path_;
		std::string exeName_;

		std::string launcherWindowTitle_;
		std::string launcherStartButton_;
		bool        usesLauncher_;

		std::thread* processThread;
	};
}
