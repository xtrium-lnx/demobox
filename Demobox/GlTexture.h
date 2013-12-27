#pragma once

#include <cstdint>
#include <string>

namespace Demobox
{
	class GlTexture
	{
	public:
		GlTexture(const std::string& filename);
		~GlTexture();

		void bind(uint32_t unit = 0);
		void release();

	private:
		static GlTexture* sBoundTextures_[32];

		uint32_t textureId_;
		uint32_t unit_;
	};
}
