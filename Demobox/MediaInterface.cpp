#include "MediaInterface.h"

#include <vlc/vlc.h>
#include <iostream>

using namespace Demobox;

MediaInterface* MediaInterface::instance_ = nullptr;

void *s_mediaInterface_lock(void *data, void **p_pixels)
{
	MediaInterface* thisPtr = static_cast<MediaInterface*>(data);
	thisPtr->videoMutex_.lock();
	*p_pixels = thisPtr->videoImage_;
	return nullptr;
}

void s_mediaInterface_unlock(void *data, void *id, void *const *p_pixels)
{
	MediaInterface* thisPtr = static_cast<MediaInterface*>(data);
	thisPtr->videoMutex_.unlock();
}

void s_mediaInterface_display(void *data, void *id)
{
	(void)data;
}

MediaInterface::MediaInterface()
	: paused_(false),
	  libvlc_(nullptr),
	  media_(nullptr),
	  mediaPlayer_(nullptr)
{
	const char* vlc_argv[] = {
		"--no-osd"
	};

	libvlc_ = libvlc_new(1, vlc_argv);

	videoImage_ = new unsigned char[1920 * 1080 * 3];
}

MediaInterface::~MediaInterface()
{
	if (mediaPlayer_)
	{
		libvlc_media_player_stop(mediaPlayer_);
		libvlc_media_player_release(mediaPlayer_);
	}

	libvlc_release(libvlc_);

	videoMutex_.unlock();
	delete[] videoImage_;
}

MediaInterface* MediaInterface::instance()
{
	if (!instance_)
		instance_ = new MediaInterface();

	return instance_;
}

void MediaInterface::open(const std::string& path)
{
	if (mediaPlayer_)
	{
		libvlc_media_player_stop(mediaPlayer_);
		libvlc_media_player_release(mediaPlayer_);
	}

	media_ = libvlc_media_new_path(libvlc_, path.c_str());

	mediaPlayer_ = libvlc_media_player_new_from_media(media_);
	libvlc_video_set_callbacks(mediaPlayer_, s_mediaInterface_lock, s_mediaInterface_unlock, s_mediaInterface_display, this);
	libvlc_video_set_format(mediaPlayer_, "RV24", 1920, 1080, 1920 * 3);

	libvlc_media_release(media_);
	media_ = nullptr;
}

void MediaInterface::play()
{
	if (mediaPlayer_)
		libvlc_media_player_play(mediaPlayer_);
}

void MediaInterface::pause(bool paused /* = true */)
{
	if (mediaPlayer_)
	{
		paused_ = paused;
		libvlc_media_player_set_pause(mediaPlayer_, paused_);
	}
}

void MediaInterface::stop()
{
	if (mediaPlayer_)
	{
		libvlc_media_player_stop(mediaPlayer_);
		libvlc_media_player_release(mediaPlayer_);
		mediaPlayer_ = nullptr;
		paused_ = false;
	}
}

bool MediaInterface::isPlaying() const
{
	return (mediaPlayer_ && !paused_);
}

bool MediaInterface::isPaused() const
{
	return paused_;
}

bool MediaInterface::hasVideo() const
{
	if (mediaPlayer_)
		return (libvlc_media_player_has_vout(mediaPlayer_) > 0);

	return false;
}

void MediaInterface::handleVideo(std::function<void(unsigned char*, uint32_t, uint32_t)> handler)
{
	videoMutex_.lock();
	handler(videoImage_, 1920, 1080);
	videoMutex_.unlock();
}
