#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Demobox
{
	class GlShader;

	class GlTextRenderer
	{
	public:
		enum class Alignment
		{
			LEFT = 0,
			CENTER,
			RIGHT
		};

		static GlTextRenderer* instance();
		~GlTextRenderer();

		void  useFont(const std::string& name, uint32_t size);
		void  setAlignment(Alignment alignment);
		float write(float xInit, float yInit, float sx, float sy, const std::string& text, float alpha);

	private:
		static GlTextRenderer* instance_;
		GlTextRenderer();

		FT_Library freetype_;

		std::unordered_map< std::string, std::unordered_map<uint32_t, FT_Face> > faces_;
		FT_Face currentFace_;

		Alignment alignment_;

		uint32_t  glyphTextureId_;
		uint32_t  glyphVBOId_;

		GlShader* fontShader_;
	};
}

