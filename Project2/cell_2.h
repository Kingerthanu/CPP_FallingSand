#ifndef CELL_2_H
#define CELL_2_H

#include "Vertex.h"
#include <glad/glad.h>

class Cell_Gen2
{

	private:
		bool state = false;
		int offset;
		char NEIGHBORS_SIZE = 0;

	public:
		bool** NEIGHBORS = nullptr;

		Cell_Gen2()
		{

			// ...

		};

		Cell_Gen2(const int& blockOffset)
		{

			this->offset = blockOffset;

		};

		void addNeighbor(Cell_Gen2& newNeighbor)
		{

			bool** tmp = new bool* [NEIGHBORS_SIZE + 1];

			for (int index = 0; index < this->NEIGHBORS_SIZE; index++) {

				tmp[index] = this->NEIGHBORS[index];

			}

			tmp[this->NEIGHBORS_SIZE++] = &newNeighbor.state;

			delete[] this->NEIGHBORS;

			this->NEIGHBORS = tmp;


		};

		void think()
		{

			char amntAlive = 0;

			for (char i = 0; i < this->NEIGHBORS_SIZE; i++) {
				if (*this->NEIGHBORS[i] == true) {
					amntAlive++;
				}
			}

			// Sierpinski Triangle rules
			if (this->state) {
				// Deactivation rule for active cells
				if (amntAlive > 3) {
					this->state = false;
				}
			}
			else {
				// Activation rule for inactive cells
				if (amntAlive == 3) {
					this->state = true;
				}
			}

		};


		void grabNeighbors(Cell_Gen2**& cells, const int& maxRow, const int& maxCol, const int& row, const int& col)
		{
			if ((int)(col - 1) > -1) {

				if ((int)(row - 1) > -1) {

					this->addNeighbor(cells[row - 1][col - 1]);

				}

				this->addNeighbor(cells[row][col - 1]);

				if ((row + 1) < maxRow) {
					this->addNeighbor(cells[row + 1][col - 1]);
					this->addNeighbor(cells[row + 1][col]);
				}

			}

			if ((col + 1) < maxCol) {

				if (row + 1 < maxRow) {
					this->addNeighbor(cells[row + 1][col + 1]);
				}

				this->addNeighbor(cells[row][col + 1]);

				if ((int)(row - 1) > -1) {
					this->addNeighbor(cells[row - 1][col + 1]);
				}

			}

			if ((int)(row - 1) > -1) {
				this->addNeighbor(cells[row - 1][col]);
			}

		};

		void spark()
		{

			this->state = true;

		};

		void turnOn(Vertex*& bufferData)
		{

			// Update all vertices in the buffer
			for (int i = this->offset, newOffset = this->offset + 4; i < newOffset; ++i) {

				if (this->state) { //  && bufferData[i].color.r <= 1.0f
					bufferData[i].color = glm::vec3(0.0085f, 0.0045f, 0.00042f) * 1000.0f;  // Equivalent to += bufferData[i].color;
				}
				else {
					bufferData[i].color *= 0.0f;
				}
			}

		};
};
#endif
