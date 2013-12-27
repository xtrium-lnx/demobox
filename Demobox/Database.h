#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "MediaItem.h"
#include "MediaItemDemo.h"

namespace Demobox
{
	class Database
	{
	public:
		~Database();

		//

		std::vector<MediaItem*> getDemos();
		std::vector<MediaItem*> getDemosByTitle(const std::string& contains);
		std::vector<MediaItem*> getDemosByGroup(const std::string& contains);
		std::vector<MediaItem*> getDemosByParty(const std::string& contains);
		std::vector<MediaItem*> getDemosByYear(uint32_t year);
		std::vector<MediaItem*> getDemosByPartyAndYear(const std::string& contains, uint32_t year);
		std::vector<MediaItem*> getDemosByCategory(MediaItemDemo::Category category);
		std::vector<MediaItem*> getDemosByPlatform(MediaItemDemo::Platform platform);

		std::vector<std::string> getDemoGroups();
		std::vector<std::string> getDemoCategories();
		std::vector<std::string> getDemoPlatforms();
		std::vector<std::string> getDemoParties();

		//

		std::vector<MediaItem*> getMusicSongs();
		std::vector<MediaItem*> getMusicSongsByTitle(const std::string& contains);
		std::vector<MediaItem*> getMusicSongsByArtist(const std::string& contains);
		std::vector<MediaItem*> getMusicSongsByAlbum(const std::string& contains);

		std::vector<std::string> getMusicArtists();
		std::vector<std::string> getMusicAlbums();

		//

		std::vector<MediaItem*> getMovies();
		std::vector<MediaItem*> getMoviesByGenre(const std::string& contains);

		std::vector<std::string> getMovieGenres();

		//

		static Database* instance();
		void initialize(const std::string& demosPath, const std::string& musicPath, const std::string& moviePath);

	private:
		enum class GatherMode
		{
			DIRECTORIES,
			FILES
		};

		Database();
		static Database* instance_;

		bool initialized_;

		std::string basePath_;
		std::vector<MediaItem*> demos_;
		std::vector<MediaItem*> songs_;
		std::vector<MediaItem*> movies_;

		void gather(const std::string& path, std::vector<std::string>& target, GatherMode mode = GatherMode::FILES);
	};
}
