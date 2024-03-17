#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include "Vertex_Buffer.h"


class VAO
{

	public:
		GLuint ID;
		VAO();

		void LinkAttrib(VBO& VBO, const GLuint& layout, const GLuint& numComponents, const GLenum& type, const GLsizeiptr& stride, void* offset);
		void Bind();
		void unBind();
		void Delete();
};


#endif