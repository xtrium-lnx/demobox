#include "MediaItemMovie.h"
#include "MediaInterface.h"

#include <Windows.h>

using namespace Demobox;

MediaItemMovie::MediaItemMovie(const std::string& path)
{
	char data[1024];
	char* filename;
	GetFullPathName(path.c_str(), 1024, data, &filename);
	path_ = std::string(data);
	filename_ = std::string(filename);

	title_ = filename_;
	author_ = "";
	genre_ = "";
}

MediaItemMovie::~MediaItemMovie()
{
}

std::string MediaItemMovie::path()
{
	return path_;
}

void MediaItemMovie::setGenre(const std::string& genre)
{
	genre_ = genre;
}

std::string MediaItemMovie::genre() const
{
	return genre_;
}

std::string MediaItemMovie::filename()
{
	return filename_;
}

void MediaItemMovie::start()
{
	MediaInterface::instance()->open(path_);
	MediaInterface::instance()->play();
}
