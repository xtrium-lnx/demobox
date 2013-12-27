#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Demobox
{
	class GlVertexBuffer
	{
	public:
		GlVertexBuffer();
		~GlVertexBuffer();

		bool CreateAttribute(const std::string& name, const std::vector<float> data, uint8_t numItemsPerElement);
		void SetIndices(std::vector<uint32_t> indices);

		void Upload();
		void Draw();

	private:
		struct Attribute
		{
			std::string name;
			uint32_t index;
			std::vector<float> data;
			uint32_t numItemsPerElement;
		};

		std::vector<Attribute> attributes_;
		std::vector<uint32_t>  indices_;

		uint32_t  vaoId_;
		uint32_t *vboIds_;

		bool ready_;
	};
}
