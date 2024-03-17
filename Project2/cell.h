#ifndef CELL_H
#define CELL_H

#include "Vertex.h"
#include <glad/glad.h>

class Cell
{

	private:
		bool state = false;
		int offset;
		char NEIGHBORS_SIZE = 0;

	public:
		Cell** NEIGHBORS = new Cell*[0];
		

		Cell(const int& blockOffset)
		{

			this->offset = blockOffset;

		};

		void addNeighbor(Cell& newNeighbor)
		{

			Cell** tmp = new Cell*[NEIGHBORS_SIZE + 1];

			for (int index = 0; index < this->NEIGHBORS_SIZE; index++) {

				tmp[index] = this->NEIGHBORS[index];

			}

			tmp[this->NEIGHBORS_SIZE++] = &newNeighbor;

			delete[] this->NEIGHBORS;

			this->NEIGHBORS = tmp;


		};

		void think()
		{

			char amntAlive = 0;

			for (char i = 0; i < this->NEIGHBORS_SIZE; i++) {

				if (this->NEIGHBORS[i]->state) {

					amntAlive++;

				}

			}

			if (this->state) {

				if (amntAlive != 2 && amntAlive != 3) {

					this->state = false;

				}

			}
			else {

				if (amntAlive == 3) {

					this->state = true;

				}

			}

			this->turnOn();

		};

		void grabNeighbors(Cell***& cells, unsigned int& maxCells, unsigned int& row, unsigned int& col)
		{

			if ((int)(col - 1) > -1) {

				if ((int)(row - 1) > -1) {

					this->addNeighbor(*cells[row - 1][col - 1]);

				}

				this->addNeighbor(*cells[row][col - 1]);

				if ((row + 1) < maxCells) {
					this->addNeighbor(*cells[row + 1][col - 1]);
					this->addNeighbor(*cells[row + 1][col]);
				}

			}

			if ((col + 1) < maxCells) {

				if (row + 1 < maxCells) {
					this->addNeighbor(*cells[row + 1][col + 1]);
				}

				this->addNeighbor(*cells[row][col + 1]);

				if ((int)(row - 1) > -1) {
					this->addNeighbor(*cells[row - 1][col + 1]);
				}

			}

			if ((int)(row - 1) > -1) {
				this->addNeighbor(*cells[row - 1][col]);
			}

		};

		void spark()
		{

			this->state = true;

		};

		void turnOn()
		{

			Vertex* bufferData = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

			if (this->state) {

				// Check if the mapping was successful
				if (bufferData != nullptr) {

					// Update all vertices in the buffer
					for (int i = this->offset, newOffset = this->offset + 4; i < newOffset; i++) {
						bufferData[i].color = glm::vec3(0.53f, 0.21f, 0.86f);
					}

					// Unmap the buffer to apply the changes
					glUnmapBuffer(GL_ARRAY_BUFFER);
				}
			}
			else {	

				// Check if the mapping was successful
				if (bufferData != nullptr) {

					// Update all vertices in the buffer
					for (int i = this->offset, newOffset = this->offset + 4; i < newOffset; i++) {
						bufferData[i].color = glm::vec3(0.0f, 0.0f, 0.0f);
					}

					// Unmap the buffer to apply the changes
					glUnmapBuffer(GL_ARRAY_BUFFER);
				}

			}

		};

};

#endif
