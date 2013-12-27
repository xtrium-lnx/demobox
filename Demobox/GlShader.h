#pragma once

#include <cstdint>
#include <string>

namespace Demobox
{
	class GlShader
	{
	public:
		GlShader();
		~GlShader();

		void setVertexShader(const std::string& src);
		void setFragmentShader(const std::string& src);

		void build();

		void bind();
		void release();

		void setUniform(const std::string& name, int v1);
		void setUniform(const std::string& name, float v1);
		void setUniform(const std::string& name, float v1, float v2);
		void setUniform(const std::string& name, float v1, float v2, float v3);
		void setUniform(const std::string& name, float v1, float v2, float v3, float v4);

	private:
		uint32_t shaderProgramId_;
	};
}
