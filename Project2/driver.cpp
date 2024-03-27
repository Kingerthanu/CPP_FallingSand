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
#include "world.h"



// Container for our cursor class
// Will hold bools of all essential mainloop actions for cursor.
struct cursor
{
	// Bit fields for boolean flags to reduce memory footprint
	bool _leftClick : 1;
	bool _middleClick : 1;
	bool _rightClick : 1;
	bool _fillIn : 1;
	bool _held : 1;

	// Row and column interpretation of our cursor position
	unsigned int r;
	unsigned int c;

	// Floating-point coordinates for finer precision
	float rx;
	float ry;

	// Radius with float type for better precision
	float radii;

	// Char type for generation ID
	char genID;
};

// Global userCursor handler.
cursor userCursor;

// Override OpenGL Key Action Callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	
}

// Override OpenGL Mouse Action Callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mod) {



		if ((action == GLFW_PRESS)) {

			// Click boolean callback,
			if (button == GLFW_MOUSE_BUTTON_LEFT) {

				userCursor._leftClick = true;
				userCursor._held = true;

			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) {

				userCursor._rightClick = true;

			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {

				userCursor._middleClick = true;

			}

		}
		else {

			// Click boolean callback,
			if (button == GLFW_MOUSE_BUTTON_LEFT) {

				userCursor._leftClick = false;
				userCursor._held = false;

			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) {

				userCursor._rightClick = false;

			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {

				userCursor._middleClick = false;

			}

			

		}

		

	

};

// Override OpenGL cursor position Action Callback
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {


	// Normalize our position based on our pre-defined screen size.
	double yNormal = (ypos / 1900.0f);
	double xNormal = (xpos / 1900.0f);

	// If cursor position is in our screen, set its row column.
	if ((xNormal < 1.0f) || ((yNormal < 1.0f))) {

		userCursor.r = MAX_CELLS - (yNormal / (1.0f / MAX_CELLS));
		userCursor.c = (xNormal / (1.0f / MAX_CELLS));

		userCursor.rx = (xpos / 1900.0f) * 2.0f - 1.0f;
		userCursor.ry = ((1900.0f - ypos) / 1900.0f) * 2.0f - 1.0f;

	}


	

	
};


struct Mesh
{

	public:
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

};

class Button
{

	protected:
		Mesh wireFrame;
		unsigned int offset = 0;
		
	public:

		Mesh& getWireFrame()
		{

			return this->wireFrame;

		};

		virtual void react(){};

		const char returnID;
		Button(const char& outputID, const float& xPos, const float& yPos, const float& width, const float& height, const glm::vec3& flatColor = glm::vec3(1.0f, 0.0f, 0.5f)) : returnID(outputID)
		{

			// Bottom Left.
			this->wireFrame.vertices.push_back(Vertex{ glm::vec2(xPos, yPos), flatColor});

			//Bottom Right.
			this->wireFrame.vertices.push_back(Vertex{ glm::vec2(xPos + width, yPos), flatColor });

			// Top Left.
			this->wireFrame.vertices.push_back(Vertex{ glm::vec2(xPos, yPos + height), flatColor });

			// Top Right.
			this->wireFrame.vertices.push_back(Vertex{ glm::vec2(xPos + width, yPos + height), flatColor });

			// Bottom Left -> Bottom Right -> Top Left
			this->wireFrame.indices.push_back(0);
			this->wireFrame.indices.push_back(1);
			this->wireFrame.indices.push_back(2);

			// Bottom Right -> Top Left -> Top Right
			this->wireFrame.indices.push_back(1);
			this->wireFrame.indices.push_back(2);
			this->wireFrame.indices.push_back(3);



			//this->vertexBuffer = VBO(this->wireFrame.vertices);
			//this->elementBuffer = EBO(this->wireFrame.indices);

			
		}

		VBO& getVertexBuffer()
		{

			//return this->vertexBuffer;

		}

		bool isPressed()
		{

			return (this->wireFrame.vertices[0].position[0] <= userCursor.rx && this->wireFrame.vertices[0].position[1] <= userCursor.ry) && (userCursor.rx <= this->wireFrame.vertices[3].position[0] && userCursor.ry <= this->wireFrame.vertices[3].position[1]);

		}

		// Single shader being utilized, don't need to pass can call GLFW func.
		void draw()
		{
			/*
			this->vertexBuffer.Bind();
			this->elementBuffer.Bind();

			glDrawElements(GL_TRIANGLES, this->wireFrame.indices.size(), GL_UNSIGNED_INT, 0);
			*/
		}


};

class AnalogSlider : public Button
{
	private:
		float value = 0.0f;
		float gMin, gMax;

	public:
		AnalogSlider(unsigned int newOffset, const float& xPos, const float& yPos, const float& width, const float& height, const float& globalYMinimum, const float& globalYMaximum, const glm::vec3& flatColor = glm::vec3(0.13f, 0.05f, 0.13f)) : Button(' ', xPos, yPos, width, height, flatColor)
		{
	
			this->gMin = globalYMinimum;
			this->gMax = globalYMaximum;
			this->offset = newOffset;
			
		}

		void shift()
		{

			const float offsetAmnt = userCursor.ry;
			
	
			if (this->gMin <= offsetAmnt && offsetAmnt <= this->gMax) {

				this->wireFrame.vertices[0].position[1] = offsetAmnt - 0.01f;
				this->wireFrame.vertices[1].position[1] = offsetAmnt - 0.01f;
				this->wireFrame.vertices[2].position[1] = offsetAmnt + 0.01f;
				this->wireFrame.vertices[3].position[1] = offsetAmnt + 0.01f;
				
				userCursor.radii = ((offsetAmnt - this->gMin) / (this->gMax - this->gMin)) * 100.0f;

				// Because we know this will be first position in our wireframe of our UI buffer.
				Vertex* bufferData = (Vertex*)(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));



				// Update all vertices in the buffer

				bufferData[this->offset].position[1] = this->wireFrame.vertices[0].position[1];
				bufferData[this->offset + 1].position[1] = this->wireFrame.vertices[1].position[1];
				bufferData[this->offset + 2].position[1] = this->wireFrame.vertices[2].position[1];
				bufferData[this->offset + 3].position[1] = this->wireFrame.vertices[3].position[1];


				glUnmapBuffer(GL_ARRAY_BUFFER);

			}
			
		}

};

class SliderButton : public Button
{
	private:
		bool& curState;

	public:
		SliderButton(bool& state, const float& xPos, const float& yPos, const float& width, const float& height, const glm::vec3& flatColor = glm::vec3(0.13f, 0.15f, 0.13f)) : Button(' ', xPos, yPos, width, height, flatColor), curState(state) {}

		void shift()
		{
	
			const float offsetAmnt = 0.35f;

			if (this->curState) {

				this->wireFrame.vertices[0].position[0] -= offsetAmnt;
				this->wireFrame.vertices[1].position[0] -= offsetAmnt;
				this->wireFrame.vertices[2].position[0] -= offsetAmnt;
				this->wireFrame.vertices[3].position[0] -= offsetAmnt;

				

			}
			else {

				this->wireFrame.vertices[0].position[0] += offsetAmnt;
				this->wireFrame.vertices[1].position[0] += offsetAmnt;
				this->wireFrame.vertices[2].position[0] += offsetAmnt;
				this->wireFrame.vertices[3].position[0] += offsetAmnt;

			}
		
			// Because we know this will be first position in our wireframe of our UI buffer.
			Vertex* bufferData = (Vertex*)(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

			// Update all vertices in the buffer
			
			for (unsigned int i = 0; i < 4; ++i) {
				
				bufferData[i].position[0] = this->wireFrame.vertices[i].position[0];

			}

			glUnmapBuffer(GL_ARRAY_BUFFER);
			
		}
};

class eventButton : public Button
{

	public:
		void react() 
		{
		
			userCursor._fillIn = true;
				
		};

};

class UI_PANEL
{

	public:

		Mesh wireFrame;
		std::vector<Button> toggleButtons;
		AnalogSlider radiusSlider = AnalogSlider(44, 0.90f, 0 - 0.08f, 0.05f, 0.02f, 0 - 0.16f, 0 + 0.10f);
		Button fillInButton;

	public:

		Mesh& getWireFrame()
		{

			return this->wireFrame;

		};

		UI_PANEL(const float& xPos, const float& yPos, const float& width, const float& height, const glm::vec3& flatColor = glm::vec3(0.45f, 0.4f, 0.4f)) : fillInButton(Button('F', xPos + 0.08f, yPos + 0.15f, 0.05f, 0.05f, glm::vec3(0.2f, 0.3f, 0.1f)))
		{

			// Bottom Left.
			this->wireFrame.vertices.push_back(Vertex{ glm::vec2(xPos, yPos - 0.02f), flatColor });

			//Bottom Right.
			this->wireFrame.vertices.push_back(Vertex{ glm::vec2(xPos + width, yPos - 0.02f), flatColor });

			// Top Left.
			this->wireFrame.vertices.push_back(Vertex{ glm::vec2(xPos, yPos + height), flatColor });

			// Top Right.
			this->wireFrame.vertices.push_back(Vertex{ glm::vec2(xPos + width, yPos + height), flatColor });

			// Bottom Left -> Bottom Right -> Top Left
			this->wireFrame.indices.push_back(0);
			this->wireFrame.indices.push_back(1);
			this->wireFrame.indices.push_back(2);

			// Bottom Right -> Top Left -> Top Right
			this->wireFrame.indices.push_back(1);
			this->wireFrame.indices.push_back(2);
			this->wireFrame.indices.push_back(3);

			this->toggleButtons.push_back(Button('S', xPos + 0.02f, yPos + 0.01f, 0.05f, 0.05f, glm::vec3(1.0f, 0.851f, 0.0f)));
			this->toggleButtons.push_back(Button('B', xPos + 0.02f, yPos + 0.08f, 0.05f, 0.05f, glm::vec3(0.5f, 0.5f, 0.59f)));
			this->toggleButtons.push_back(Button('G', xPos + 0.02f, yPos + 0.15f, 0.05f, 0.05f, glm::vec3(0.25f, 0.25f, 0.25f)));
			this->toggleButtons.push_back(Button('D', xPos + 0.08f, yPos + 0.01f, 0.05f, 0.05f, glm::vec3(0.0f, 0.0f, 0.0f)));
			this->toggleButtons.push_back(Button('W', xPos + 0.08f, yPos + 0.08f, 0.05f, 0.05f, glm::vec3(0.0f, 0.0f, 0.32f)));
			this->toggleButtons.push_back(Button('K', xPos + 0.14f, yPos + 0.01f, 0.05f, 0.05f, glm::vec3(0.64f, 0.0f, 0.02f)));
			this->toggleButtons.push_back(Button('P', xPos + 0.14f, yPos + 0.08f, 0.05f, 0.05f, glm::vec3(0.5f, 0.56f, 0.59f)));
			this->toggleButtons.push_back(Button('T', xPos + 0.14f, yPos + 0.15f, 0.05f, 0.05f, glm::vec3(0.5f, 0.59f, 0.59f)));
			
			// 0.90f, 0 - 0.08f, 0.05f, 0.02f

			Mesh radiusSliderRail;
			// Bottom Left.
			radiusSliderRail.vertices.push_back(Vertex{ glm::vec2(0.92f, 0 - 0.18f), glm::vec3(0.13f, 0.05f, 0.13f) });

			//Bottom Right.
			radiusSliderRail.vertices.push_back(Vertex{ glm::vec2(0.92f + 0.02f,  0 - 0.18f - 0.02f), glm::vec3(0.13f, 0.05f, 0.13f) });

			// Top Left.
			radiusSliderRail.vertices.push_back(Vertex{ glm::vec2(0.92f,  0 - 0.18f + 0.02f), glm::vec3(0.13f, 0.05f, 0.13f) });

			// Top Right.
			radiusSliderRail.vertices.push_back(Vertex{ glm::vec2(0.92f + 0.02f,  0 - 0.18f + 0.02f), glm::vec3(0.13f, 0.05f, 0.13f) });

			this->wireFrame.vertices.insert(this->wireFrame.vertices.end(), fillInButton.getWireFrame().vertices.begin(), fillInButton.getWireFrame().vertices.end());

			for (unsigned int i = 0; i < this->toggleButtons.size(); ++i){
				
				this->wireFrame.vertices.insert(this->wireFrame.vertices.end(), this->toggleButtons[i].getWireFrame().vertices.begin(), this->toggleButtons[i].getWireFrame().vertices.end());

			}

			
			this->wireFrame.vertices.insert(this->wireFrame.vertices.end(), this->radiusSlider.getWireFrame().vertices.begin(), this->radiusSlider.getWireFrame().vertices.end());

		}

		void checkButtons()
		{

			if (this->radiusSlider.isPressed()) {

				this->radiusSlider.shift();
				return;

			}
			else if (this->fillInButton.isPressed()) {
				userCursor._fillIn = !userCursor._fillIn;
				return;
			}

			
			// Then check other buttons for their logic.
			for (unsigned int i = 0, size = this->toggleButtons.size(); i < size; i++)
			{

				if (this->toggleButtons[i].isPressed()) {

					userCursor.genID = toggleButtons[i].returnID;

				}


			}

		}

		VBO& getVertexBuffer()
		{

			//return this->vertexBuffer;

		}

		// Single shader being utilized, don't need to pass can call GLFW func.
		void draw()
		{

			/*
				this->vertexBuffer.Bind();
				this->elementBuffer.Bind();
			
				glDrawElements(GL_TRIANGLES, this->wireFrame.indices.size(), GL_UNSIGNED_INT, 0);

				for (unsigned int i = 0, size = this->toggleButtons.size(); i < size; i++)
				{
					std::cout << "Attemping Draw " << i << '\n';
					this->toggleButtons[i].draw();

				}

				this->radiusSlider.draw();
			*/
		}

};

class UI_ElementSelector
{
	
	VAO vertexArray;
	VBO vertexBuffer;
	EBO elementBuffer;
	Mesh wireFrame;

	bool expanded = false;
	unsigned int expansionStep = 6;
	
	SliderButton expandButton = SliderButton(expanded, 0.95f, 0 - 0.08f, 0.05f, 0.08f);
	
	// Canvas should have ability to pass in VAO to bind its buffer to.
	UI_PANEL canvas = UI_PANEL(0.65f, ((0.25f / -2.0f) - 0.02f), 0.45f, 0.25f);

	void addSquare(const unsigned int& origin, std::vector<GLuint>& indices, const unsigned int& amount = 1)
	{

		for (unsigned int i = 0, prevOrigin = origin; i < amount; i++, prevOrigin += 4) {

			// Bottom Left -> Bottom Right -> Top Left
			indices.push_back(prevOrigin);
			indices.push_back(prevOrigin + 1);
			indices.push_back(prevOrigin + 2);

			// Bottom Right -> Top Left -> Top Right
			indices.push_back(prevOrigin + 1);
			indices.push_back(prevOrigin + 2);
			indices.push_back(prevOrigin + 3);

		}
		

	};

	public:

		UI_ElementSelector()
		{

			
			this->addSquare(wireFrame.vertices.size(), wireFrame.indices);

			// Copy vertices from expandButton's wireframe to wireFrame
			this->wireFrame.vertices.insert(this->wireFrame.vertices.end(), this->expandButton.getWireFrame().vertices.begin(), this->expandButton.getWireFrame().vertices.end());

			this->addSquare(this->wireFrame.vertices.size(), this->wireFrame.indices);
			this->addSquare(this->wireFrame.vertices.size(), this->wireFrame.indices, 12);				// SQUARE <--------------
			// Copy vertices from expandButton's wireframe to wireFrame
			this->wireFrame.vertices.insert(this->wireFrame.vertices.end(), this->canvas.getWireFrame().vertices.begin(), this->canvas.getWireFrame().vertices.end());

			this->vertexArray.Bind();
			
			this->vertexBuffer = VBO(this->wireFrame.vertices);
			this->elementBuffer = EBO(this->wireFrame.indices);

			this->vertexArray.LinkAttrib(this->vertexBuffer, 0, 2, GL_FLOAT, sizeof(Vertex), (void*)0);		// Give indexes (x,y)
			this->vertexArray.LinkAttrib(this->vertexBuffer, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(2 * sizeof(float)));	// Give colors (r, g, b)
	
		}

		void draw()
		{
		
			this->vertexArray.Bind();
			glDrawElements(GL_TRIANGLES, this->expansionStep, GL_UNSIGNED_INT, 0);

		}

		bool checkPressed()
		{

			// Check if open/close button is clicked which will always be 0th in UI.
			if (expandButton.isPressed())
			{

				if ((this->expanded = (!this->expanded))) {
					this->expansionStep = this->wireFrame.indices.size();

				}
				else {
					this->expansionStep = 6;

				}

				this->vertexBuffer.Bind();
				this->expandButton.shift();

				userCursor._leftClick = false;

				return true;

			}

			if(this->expanded){
				this->vertexBuffer.Bind();
				this->canvas.checkButtons();
			}

			return false;

		}

		bool isExpanded()
		{

			return this->expanded;

		}	

};



int main()
{

	// Set openGL context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//		Create user window			  WIDTH | HEIGHT
	GLFWwindow* window = glfwCreateWindow(1900, 1900, "Sand-Particle Simulator", NULL, NULL);

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


	//Users sight	WIDTH | HEIGHT
	glViewport(0, 0, 1900, 1900);

	// Create our renderer
	Shader shaderProg("default.vert", "default.frag");
	
	// Bind up context.
	shaderProg.Activate();


	// Counters for frame-rate.
	std::chrono::duration<double> frameDuration(1.0 / 120), sleepTime;
	std::chrono::steady_clock::time_point frameStart, frameEnd;
	std::chrono::milliseconds frameTime;

	UI_ElementSelector elementSlidingPanel = UI_ElementSelector();

	World gaemWorld = World();

	while (!glfwWindowShouldClose(window)) {

		// Start game frame.
		frameStart = std::chrono::high_resolution_clock::now();

		// Clear buffer to now be drawn on.
		glClearColor(0.0f, 0.0f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		
		gaemWorld.react();
		
		elementSlidingPanel.draw();
		
	
		glfwSwapBuffers(window);
		glfwPollEvents();


		// End game frame.
		frameEnd = std::chrono::high_resolution_clock::now();
		frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
		if (frameTime < frameDuration) {	// Just Gonna Check User Input Every Cycle

			// ~~~~~~~~~~~~~~~ Cell Spawn Checks ~~~~~~~~~~~~~~~~~~~~~~~~~

			if (userCursor._leftClick) {

				if (!elementSlidingPanel.checkPressed()) {

					// If We Closed Our GUI Window (User Has Selected Something To Spawn)
					if (!elementSlidingPanel.isExpanded()) {

						// If Currently Holding A Throwable.
						if (!gaemWorld.entity_Batch.empty()) {
		
							for(unsigned int i = 0; i < gaemWorld.entity_Batch.size(); i++){

								switch (gaemWorld.entity_Batch[i]->getTypeID()) {

									case 'R':
										((Dynamic_Entity_Throwable_Rock*&)gaemWorld.entity_Batch[i])->move(gaemWorld.CELL_GRID, userCursor.c, userCursor.r);	// Need to dynamically pick our move for type
										break;
									case 'B':
										((Dynamic_Entity_Throwable_Boulder*&)gaemWorld.entity_Batch[i])->move(gaemWorld.CELL_GRID, userCursor.c, userCursor.r);	// Need to dynamically pick our move for type
										break;
									case 'X':
										((Dynamic_Entity_Throwable_Bombaclat*&)gaemWorld.entity_Batch[i])->move(gaemWorld.CELL_GRID, userCursor.c, userCursor.r);	// Need to dynamically pick our move for type
										break;

								}
							}
						}
						else {

							// Check If We Are Filling In This Shape
							if (userCursor._fillIn) {

								switch (userCursor.genID) {
									case 'S':
										gaemWorld.drawCircle(userCursor.c, userCursor.r - 11, userCursor.radii, 'S');
										break;
									case 'B':
										gaemWorld.drawCircle(userCursor.c, userCursor.r, userCursor.radii, 'B');
										break;
									case 'G':
										gaemWorld.drawCircle(userCursor.c, userCursor.r, userCursor.radii, 'G');
										break;
									case 'D':
										gaemWorld.drawCircle(userCursor.c, userCursor.r, userCursor.radii, 'D');
										break;
									case 'W':
										gaemWorld.drawCircle(userCursor.c, userCursor.r, userCursor.radii, 'W');
										break;

								}

							}
							else {	// Or Not...

								switch (userCursor.genID) {
									case 'S':
										gaemWorld.drawFillCircle(userCursor.c, userCursor.r - 11, userCursor.radii, 'S');
										break;
									case 'B':
										gaemWorld.drawFillCircle(userCursor.c, userCursor.r, userCursor.radii, 'B');
										break;
									case 'G':
										gaemWorld.drawFillCircle(userCursor.c, userCursor.r, userCursor.radii, 'G');
										break;
									case 'D':
										gaemWorld.drawFillCircle(userCursor.c, userCursor.r, userCursor.radii, 'D');
										break;
									case 'W':
										gaemWorld.drawFillCircle(userCursor.c, userCursor.r, userCursor.radii, 'W');
										break;

								}

							}

							// Check Entity Spawns
							switch (userCursor.genID) {

								case 'K':
									//gaemWorld.entity_Batch.push_back(new Dynamic_Throwable_Entity_Bombaclat(gaemWorld.CELL_GRID, userCursor.c, userCursor.r));
									gaemWorld.entity_Batch.push_back(new Dynamic_Entity_Throwable_Bombaclat(gaemWorld.CELL_GRID, userCursor.c, userCursor.r));
									//userCursor._leftClick = false;
									break;
								case 'T':
									gaemWorld.entity_Batch.push_back(new Dynamic_Entity_Throwable_Boulder(gaemWorld.CELL_GRID, userCursor.c, userCursor.r));
									break;
								case 'P':
									gaemWorld.entity_Batch.push_back(new Dynamic_Entity_Throwable_Rock(gaemWorld.CELL_GRID, userCursor.c, userCursor.r));
									break;



							}

						}

					}

				}

			}


			// ~~~~~~~~~~~~END Cell Spawn Checks ~~~~~~~~~~~~~~~~~~~~~~~~~
			std::this_thread::sleep_for((frameDuration - frameTime));
		}

	}

	shaderProg.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}



