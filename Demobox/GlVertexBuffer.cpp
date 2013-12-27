#include "GlVertexBuffer.h"

#include <GL/glew.h>

using namespace Demobox;

GlVertexBuffer::GlVertexBuffer()
	: vaoId_(0),
	  vboIds_(nullptr),
	  ready_(false)
{
}

GlVertexBuffer::~GlVertexBuffer()
{
	if (ready_)
	{
		glDeleteBuffers(attributes_.size() + 1, vboIds_);
		glDeleteVertexArrays(1, &vaoId_);
	}
}

bool GlVertexBuffer::CreateAttribute(const std::string& name, const std::vector<float> data, uint8_t numItemsPerElement)
{
	for (Attribute& a : attributes_)
		if (a.name == name)
			return false;

	Attribute attribute;

	attribute.name = name;
	attribute.index = attributes_.size();
	attribute.numItemsPerElement = numItemsPerElement;
	attribute.data = data;

	attributes_.push_back(attribute);

	return true;
}

void GlVertexBuffer::SetIndices(std::vector<uint32_t> indices)
{
	indices_ = indices;
}

void GlVertexBuffer::Upload()
{
	if (!ready_)
	{
		glGenVertexArrays(1, &vaoId_);
		glBindVertexArray(vaoId_);
		vboIds_ = new unsigned int[attributes_.size() + 1];
		glGenBuffers(attributes_.size() + 1, vboIds_);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds_[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices_.size(), &(indices_[0]), GL_STATIC_DRAW);

		for (Attribute& attr : attributes_)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vboIds_[attr.index + 1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * attr.data.size(), &(attr.data[0]), GL_STATIC_DRAW);
		}

		glBindVertexArray(0);

		ready_ = true;
	}
}

void GlVertexBuffer::Draw()
{
	if (!ready_)
		Upload();

	glBindVertexArray(vaoId_);

	for (Attribute& attr : attributes_)
	{
		glEnableVertexAttribArray(attr.index);
		glBindBuffer(GL_ARRAY_BUFFER, vboIds_[attr.index + 1]);
		glVertexAttribPointer(attr.index, attr.numItemsPerElement, GL_FLOAT, GL_FALSE, 0, 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds_[0]);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);

	for (Attribute& attr : attributes_)
		glDisableVertexAttribArray(attr.index);

	glBindVertexArray(0);
}
