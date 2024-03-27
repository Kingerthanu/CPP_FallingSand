#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>
#include <vector>

class EBO
{

public:
	GLuint ID;
	EBO(std::vector<GLuint>& indices);
	EBO();

	void Bind();
	void unBind();
	void Delete();
};

#endif
