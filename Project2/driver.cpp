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

// Container for our cursor class
// Will hold bools of all essential mainloop actions for cursor.
struct cursor
{
	bool _leftClick = false;
	bool _middleClick = false;
	bool _rightClick = false;
	bool _delete = false;

	// Row and column interpretation of our cursor position
	int r = 0;
	int c = 0;

};

// Global userCursor handler.
cursor userCursor;

// Override OpenGL Key Action Callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	// If we clicked B, turn on delete mode on cursor.
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_B) {
			
			userCursor._delete = !userCursor._delete;

		}

	}
}

// Override OpenGL Mouse Action Callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mod) {


	// Click boolean callback,
	if (button == GLFW_MOUSE_BUTTON_LEFT) {

		userCursor._leftClick = (action == GLFW_PRESS);

	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {

		userCursor._rightClick = (action == GLFW_PRESS);

	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {

		userCursor._middleClick = (action == GLFW_PRESS);

	}
	else if (button == GLFW_MOUSE_BUTTON_4) {

		userCursor._delete = (action == GLFW_PRESS);

	}
};

// Override OpenGL cursor position Action Callback
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {

	// Normalize our position based on our pre-defined screen size.
	float yNormal = (ypos / 1900.0f);
	float xNormal = (xpos / 1900.0f);

	// If cursor position is in our screen, set its row column.
	if ((unsigned)(xNormal < 1) && ((unsigned)(yNormal < 1))) {

		userCursor.r = MAX_CELLS - (yNormal / (1.0f / MAX_CELLS));
		userCursor.c = (xNormal / (1.0f / MAX_CELLS));

	}
};

// Postcondition:  Will return a integer value of our indices count for openGL draw.
int generateCells(Dynamic_Cell***& cellGrid, const unsigned int& gridWidth, const unsigned int& gridHeight, VBO& vertexBuffer, EBO& elementBuffer)
{

	// Container for our verts and indices to be given to our 2D map.
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	// Calculate size of our cells based on length (normalized from -1 -> 1 and cell count
	float cellHeight = 2.0f / MAX_CELLS;
	float cellWidth = 2.0f / MAX_CELLS;

	
	cellGrid = new Dynamic_Cell * *[MAX_CELLS];

	for (unsigned int i = 0; i < MAX_CELLS; ++i) {	// Set rows.

		cellGrid[i] = new Dynamic_Cell * [MAX_CELLS];

		for (unsigned int j = 0; j < MAX_CELLS; ++j) {	// Set columns

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

	// Return size of indices we will be using
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

	// Set the position of the window
	glfwSetWindowPos(window, 800, 100);

	// Check if created
	if (!window) {

		glfwTerminate();
		return 0;

	}

	// Tell openGL we are using this window
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);

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

	// Set up map grid and index count.
	Dynamic_Cell*** CELL_GRID;
	const int INDEX_COUNT = generateCells(CELL_GRID, 1900, 1900, VBO1, EBO1);

	// Tell VAO how to interpret our data.
	VAO1.LinkAttrib(VBO1, 0, 2, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(2 * sizeof(float)));

	// Bind up context.
	VBO1.Bind();
	EBO1.Bind();
	shaderProg.Activate();

	// Counters for frame-rate.
	std::chrono::duration<double> frameDuration(1.0 / 120), sleepTime;
	std::chrono::steady_clock::time_point frameStart, frameEnd;
	std::chrono::milliseconds frameTime;

	while (!glfwWindowShouldClose(window)) {

		// Start game frame.
		frameStart = std::chrono::high_resolution_clock::now();

		// Clear buffer to now be drawn on.
		glClearColor(0.0f, 0.0f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Grab buffer of data for cell updates.
		Vertex* bufferData = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

		for (int i = 0; i < MAX_CELLS; ++i) { // For each row (down to up)

			for (int j = 0; j < MAX_CELLS; ++j) { // For each column (left to right)

				// If cell has not been drawn (color been updated) update.
				if (!CELL_GRID[i][j]->getState()) {

					CELL_GRID[i][j]->turnOn(bufferData);

				}

				CELL_GRID[i][j]->think(CELL_GRID, i, j);

			}
		}

		//std::cout << "Row " << userCursor.r << " Column " << userCursor.c << '\n';

		glUnmapBuffer(GL_ARRAY_BUFFER);	// Stop using our passed buffer.

		// ~~~~~~~~~~~~~~~ Cell Spawn Checks ~~~~~~~~~~~~~~~~~~~~~~~~~

		// Check if we are selecting a free cell.
		if (CELL_GRID[userCursor.r][userCursor.c]->typeID == 'D') {

			// If so, and is a left click; spawn sand non-solid.
			if (userCursor._leftClick) {

				int oldOffset = CELL_GRID[userCursor.r][userCursor.c]->offset;
				delete CELL_GRID[userCursor.r][userCursor.c];
				CELL_GRID[userCursor.r][userCursor.c] = new Dynamic_Sand_Cell(oldOffset);
			
			}
			// else if and is a right click; spawn stone solid.
			else if (userCursor._rightClick) {
				int oldOffset = CELL_GRID[userCursor.r][userCursor.c]->offset;
				delete CELL_GRID[userCursor.r][userCursor.c];
				CELL_GRID[userCursor.r][userCursor.c] = new Dynamic_Solid_Cell(oldOffset);
			}
			// else if and is a middle click; spawn water non-solid.
			else if (userCursor._middleClick) {
				int oldOffset = CELL_GRID[userCursor.r][userCursor.c]->offset;
				delete CELL_GRID[userCursor.r][userCursor.c];
				CELL_GRID[userCursor.r][userCursor.c] = new Dynamic_Water_Cell(oldOffset);
			}
			
		}
		else if (userCursor._delete)
		{

			int oldOffset = CELL_GRID[userCursor.r][userCursor.c]->offset;
			delete CELL_GRID[userCursor.r][userCursor.c];
			CELL_GRID[userCursor.r][userCursor.c] = new Dynamic_Cell(oldOffset);

		}
		

		// ~~~~~~~~~~~~END Cell Spawn Checks ~~~~~~~~~~~~~~~~~~~~~~~~~

		glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();


		// End game frame.
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



