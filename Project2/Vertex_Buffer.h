#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include "Vertex.h"
#include <vector>

class VBO
{

	public:
		GLuint ID;
		VBO(std::vector<Vertex>& vertices, const int& GLType = GL_STATIC_DRAW);
		VBO();

		void Bind();
		void unBind();
		void Delete();
};

#endif
