#include "MediaItem.h"

using namespace Demobox;

MediaItem::MediaItem()
{
}

MediaItem::~MediaItem()
{
}

std::string MediaItem::title()  const
{
	return title_;
}

std::string MediaItem::author() const
{
	return author_;
}

void MediaItem::setScreenshot(GlTexture* screenshot)
{
	screenshot_ = screenshot;
}

GlTexture* MediaItem::screenshot()
{
	return screenshot_;
}
