#ifndef DYNAMIC_CELL_H
#define DYNAMIC_CELL_H

#include "Vertex.h"

class Dynamic_Cell
{

	protected:

		bool state = false;	// If drawn.

	public:

		bool getState() const
		{

			return this->state;

		};

		char typeID = 'D';	// Identifier tag of dynamic cells.
		const int offset;	// Offset in array

		// Action function for all dynamic classes; should be overridden with unique for child. Dynamic cell shouldn't do anything.
		virtual void think(Dynamic_Cell***& cellMap, const int& r, int& c) {};

		Dynamic_Cell() : offset(0){};
		Dynamic_Cell(const int& newOffset) : offset(newOffset) {};

		// Turn on state to say we have drawn a dynamic cell.
		void spark()
		{

			this->state = true;

		};

		// A dynamic cell will have no color, zero previous color on cell.
		virtual void turnOn(Vertex*& bufferData)
		{

			// Update all vertices in the buffer
			for (int i = this->offset, newOffset = this->offset + 4; i < newOffset; ++i) {

				bufferData[i].color *= 0.0f;

			}

			// Now say we have drawn it.
			this->state = true;

		};

};

class Dynamic_Solid_Cell : public Dynamic_Cell
{


	public:
		Dynamic_Solid_Cell(int newOffset) : Dynamic_Cell(newOffset)
		{
			// B for solid block.
			this->typeID = 'B';

		};


		// Solid Cell will inherit the empty think method of dynamic cell...


		// Static color filling of vertices.
		void turnOn(Vertex*& bufferData) override
		{

			// Update all vertices in the buffer
			for (int i = this->offset, newOffset = this->offset + 4; i < newOffset; ++i) {

					bufferData[i].color = glm::vec3(0.5f, 0.5f, 0.59f);

			}


			// Now say we have drawn it.
			this->state = true;

		};

};

class Dynamic_NonSolid_Cell : public Dynamic_Cell
{

	protected:
		// { X-Axis Velocity, Y-Axis Velocity }
		float velocity[2] = { 0.0f, 0.0f };

		// Mass for energy conversion that all falling, moving cells will share.
		float mass = 0.0f;
		
		char traverseWithVelocity(Dynamic_Cell***& cellMap, const int& r, const int& c) {

			if (!((int)velocity[0]) && !((int)velocity[1])) {
				return '0';

			}

			// Determine the direction of movement (horizontal and vertical)
			int _negX = (c + velocity[0] > c) ? 1 : ((c + velocity[0] < c) ? -1 : 0);
			int _negY = (r + velocity[1] > r) ? 1 : ((r + velocity[1] < r) ? -1 : 0);
			 
			//std::cout << _negX << ' ' << _negY << "  |  " << velocity[0] << ' ' << velocity[1] << '\n';

			int runVelocity[2] = { _negX, _negY };

			// Iterate until we hit an obstacle or reach the boundary
			while (abs(runVelocity[1]) <= abs(velocity[1]) && abs(runVelocity[0]) <= abs(velocity[0]))  {
				
				// Check if we are out of bounds
				if (c + runVelocity[0] < 0 || r + runVelocity[1] < 0 || c + runVelocity[0] >= MAX_CELLS || r + runVelocity[1] >= MAX_CELLS) {

					velocity[0] = runVelocity[0] - _negX;
					velocity[1] = runVelocity[1] - _negY;
					return '0';
				}
				
				Dynamic_Cell* currentCell = cellMap[r + runVelocity[1]][c + runVelocity[0]];

				// Check for obstacle using the think function of the cell
				if (currentCell->typeID != 'D') {
					
					// Hit an obstacle, stop traversal
					velocity[0] = runVelocity[0] - _negX;
					velocity[1] = runVelocity[1] - _negY;

					if (!((int)velocity[0]) && !((int)velocity[1])) {
						return 'S';

					}
					
					return 'C';
				}

				// Move according to velocity
				runVelocity[0] += _negX;
				runVelocity[1] += _negY;

			}


			if (abs(runVelocity[1]) < abs(velocity[1]))
			{

				runVelocity[0] -= _negX;

				while (abs(runVelocity[1]) < abs(velocity[1])) {

					// Check if we are out of bounds
					if (r + runVelocity[1] < 0 || r + runVelocity[1] >= MAX_CELLS) {

						velocity[1] = runVelocity[1] - _negY;
						return '0';
					}

					Dynamic_Cell* currentCell = cellMap[r + runVelocity[1]][c + runVelocity[0]];

					// Check for obstacle using the think function of the cell
					if (currentCell->typeID != 'D') {

						// Hit an obstacle, stop traversal
						velocity[1] = runVelocity[1] - _negY;

						if (!((int)velocity[1])) {
							return 'S';

						}

						return 'C';
					}

					// Move according to velocity
					runVelocity[1] += _negY;

				}


			}
			
			else if (abs(runVelocity[0]) < abs(velocity[0]))
			{

				runVelocity[1] -= _negY;

				while (abs(runVelocity[0]) <= abs(velocity[0])) {

					// Check if we are out of bounds
					if (c + runVelocity[0] < 0 || c + runVelocity[0] >= MAX_CELLS) {

						velocity[0] = runVelocity[0] - _negX;
						return '0';
					}

					Dynamic_Cell* currentCell = cellMap[r + runVelocity[1]][c + runVelocity[0]];

					// Check for obstacle using the think function of the cell
					if (currentCell->typeID != 'D') {

						// Hit an obstacle, stop traversal
						velocity[0] = runVelocity[0] - _negX;

						if (!((int)velocity[0])) {
							return 'S';

						}

						return 'C';
					}

					// Move according to velocity
					runVelocity[0] += _negX;

				}


			}


			return ' ';

		}
		
	public:

		// Should only set offset, shouldn't 
		Dynamic_NonSolid_Cell(int newOffset) : Dynamic_Cell(newOffset){};

		// Will share dynamic cell's think.
		
		virtual void turnOn(Vertex*& bufferData) override = 0;

};

class Dynamic_Water_Cell : public Dynamic_NonSolid_Cell
{

	private:
		/*
		
		bool falling = true;
		float velocity[2] = { 0.0f, 0.0f };
		
		*/
		
		// Flip will work to pick direction of travel of water droplet.
		bool flip = false;
	
	public:

		// Expect our offset in array; expects block velocity and our spill direction.
		Dynamic_Water_Cell(int newOffset, const float& velocityX = 0.0f, const float& velocityY = 1.0f, const bool& flipState = false) : Dynamic_NonSolid_Cell(newOffset)
		{

			velocity[0] = velocityX;
			velocity[1] = velocityY;
			this->flip = flipState;

			// W for water 
			this->typeID = 'W';

		};


		void think(Dynamic_Cell***& cellMap, const int& r, int& c) override
		{

			// If we can fall downwards by our given velocity.
			if (((r - velocity[1]) > -1)) {

				// Grab the directly cell below us.
				Dynamic_Cell*& handler = cellMap[(r - (int)velocity[1])][c];

				// If it is a dynamic cell (empty).
				if ((handler->typeID == 'D')) {

					// Swap our block with a new water block.
					int oldOffset = handler->offset;
					int ourOffset = cellMap[r][c]->offset;

					delete handler;

					handler = new Dynamic_Water_Cell(oldOffset, velocity[0], velocity[1] + 0.5f, this->flip);
					cellMap[r][c] = new Dynamic_Cell(ourOffset);

					// Then destruct ourselves to make sure we don't leak from 2D array.
					delete this;

					// Jump out.
					return;
				}
				else {	// Now check to the left and right of that block below us at that interval.


					// If block to right is free.
					if (((c + 1) < MAX_CELLS) && (cellMap[(r)][(c + 1)]->typeID == 'D' && (cellMap[(r - (int)velocity[1])][(c + 1)]->typeID == 'D'))) {


						// Swap with water.
						int oldOffset = cellMap[(r - (int)velocity[1])][(c + 1)]->offset;
						int ourOffset = cellMap[r][c]->offset;

						delete cellMap[(r - (int)velocity[1])][(c + 1)];

						cellMap[(r - (int)velocity[1])][(c + 1)] = new Dynamic_Water_Cell(oldOffset, velocity[0] + velocity[1] * (this->mass * 0.5f ), velocity[1], false);
						cellMap[r][c] = new Dynamic_Cell(ourOffset);

						// Delete ourselves.
						delete this;
						return;

					}
					// Else if block to the left is free.
					else if (((c - 1) > -1) && (cellMap[(r)][(c - 1)]->typeID == 'D' && (cellMap[(r - (int)velocity[1])][(c - 1)]->typeID == 'D'))) {

						int oldOffset = cellMap[(r - (int)velocity[1])][(c - 1)]->offset;
						int ourOffset = cellMap[r][c]->offset;

						delete cellMap[(r - (int)velocity[1])][(c - 1)];

						cellMap[(r - (int)velocity[1])][(c - 1)] = new Dynamic_Water_Cell(oldOffset, velocity[0] + velocity[1] * (this->mass * -0.5f), velocity[1], true);
						cellMap[r][c] = new Dynamic_Cell(ourOffset);

						delete this;
						return;

					}

				}


			}

			// Else we are spilling.
			if (this->flip && ((c - 1) > -1))
			{

				// If we can spill left...
				if (cellMap[r][(c - 1)]->typeID == 'D') {


					Dynamic_Cell*& handler = cellMap[r][(c - 1)];

					int oldOffset = handler->offset;
					int ourOffset = cellMap[r][c]->offset;

					delete handler;

					handler = new Dynamic_Water_Cell(oldOffset, -1.0f, 1.0f, true);
					cellMap[r][c] = new Dynamic_Cell(ourOffset);

					delete this;
					c++;
					return;


				}

				

				

			}
			// If we can spill right...
			else if (!this->flip && (((c + 1) < MAX_CELLS)))
			{

				if (cellMap[r][(c + 1)]->typeID == 'D') {


					Dynamic_Cell*& handler = cellMap[r][(c + 1)];

					int oldOffset = handler->offset;
					int ourOffset = cellMap[r][c]->offset;

					delete handler;

					handler = new Dynamic_Water_Cell(oldOffset, 1.0f, 1.0f, false);
					cellMap[r][c] = new Dynamic_Cell(ourOffset);

					delete this;
					c++;
					return;


				}



			}

			// else if we cant move, do nothing.
			velocity[1] = 1.0f;

			return;

		};


		void turnOn(Vertex*& bufferData) override
		{


			// Update all vertices in the buffer
			for (int i = this->offset, newOffset = this->offset + 4; i < newOffset; ++i) {

				bufferData[i].color = glm::vec3(0.0f, 0.0f, 0.42f);  // Equivalent to += bufferData[i].color;
	
			}

			this->state = true;

		};


};

class Dynamic_Sand_Cell : public Dynamic_NonSolid_Cell
{

	public:
		Dynamic_Sand_Cell(int newOffset, const float& velocityX = 1.0f, const float& velocityY = -1.0f) : Dynamic_NonSolid_Cell(newOffset)
		{

			this->typeID = 'S';
			this->mass = 10.5f;
			velocity[0] = velocityX;
			velocity[1] = velocityY;


		};

		void think(Dynamic_Cell***& cellMap, const int& r, int& c) override
		{

			
			switch (this->traverseWithVelocity(cellMap, r, c)) {


				case ' ':
				{
					Dynamic_Cell*& handler = cellMap[(r + (int)velocity[1])][(c + (int)velocity[0])];
					int oldOffset = handler->offset;
					int ourOffset = cellMap[r][c]->offset;

					delete handler;

					handler = new Dynamic_Sand_Cell(oldOffset, this->velocity[0], this->velocity[1]);
					cellMap[r][c] = new Dynamic_Cell(ourOffset);

					delete this;
					return;
				}
				case '0':
				{

					if ((r - 1 + (int)velocity[1]) > -1) {

						if (cellMap[(r - 1 + (int)velocity[1])][(c + (int)velocity[0])]->typeID == 'D') {

							int oldOffset = cellMap[(r - 1 + (int)velocity[1])][(c + (int)velocity[0])]->offset;
							int ourOffset = cellMap[r][c]->offset;

							delete cellMap[(r - 1 + (int)velocity[1])][(c + (int)velocity[0])];

							cellMap[(r - 1 + (int)velocity[1])][(c + (int)velocity[0])] = new Dynamic_Sand_Cell(oldOffset, 0, -1.0f);
							cellMap[r][c] = new Dynamic_Cell(ourOffset);
							delete this;
							return;

						}
					}

					return;

				}
				case 'S':
				{

					if ((r - 1 + (int)velocity[1]) > -1) {

						if (c + 1 + (int)velocity[0] < MAX_CELLS && (cellMap[(r - 1 + (int)velocity[1])][(c + 1 + (int)velocity[0])]->typeID == 'D' && (cellMap[(r + (int)velocity[1])][(c + 1 + (int)velocity[0])]->typeID == 'D'))) {

							int oldOffset = cellMap[(r - 1 + (int)velocity[1])][(c + 1 + (int)velocity[0])]->offset;
							int ourOffset = cellMap[r][c]->offset;

							delete cellMap[(r - 1 + (int)velocity[1])][(c + 1 + (int)velocity[0])];

							cellMap[(r - 1 + (int)velocity[1])][(c + 1 + (int)velocity[0])] = new Dynamic_Sand_Cell(oldOffset, 0, -1.0f);
							cellMap[r][c] = new Dynamic_Cell(ourOffset);
							delete this;
							return;

						}
						else if (c - 1 + (int)velocity[0] > -1 && (cellMap[(r - 1 + (int)velocity[1])][(c - 1 + (int)velocity[0])]->typeID == 'D') && (cellMap[(r + (int)velocity[1])][(c - 1 + (int)velocity[0])]->typeID == 'D')) {

							int oldOffset = cellMap[(r - 1 + (int)velocity[1])][(c - 1 + (int)velocity[0])]->offset;
							int ourOffset = cellMap[r][c]->offset;

							delete cellMap[(r - 1 + (int)velocity[1])][(c - 1 + (int)velocity[0])];

							cellMap[(r - 1 + (int)velocity[1])][(c - 1 + (int)velocity[0])] = new Dynamic_Sand_Cell(oldOffset, 0, -1.0f);
							cellMap[r][c] = new Dynamic_Cell(ourOffset);

							delete this;
							return;

						}
					}

					return;

				}

				case 'C':
				{

					

					Dynamic_Cell*& handler = cellMap[(r + (int)velocity[1])][(c + (int)velocity[0])];
					int oldOffset = handler->offset;
					int ourOffset = cellMap[r][c]->offset;

					delete handler;

					handler = new Dynamic_Sand_Cell(oldOffset, 0.0f, 0.0f);
					cellMap[r][c] = new Dynamic_Cell(ourOffset);

					delete this;
					return;
				}

			}

			return;
			
			/*
				if(!(int)velocity[0] && !(int)velocity[1]){
				
					return;
				
				}

			
				if (((r - (int)velocity[1]) > -1) && (c - (int)velocity[0]) > -1) {

					Dynamic_Cell*& handler = cellMap[(r - (int)velocity[1])][(c - (int)velocity[0])];

					if ((handler->typeID == 'D')) {

						int oldOffset = handler->offset;
						int ourOffset = cellMap[r][c]->offset;

						delete handler;

						handler = new Dynamic_Sand_Cell(oldOffset, velocity[0], velocity[1] + (this->mass * 0.05f));
						cellMap[r][c] = new Dynamic_Cell(ourOffset);

						delete this;
						return;
					}
					else if ((handler->typeID == 'S')) {

						if ((cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] + 1)]->typeID == 'D')) {

							int oldOffset = cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] + 1)]->offset;
							int ourOffset = cellMap[r][c]->offset;

							delete cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] + 1)];

							cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] + 1)] = new Dynamic_Sand_Cell(oldOffset, 1.0f);
							cellMap[r][c] = new Dynamic_Cell(ourOffset);

							delete this;
							return;

						}
						else if ((cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] - 1)]->typeID == 'D')) {

							int oldOffset = cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] - 1)]->offset;
							int ourOffset = cellMap[r][c]->offset;

							delete cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] - 1)];

							cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] - 1)] = new Dynamic_Sand_Cell(oldOffset, -1.0f);
							cellMap[r][c] = new Dynamic_Cell(ourOffset);

							delete this;
							return;

						}


						

						


					}
					else if ((handler->typeID == 'W')) {

						int oldOffset = handler->offset;
						int ourOffset = cellMap[r][c]->offset;

						delete handler;

						handler = new Dynamic_Sand_Cell(oldOffset, 0.0f);
						cellMap[r][c] = new Dynamic_Water_Cell(ourOffset);

						delete this;
						return;

					}

				}

				this->velocity[0] *= 0.95f;
				this->velocity[1] *= 0.5f;

				//std::cout << offset << " Stuck :]\n";

				return;
				
				*/
		};

		void turnOn(Vertex*& bufferData) override
		{

			float flatColorSeed = ((rand() % MAX_CELLS) / (float)MAX_CELLS);
			glm::vec3 colorRGB = glm::vec3(flatColorSeed - 0.025f, flatColorSeed, flatColorSeed - 0.375f);

			// Update all vertices in the buffer
			for (unsigned int i = this->offset, newOffset = this->offset + 4; i < newOffset; ++i) {
					bufferData[i].color = colorRGB;

			}

			this->state = true;

		};

};

#endif
