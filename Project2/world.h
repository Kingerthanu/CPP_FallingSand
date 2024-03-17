#ifndef WORLD_H
#define WORLD_H

#include "dynamicCell.h"
#include "Vertex_Array.h"
#include "Element_Buffer.h"

class World
{

	public:

		// Postcondition:  Will return a integer value of our indices count for openGL draw.
		int generateCells()
		{

			// Container for our verts and indices to be given to our 2D map.
			std::vector<Vertex> vertices;
			std::vector<GLuint> indices;

			// Calculate size of our cells based on length (normalized from -1 -> 1 and cell count
			float cellHeight = 2.0f / MAX_CELLS;
			float cellWidth = 2.0f / MAX_CELLS;


			this->CELL_GRID = new Dynamic_Cell * *[MAX_CELLS];

			for (unsigned int i = 0; i < MAX_CELLS; ++i) {	// Set rows.

				this->CELL_GRID[i] = new Dynamic_Cell * [MAX_CELLS];

				for (unsigned int j = 0; j < MAX_CELLS; ++j) {	// Set columns

					this->CELL_GRID[i][j] = new Dynamic_Cell(vertices.size());

					// Calculate indices for the four vertices of each square
					int bottomLeft = vertices.size();
					int bottomRight = bottomLeft + 1;
					int topLeft = bottomRight + 1;
					int topRight = topLeft + 1;

					// Push vertices for the square with color (assuming it's white)
					vertices.push_back(Vertex{ glm::vec2(-1.0f + (cellWidth * j), -1.0f + (cellHeight * i)), glm::vec3(0.0f, 0.0f, 0.0f) });
					vertices.push_back(Vertex{ glm::vec2(-1.0f + (cellWidth * (j + 1)), -1.0f + (cellHeight * i)), glm::vec3(0.0f, 0.0f, 0.0f) });

					vertices.push_back(Vertex{ glm::vec2(-1.0f + (cellWidth * j), -1.0f + (cellHeight * (i + 1))), glm::vec3(0.0f, 0.0f, 0.0f) });
					vertices.push_back(Vertex{ glm::vec2(-1.0f + (cellWidth * (j + 1)), -1.0f + (cellHeight * (i + 1))), glm::vec3(0.0f, 0.0f, 0.0f) });


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
			this->VBO1 = VBO(vertices);
			this->EBO1 = EBO(indices);

			// Return size of indices we will be using
			return indices.size();

		};


		// Set up map grid and index count.
		Dynamic_Cell*** CELL_GRID;
		std::vector<Dynamic_Entity*> entity_Batch;
		VAO VAO1;
		VBO VBO1;
		EBO EBO1;
		int INDEX_COUNT;

		void checkPoint(const int& x, const int& y, const char& typeID) {
			if (x > -1 && x < MAX_CELLS && y > -1 && y < MAX_CELLS) {

				if (typeID == 'D') {

					int oldOffset = this->CELL_GRID[y][x]->offset;
					delete this->CELL_GRID[y][x];
					this->CELL_GRID[y][x] = new Dynamic_Cell(oldOffset);
					return;

				}

				if (this->CELL_GRID[y][x]->typeID == 'D') {
					// Perform some action if the cell is of type 'D'
					int oldOffset = this->CELL_GRID[y][x]->offset;
					delete this->CELL_GRID[y][x];

					switch (typeID) {

					case 'G':
						this->CELL_GRID[y][x] = new Dynamic_Smoke_Cell(oldOffset);
						return;
					case 'B':
						this->CELL_GRID[y][x] = new Dynamic_Solid_Cell(oldOffset);
						return;
					case 'S':
						this->CELL_GRID[y][x] = new Dynamic_Sand_Cell(oldOffset);
						return;
					case 'W':
						this->CELL_GRID[y][x] = new Dynamic_Water_Cell(oldOffset);
						return;


					}
				}
			}
		}

		World()
		{

			this->INDEX_COUNT = generateCells();
			VAO1.Bind();

			VAO1.LinkAttrib(VBO1, 0, 2, GL_FLOAT, sizeof(Vertex), (void*)0);		// Give indexes (x,y)
			VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(2 * sizeof(float)));	// Give colors (r, g, b)

			VAO1.unBind();

		}

		void react()
		{

			VAO1.Bind();
			VBO1.Bind();
			EBO1.Bind();


			// Grab buffer of data for cell color updates.
			Vertex* bufferData = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

			for (int i = 0; i < MAX_CELLS; ++i) { // For each row (down to up)
				for (int j = 0; j < MAX_CELLS; ++j) { // For each column (left to right)

					// If cell has not been drawn (color been updated) update.
					if (!CELL_GRID[i][j]->getState()) {

						CELL_GRID[i][j]->turnOn(bufferData);

					}

					CELL_GRID[i][j]->think(CELL_GRID, i, j);	// Each cell will think uniquely depending on definition (not abstract, still check empty cells [room for optimization]).

				}
			}

			glUnmapBuffer(GL_ARRAY_BUFFER);	// Stop using our passed buffer.


			for (unsigned int i = 0; i < entity_Batch.size(); i++) {

				switch(entity_Batch[i]->react()){

					case 'X':
						int x = entity_Batch[i]->getCenterX();
						int y = entity_Batch[i]->getCenterY();
						explodeAt(x, y, 25);

				};

			}


			

			glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_INT, 0);

		}


		void drawFillCircle(const int& centerX, const int& centerY, const int& radius, const char& typeID = 'B') {

			int minX = std::max(0, centerX - radius);
			int maxX = std::min(MAX_CELLS - 1, centerX + radius);
			int minY = std::max(0, centerY - radius);
			int maxY = std::min(MAX_CELLS - 1, centerY + radius);

			int radiusSquared = radius * radius;

			for (int y = minY; y <= maxY; ++y) {
				for (int x = minX; x <= maxX; ++x) {
					int dx = x - centerX;
					int dy = y - centerY;
					if (dx * dx + dy * dy <= radiusSquared) {
						checkPoint(x, y, typeID);
					}
				}
			}
		}


		void drawCircle(const int& centerX, const int& centerY, const int& radius, const char& typeID = 'B') {
			int x = radius;
			int y = 0;
			int err = 0;

			while (x >= y) {

				// Check all eight symmetrical points around the center
				checkPoint(centerX + x, centerY + y, typeID);
				checkPoint(centerX + y, centerY + x, typeID);
				checkPoint(centerX - y, centerY + x, typeID);
				checkPoint(centerX - x, centerY + y, typeID);
				checkPoint(centerX - x, centerY - y, typeID);
				checkPoint(centerX - y, centerY - x, typeID);
				checkPoint(centerX + y, centerY - x, typeID);
				checkPoint(centerX + x, centerY - y, typeID);

				if (err <= 0) {
					y += 1;
					err += 2 * y + 1;
				}

				else if (err > 0) {
					x -= 1;
					err -= 2 * x + 1;
				}

			}
		}

		void explodeAt(const int& centerX, const int& centerY, const int& radius)
		{

			int minX = std::max(0, centerX - radius);
			int maxX = std::min(MAX_CELLS - 1, centerX + radius);
			int minY = std::max(0, centerY - radius);
			int maxY = std::min(MAX_CELLS - 1, centerY + radius);

			int radiusSquared = radius * radius;

			for (int y = minY; y <= maxY; ++y) {
				for (int x = minX; x <= maxX; ++x) {
					int dx = x - centerX;
					int dy = y - centerY;
					if (dx * dx + dy * dy <= radiusSquared) {
						

						if (x > -1 && x < MAX_CELLS && y > -1 && y < MAX_CELLS) {

								if (this->CELL_GRID[y][x]->typeID == 'B') {

									((Dynamic_Solid_Cell*&)this->CELL_GRID[y][x])->absorbExplosion();
									continue;
								}

								int oldOffset = this->CELL_GRID[y][x]->offset;
								delete this->CELL_GRID[y][x];

								if (rand() % 100 < 10) {

									if (rand() % 10 == 1) {
										this->CELL_GRID[y][x] = new Dynamic_Shrapnel_Cell(oldOffset, dx, dy);
									}
									else {
										this->CELL_GRID[y][x] = new Dynamic_Smoke_Cell(oldOffset, dx, dy);
										
									}

									
								}
								else {
									
									this->CELL_GRID[y][x] = new Dynamic_Cell(oldOffset);
									
								}
								

						}
					}
				}
			}
			


		};

};

#endif