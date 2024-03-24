#include "Vertex_Buffer.h"

VBO::VBO()
{

	// ...

}

VBO::VBO(std::vector<Vertex>& vertices, const int& GLType)
{

	glGenBuffers(1, &this->ID);
	glBindBuffer(GL_ARRAY_BUFFER, this->ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GLType);

}

void VBO::Bind()
{

	glBindBuffer(GL_ARRAY_BUFFER, this->ID);

}
void VBO::unBind()
{

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void VBO::Delete()
{

	glDeleteBuffers(1, &this->ID);

}