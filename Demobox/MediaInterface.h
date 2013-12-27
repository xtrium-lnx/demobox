#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <mutex>

struct libvlc_instance_t;
struct libvlc_media_t;
struct libvlc_media_player_t;

namespace Demobox
{
	class MediaInterface
	{
	public:
		static MediaInterface* instance();
		~MediaInterface();

		void open(const std::string& path);

		void play();
		void pause(bool paused = true);
		void stop();

		bool isPlaying() const;
		bool isPaused() const;

		bool hasVideo() const;
		void handleVideo(std::function<void(unsigned char*, uint32_t, uint32_t)> handler);

		std::mutex             videoMutex_;
		unsigned char*         videoImage_;

	private:
		static MediaInterface* instance_;
		MediaInterface();

		libvlc_instance_t     *libvlc_;
		libvlc_media_t        *media_;
		libvlc_media_player_t *mediaPlayer_;

		bool                   paused_;
	};
}

