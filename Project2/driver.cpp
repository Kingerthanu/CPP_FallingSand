#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include "shader.h"
#include "Vertex_Array.h"
#include "Element_Buffer.h"
#include <glm/glm.hpp>
#include <random>

// How many cells will be made per axis (gonna have equal window) 
const static int MAX_CELLS = 300;

// Max cells will be used by cell 2 so add before
#include "dynamicCell.h"

struct cursor
{
	bool _leftClick = false;
	bool _middleClick = false;
	bool _rightClick = false;
	bool _pulse = false;

	int r = 0;
	int c = 0;

};

cursor userCursor;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mod) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		userCursor._leftClick = (action == GLFW_PRESS);

	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		userCursor._rightClick = (action == GLFW_PRESS);

	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		userCursor._middleClick = (action == GLFW_PRESS);

	}
};

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {

	float yNormal = (ypos / 1900.0f);
	float xNormal = (xpos / 1900.0f);

	if ((xNormal < 1) && (yNormal < 1)) {

		userCursor.r = MAX_CELLS - (yNormal / (1.0f / MAX_CELLS));
		userCursor.c = (xNormal / (1.0f / MAX_CELLS));

	}
};

int generateCells(Dynamic_Cell***& cellGrid, const unsigned int& gridWidth, const unsigned int& gridHeight, VBO& vertexBuffer, EBO& elementBuffer)
{

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	float cellHeight = 2.0f / MAX_CELLS;
	float cellWidth = 2.0f / MAX_CELLS;

	cellGrid = new Dynamic_Cell * *[MAX_CELLS];

	for (unsigned int i = 0; i < MAX_CELLS; ++i) {

		cellGrid[i] = new Dynamic_Cell * [MAX_CELLS];

		for (unsigned int j = 0; j < MAX_CELLS; ++j) {

			cellGrid[i][j] = new Dynamic_Cell(vertices.size());

			// Calculate indices for the four vertices of each square
			int bottomLeft = vertices.size();
			int bottomRight = bottomLeft + 1;
			int topLeft = bottomRight + 1;
			int topRight = topLeft + 1;

			// Push vertices for the square with color (assuming it's white)
			vertices.push_back(Vertex{ glm::vec3(-1.0f + (cellWidth * j), -1.0f + (cellHeight * i), 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3(-1.0f + (cellWidth * (j + 1)), -1.0f + (cellHeight * i), 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) });

			vertices.push_back(Vertex{ glm::vec3(-1.0f + (cellWidth * j), -1.0f + (cellHeight * (i + 1)), 1.0f), glm::vec3(0.0f, 0.0f, 0.0f)});
			vertices.push_back(Vertex{ glm::vec3(-1.0f + (cellWidth * (j + 1)), -1.0f + (cellHeight * (i + 1)), 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) });


			// Push indices for the square
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
			indices.push_back(topLeft);

			indices.push_back(bottomRight);
			indices.push_back(topRight);
			indices.push_back(topLeft);

		}
	}

	// Override
	vertexBuffer = VBO(vertices);
	elementBuffer = EBO(indices);

	return indices.size();

};


int main()
{


	// Set openGL context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//		Create user window			  WIDTH | HEIGHT
	GLFWwindow* window = glfwCreateWindow(1900, 1900, "Cell Division", NULL, NULL);

	// Check if created
	if (!window) {

		glfwTerminate();
		return 0;

	}

	// Tell openGL we are using this window
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Call OpenGL library
	gladLoadGL();

	//				WIDTH | HEIGHT
	glViewport(0, 0, 1900, 1900);


	// Create our renderer
	Shader shaderProg("default.vert", "default.frag");

	// Set our vertex array, buffer, and element.
	VAO VAO1;
	VAO1.Bind();

	VBO VBO1;
	EBO EBO1;

	Dynamic_Cell*** CELL_GRID;

	int INDEX_COUNT = generateCells(CELL_GRID, 1900, 1900, VBO1, EBO1);

	VAO1.LinkAttrib(VBO1, 0, 2, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(2 * sizeof(float)));

	VBO1.Bind();
	EBO1.Bind();

	shaderProg.Activate();

	std::chrono::duration<double> frameDuration(1.0 / 120), sleepTime;

	std::chrono::steady_clock::time_point frameStart, frameEnd;
	std::chrono::milliseconds frameTime;

	while (!glfwWindowShouldClose(window)) {

		frameStart = std::chrono::high_resolution_clock::now();


		glClearColor(0.0f, 0.0f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		Vertex* bufferData = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

		for (int i = 0; i < MAX_CELLS; ++i) {

			for (int j = 0; j < MAX_CELLS; ++j) {

				if (!CELL_GRID[i][j]->getState()) {

					CELL_GRID[i][j]->turnOn(bufferData);

				}

				if (CELL_GRID[i][j]->think(CELL_GRID, i, j)) {
					j++;
				}

			}
		}

		//std::cout << "Row " << userCursor.r << " Column " << userCursor.c << '\n';

		
		glUnmapBuffer(GL_ARRAY_BUFFER);

		// ~~~~~~~~~~~~~~~ Cell Spawn Checks ~~~~~~~~~~~~~~~~~~~~~~~~~

		if (CELL_GRID[userCursor.r][userCursor.c]->typeID == 'D') {

			if (userCursor._leftClick) {

				int oldOffset = CELL_GRID[userCursor.r][userCursor.c]->offset;
				delete CELL_GRID[userCursor.r][userCursor.c];
				CELL_GRID[userCursor.r][userCursor.c] = new Dynamic_Sand_Cell(oldOffset);
			
			}

			else if (userCursor._rightClick) {
				int oldOffset = CELL_GRID[userCursor.r][userCursor.c]->offset;
				delete CELL_GRID[userCursor.r][userCursor.c];
				CELL_GRID[userCursor.r][userCursor.c] = new Dynamic_Solid_Cell(oldOffset);
			}
			else if (userCursor._middleClick) {
				int oldOffset = CELL_GRID[userCursor.r][userCursor.c]->offset;
				delete CELL_GRID[userCursor.r][userCursor.c];
				CELL_GRID[userCursor.r][userCursor.c] = new Dynamic_Water_Cell(oldOffset);
			}

		}
		

		// ~~~~~~~~~~~~END Cell Spawn Checks ~~~~~~~~~~~~~~~~~~~~~~~~~

		glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();



		frameEnd = std::chrono::high_resolution_clock::now();
		frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);

		if (frameTime < frameDuration) {

			sleepTime = frameDuration - frameTime;
			std::this_thread::sleep_for(sleepTime);
		}

	}

	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProg.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}



