#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include "Vertex.h"
#include <vector>

class VBO
{

	public:
		GLuint ID;
		VBO(std::vector<Vertex>& vertices);
		VBO();

		void Bind();
		void unBind();
		void Delete();
};

#endif
