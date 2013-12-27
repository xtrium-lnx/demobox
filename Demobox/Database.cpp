#include "Database.h"
#include "MediaItemDemo.h"
#include "MediaItemMovie.h"
#include "MediaItemSong.h"
#include "GlTexture.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <Windows.h>

using namespace Demobox;

Database* Database::instance_ = nullptr;

Database::Database()
	: initialized_(false)
{
}

Database* Database::instance()
{
	if (!instance_)
		instance_ = new Database();

	return instance_;
}

void Database::initialize(const std::string& demosPath, const std::string& musicPath, const std::string& moviePath)
{
	if (initialized_)
		return;

	{ // Gathering demos
		std::vector<std::string> demos;
		gather(demosPath, demos, GatherMode::DIRECTORIES);
		for (std::string& demo : demos)
		{
			std::cout << "new demo: " << demo << std::endl;
			std::string demoName = std::string(strrchr(demo.c_str(), '/') + 1);

			std::string dataPath = demo;
			dataPath.append("/");
			dataPath.append(demoName);
			dataPath.append(".demo");

			std::ifstream infile(dataPath);

			std::string title, author;
			uint32_t category, platform;
			std::string exeName;
			std::string party = "";
			uint32_t year = 0, rank = 0;
			std::string windowTitle = "", startButtonCaption = "";

			std::getline(infile, title);
			std::getline(infile, author);

			infile >> category;
			std::cout << "  > cat: " << (category) << std::endl;
			infile >> platform;

			infile >> party;
			infile >> year;
			infile >> rank;

			std::getline(infile, exeName);
			std::getline(infile, exeName);
			std::getline(infile, windowTitle);
			std::getline(infile, startButtonCaption);

			MediaItemDemo* demoInstance = new MediaItemDemo(
				title, author,
				static_cast<MediaItemDemo::Category>(category), static_cast<MediaItemDemo::Platform>(platform),
				demo, exeName,
				party, year, rank
				);

			if (!windowTitle.empty() && !startButtonCaption.empty())
				demoInstance->setLauncherUsageParameters(windowTitle, startButtonCaption);

			std::string screenshotPath = demo;
			screenshotPath.append("/screenshot.pvr");
			bool screenshotExists = false;
			if (std::ifstream(screenshotPath))
				screenshotExists = true;

			if (screenshotExists)
			{
				std::cout << "   => has screenshot at " << screenshotPath << std::endl;
				demoInstance->setScreenshot(new GlTexture(screenshotPath));
			}

			demos_.push_back(demoInstance);
		}
	}

	//

	{ // Gathering songs
		std::vector<std::string> songs;
		gather(musicPath, songs);
		for (std::string& song : songs)
			songs_.push_back(new MediaItemSong(song));
	}

	//

	{ // Gathering movies
		std::vector<std::string> movieDirectories;
		gather(moviePath, movieDirectories, GatherMode::DIRECTORIES);
		for (std::string& movieDirectory : movieDirectories)
		{
			std::vector<std::string> movies;
			gather(movieDirectory, movies);
			for (std::string& moviePath : movies)
			{
				MediaItemMovie* movie = new MediaItemMovie(moviePath);
				movie->setGenre(std::string(strrchr(movieDirectory.c_str(), '/') + 1));
				movies_.push_back(movie);
			}
		}
	}

	//

	initialized_ = true;
}

Database::~Database()
{
}

void Database::gather(const std::string& path, std::vector<std::string>& target, GatherMode mode /* = GatherMode::FILES */)
{
	std::string searchPath = path;
	searchPath.append("/*");

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(searchPath.c_str(), &ffd);

	hFind = FindFirstFile(searchPath.c_str(), &ffd);

	do
	{
		bool flag = false;
		if (mode == GatherMode::FILES)
			flag = !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		else
			flag = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

		if (flag)
		{
			if (ffd.cFileName[0] == '.')
				continue;
			std::string dataPath = path;
			dataPath.append("/");
			dataPath.append(ffd.cFileName);

			std::cout << "[Database] Gathered media: " << dataPath << std::endl;

			target.push_back(dataPath);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
}

std::vector<MediaItem*> Database::getDemos()
{
	return demos_;
}

std::vector<MediaItem*> Database::getDemosByTitle(const std::string& contains)
{
	std::vector<MediaItem*> ret;

	std::string expression("(.*)"); expression.append(contains).append(".*");
	std::regex e(expression);
	for (MediaItem* demo : demos_)
	if (std::regex_match(demo->title(), e))
		ret.push_back(demo);

	return ret;
}

std::vector<MediaItem*> Database::getDemosByGroup(const std::string& contains)
{
	std::vector<MediaItem*> ret;

	std::string expression("(.*)"); expression.append(contains).append(".*");
	std::regex e(expression);
	for (MediaItem* demo : demos_)
	if (std::regex_match(demo->author(), e))
			ret.push_back(demo);

	return ret;
}

std::vector<MediaItem*> Database::getDemosByParty(const std::string& contains)
{
	std::vector<MediaItem*> ret;
	
	std::string expression("(.*)"); expression.append(contains).append(".*");
	std::regex e(expression);
	for (MediaItem* demo : demos_)
		if (std::regex_match(static_cast<MediaItemDemo*>(demo)->party(), e))
			ret.push_back(demo);
	
	return ret;
}

std::vector<MediaItem*> Database::getDemosByYear(uint32_t year)
{
	std::vector<MediaItem*> ret;

	for (MediaItem* demo : demos_)
		if (static_cast<MediaItemDemo*>(demo)->year() == year)
			ret.push_back(demo);
	
	return ret;
}

std::vector<MediaItem*> Database::getDemosByPartyAndYear(const std::string& contains, uint32_t year)
{
	std::vector<MediaItem*> ret;
	
	std::string expression("(.*)"); expression.append(contains).append(".*");
	std::regex e(expression);
	for (MediaItem* demo : demos_)
		if (std::regex_match(static_cast<MediaItemDemo*>(demo)->party(), e) && static_cast<MediaItemDemo*>(demo)->year() == year)
			ret.push_back(demo);
	
	return ret;
}

std::vector<MediaItem*> Database::getDemosByCategory(MediaItemDemo::Category category)
{
	std::vector<MediaItem*> ret;

	for (MediaItem* demo : demos_)
	if (static_cast<MediaItemDemo*>(demo)->category() == category)
		ret.push_back(demo);

	return ret;
}

std::vector<MediaItem*> Database::getDemosByPlatform(MediaItemDemo::Platform platform)
{
	std::vector<MediaItem*> ret;

	for (MediaItem* demo : demos_)
	if (static_cast<MediaItemDemo*>(demo)->platform() == platform)
		ret.push_back(demo);

	return ret;
}

std::vector<std::string> Database::getDemoGroups()
{
	std::vector<std::string> ret;

	for (MediaItem* demo : demos_)
	if (std::find(ret.begin(), ret.end(), demo->author()) == ret.end())
		ret.push_back(demo->author());

	return ret;
}

std::vector<std::string> Database::getDemoCategories()
{
	std::vector<MediaItemDemo::Category> cats;
	std::vector<std::string> ret;

	for (MediaItem* demo : demos_)
		if (std::find(cats.begin(), cats.end(), static_cast<MediaItemDemo*>(demo)->category()) == cats.end())
			cats.push_back(static_cast<MediaItemDemo*>(demo)->category());

	for (MediaItemDemo::Category cat : cats)
	{
		switch (cat)
		{
		case MediaItemDemo::Category::INTRO_1K_OR_LESS:
			ret.push_back("1k (or less) intro");
			break;

		case MediaItemDemo::Category::INTRO_4K:
			ret.push_back("4k intro");
			break;

		case MediaItemDemo::Category::INTRO_64K:
			ret.push_back("64k intro");
			break;

		case MediaItemDemo::Category::DEMO:
			ret.push_back("Demo");
			break;

		case MediaItemDemo::Category::GAME:
			ret.push_back("Game");
			break;

		case MediaItemDemo::Category::MUSICDISK:
			ret.push_back("Musicdisk");
			break;

		case MediaItemDemo::Category::WILD:
			ret.push_back("Wild / animation");
			break;
		}
	}
	
	return ret;
}

std::vector<std::string> Database::getDemoPlatforms()
{
	std::vector<MediaItemDemo::Platform> platforms;
	std::vector<std::string> ret;

	for (MediaItem* demo : demos_)
		if (std::find(platforms.begin(), platforms.end(), static_cast<MediaItemDemo*>(demo)->platform()) == platforms.end())
			platforms.push_back(static_cast<MediaItemDemo*>(demo)->platform());

	for (MediaItemDemo::Platform platform : platforms)
	{
		switch (platform)
		{
		case MediaItemDemo::Platform::WINDOWS:
			ret.push_back("Windows");
			break;
		case MediaItemDemo::Platform::LINUX:
			ret.push_back("Linux");
			break;
		case MediaItemDemo::Platform::MAC_OS_X:
			ret.push_back("Mac OS X");
			break;
		case MediaItemDemo::Platform::MS_DOS:
			ret.push_back("MS-DOS");
			break;
		case MediaItemDemo::Platform::WEB_BROWSER:
			ret.push_back("Web browser");
			break;
		case MediaItemDemo::Platform::AMIGA_OCS_ECS:
			ret.push_back("Amiga OCS/ECS");
			break;
		case MediaItemDemo::Platform::AMIGA_AGA:
			ret.push_back("Amiga AGA");
			break;
		case MediaItemDemo::Platform::ATARI_ST:
			ret.push_back("Atari ST / STe / STf");
			break;
		case MediaItemDemo::Platform::ATARI_FALCON:
			ret.push_back("Atari Falcon");
			break;
		case MediaItemDemo::Platform::C64:
			ret.push_back("Commodore 64");
			break;
		case MediaItemDemo::Platform::ATARI_VCS:
			ret.push_back("Atari VCS");
			break;
		case MediaItemDemo::Platform::AMSTRAD_CPC:
			ret.push_back("Amstrad CPC");
			break;
		case MediaItemDemo::Platform::ZX_SPECTRUM:
			ret.push_back("ZX Spectrum");
			break;
		case MediaItemDemo::Platform::MIRC:
			ret.push_back("mIRC");
			break;
		case MediaItemDemo::Platform::NINTENDO_NES:
			ret.push_back("Nintendo NES");
			break;
		case MediaItemDemo::Platform::NINTENDO_SUPER_NES:
			ret.push_back("Nintendo SuperNES");
			break;
		case MediaItemDemo::Platform::NINTENDO_64:
			ret.push_back("Nintendo 64");
			break;
		case MediaItemDemo::Platform::NINTENDO_GAMECUBE:
			ret.push_back("Nintendo GameCube");
			break;
		case MediaItemDemo::Platform::NINTENDO_WII:
			ret.push_back("Nintendo Wii");
			break;
		case MediaItemDemo::Platform::SEGA_MASTER_SYSTEM:
			ret.push_back("Sega Master System");
			break;
		case MediaItemDemo::Platform::SEGA_MEGADRIVE:
			ret.push_back("Sega Megadrive / Genesis");
			break;
		case MediaItemDemo::Platform::SEGA_GAMEGEAR:
			ret.push_back("Sega Gamegear");
			break;
		case MediaItemDemo::Platform::SONY_PSX:
			ret.push_back("Sony Playstation");
			break;
		case MediaItemDemo::Platform::SONY_PS2:
			ret.push_back("Sony Playstation 2");
			break;
		case MediaItemDemo::Platform::SONY_PS3:
			ret.push_back("Sony Playstation 3");
			break;
		case MediaItemDemo::Platform::MICROSOFT_XBOX:
			ret.push_back("Microsoft Xbox");
			break;
		case MediaItemDemo::Platform::MICROSOFT_XBOX_360:
			ret.push_back("Microsoft Xbox 360");
			break;
		default:
			break;
		}
	}

	return ret;
}

std::vector<std::string> Database::getDemoParties()
{
	std::vector<std::string> ret;
	
	for (MediaItem* demo : demos_)
		if (std::find(ret.begin(), ret.end(), static_cast<MediaItemDemo*>(demo)->party()) == ret.end())
			ret.push_back(static_cast<MediaItemDemo*>(demo)->party());

	return ret;
}

std::vector<MediaItem*> Database::getMusicSongs() { return songs_; }

std::vector<MediaItem*> Database::getMusicSongsByArtist(const std::string& contains)
{
	std::vector<MediaItem*> ret;

	std::string expression("(.*)"); expression.append(contains).append(".*");
	std::regex e(expression);
	for (MediaItem* song : songs_)
	if (std::regex_match(static_cast<MediaItemSong*>(song)->author(), e))
		ret.push_back(song);

	return ret;
}

std::vector<MediaItem*> Database::getMusicSongsByTitle(const std::string& contains) { return std::vector<MediaItem*>(); }
std::vector<MediaItem*> Database::getMusicSongsByAlbum(const std::string& contains) { return std::vector<MediaItem*>(); }

std::vector<std::string> Database::getMusicArtists()
{
	std::vector<std::string> ret;

	for (MediaItem* song : songs_)
	if (std::find(ret.begin(), ret.end(), static_cast<MediaItemSong*>(song)->author()) == ret.end())
		ret.push_back(static_cast<MediaItemSong*>(song)->author());

	return ret;
}

std::vector<std::string> Database::getMusicAlbums() { return std::vector<std::string>(); }

std::vector<MediaItem*> Database::getMovies() { return movies_; }

std::vector<MediaItem*> Database::getMoviesByGenre(const std::string& contains)
{
	std::vector<MediaItem*> ret;

	std::string expression("(.*)"); expression.append(contains).append(".*");
	std::regex e(expression);
	for (MediaItem* movie : movies_)
		if (std::regex_match(static_cast<MediaItemMovie*>(movie)->genre(), e))
			ret.push_back(movie);

	return ret;
}

std::vector<std::string> Database::getMovieGenres()
{
	std::vector<std::string> ret;

	for (MediaItem* movie : movies_)
		if (std::find(ret.begin(), ret.end(), static_cast<MediaItemMovie*>(movie)->genre()) == ret.end())
			ret.push_back(static_cast<MediaItemMovie*>(movie)->genre());

	return ret;
}
