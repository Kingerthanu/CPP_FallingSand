#include "Vertex_Array.h"

VAO::VAO()
{

	glGenVertexArrays(1, &this->ID);

}

void VAO::LinkAttrib(VBO& VBO, const GLuint& layout, const GLuint& numComponents, const GLenum& type, const GLsizeiptr& stride, void* offset)
{

	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.unBind();

}

void VAO::Bind()
{

	glBindVertexArray(this->ID);

}

void VAO::unBind()
{

	glBindVertexArray(0);

}

void VAO::Delete()
{

	glDeleteVertexArrays(1, &this->ID);

}
