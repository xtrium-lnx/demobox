#include "MediaItemSong.h"
#include "MediaInterface.h"

#include <id3.h>
#include <iostream>
#include <regex>

#include <Windows.h>

using namespace Demobox;

MediaItemSong::MediaItemSong(const std::string& path)
{
	ID3Tag *tag = ID3Tag_New();

	char data[1024];
	char* filename;
	GetFullPathName(path.c_str(), 1024, data, &filename);
	path_ = std::string(data);

	ID3Tag_Link(tag, path_.c_str());

	data[0] = 0;

	{
		ID3Field_GetASCII(ID3Frame_GetField(ID3Tag_FindFrameWithID(tag, ID3_FrameID::ID3FID_TITLE), ID3_FieldID::ID3FN_TEXT), data, 1024);
		title_ = std::string(data);
		if (!std::regex_match(title_, std::regex("[a-zA-Z0-9](.*)")))
			title_ = "No title";
		std::cout << "title: " << title_ << std::endl;
	}

	data[0] = 0;
	{
		ID3Field_GetASCII(ID3Frame_GetField(ID3Tag_FindFrameWithID(tag, ID3_FrameID::ID3FID_LEADARTIST), ID3_FieldID::ID3FN_TEXT), data, 1024);
		author_ = std::string(data);
		if (!std::regex_match(author_, std::regex("[a-zA-Z0-9](.*)")))
			author_ = "No artist";
		std::cout << "artist: " << author_ << std::endl;
	}
}


MediaItemSong::~MediaItemSong()
{
}

std::string MediaItemSong::genre() const
{
	return genre_;
}

std::string MediaItemSong::path() const
{
	return path_;
}

void MediaItemSong::start()
{
	MediaInterface::instance()->open(path_);
	MediaInterface::instance()->play();
}
