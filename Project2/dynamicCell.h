#ifndef DYNAMIC_CELL_H
#define DYNAMIC_CELL_H

#include "Vertex.h"

class Dynamic_Cell
{

	protected:

		bool state = false;

	public:

		bool getState() const
		{

			return this->state;

		};

		char typeID = 'D';
		const int offset;

		virtual bool think(Dynamic_Cell***& cellMap, const int& r, const int& c) { return false; };

		Dynamic_Cell() : offset(0){};
		Dynamic_Cell(const int& newOffset) : offset(newOffset) {};

		void spark()
		{

			this->state = true;

		};

		virtual void turnOn(Vertex*& bufferData)
		{

			// Update all vertices in the buffer
			for (int i = this->offset, newOffset = this->offset + 4; i < newOffset; ++i) {

				bufferData[i].color *= 0.0f;

			}

			this->state = true;

		};

};

class Dynamic_Solid_Cell : public Dynamic_Cell
{


	public:
		Dynamic_Solid_Cell(int newOffset) : Dynamic_Cell(newOffset)
		{

			this->typeID = 'B';

		};

		//void think(Dynamic_Cell***& cellMap, const int& r, const int& c) override;

		void turnOn(Vertex*& bufferData) override
		{

			// Update all vertices in the buffer
			for (int i = this->offset, newOffset = this->offset + 4; i < newOffset; ++i) {

					bufferData[i].color = glm::vec3(0.5f, 0.5f, 0.59f);

			}

			this->state = true;

		};

};

class Dynamic_NonSolid_Cell : public Dynamic_Cell
{

	protected:
		float velocity[2] = { 0.0f, 0.0f };

	public:

		Dynamic_NonSolid_Cell(int newOffset) : Dynamic_Cell(newOffset)
		{

			//this->state = true;
			//this->typeID = 'N';

		};

		//virtual void think(Dynamic_Cell***& cellMap, const int& r, const int& c) override {};

		virtual void turnOn(Vertex*& bufferData) override {};

};

class Dynamic_Water_Cell : public Dynamic_NonSolid_Cell
{

	private:
		/*
		
		bool falling = true;
		float velocity[2] = { 0.0f, 0.0f };
		
		*/
		bool flip = false;
	
	public:

		Dynamic_Water_Cell(int newOffset, const float& velocityX = 0.0f, const float& velocityY = 1.0f, const bool& flipState = false) : Dynamic_NonSolid_Cell(newOffset)
		{

			velocity[0] = velocityX;
			velocity[1] = velocityY;
			this->flip = flipState;
			this->typeID = 'W';

		};


		bool think(Dynamic_Cell***& cellMap, const int& r, const int& c) override
		{



			if (((r - velocity[1]) > -1)) {

				Dynamic_Cell*& handler = cellMap[(r - (int)velocity[1])][c];

				if ((handler->typeID == 'D')) {

					int oldOffset = handler->offset;
					int ourOffset = cellMap[r][c]->offset;

					delete handler;

					handler = new Dynamic_Water_Cell(oldOffset, velocity[0], velocity[1] + 0.02f, this->flip);
					cellMap[r][c] = new Dynamic_Cell(ourOffset);

					delete this;
					return false;
				}
				else {

					if (((c + 1) < MAX_CELLS) && (cellMap[(r)][(c + 1)]->typeID == 'D' && (cellMap[(r - (int)velocity[1])][(c + 1)]->typeID == 'D'))) {

						int oldOffset = cellMap[(r - (int)velocity[1])][(c + 1)]->offset;
						int ourOffset = cellMap[r][c]->offset;

						delete cellMap[(r - (int)velocity[1])][(c + 1)];

						cellMap[(r - (int)velocity[1])][(c + 1)] = new Dynamic_Water_Cell(oldOffset, velocity[0], velocity[1], false);
						cellMap[r][c] = new Dynamic_Cell(ourOffset);

						delete this;
						return false;

					}
					else if (((c - 1) > -1) && (cellMap[(r)][(c - 1)]->typeID == 'D' && (cellMap[(r - (int)velocity[1])][(c - 1)]->typeID == 'D'))) {

						int oldOffset = cellMap[(r - (int)velocity[1])][(c - 1)]->offset;
						int ourOffset = cellMap[r][c]->offset;

						delete cellMap[(r - (int)velocity[1])][(c - 1)];

						cellMap[(r - (int)velocity[1])][(c - 1)] = new Dynamic_Water_Cell(oldOffset, velocity[0], velocity[1], true);
						cellMap[r][c] = new Dynamic_Cell(ourOffset);

						delete this;
						return false;

					}

				}


			}
			


			if (this->flip && ((c - 1) > -1))
			{

				if (cellMap[r][(c - 1)]->typeID == 'D') {


					Dynamic_Cell*& handler = cellMap[r][(c - 1)];

					int oldOffset = handler->offset;
					int ourOffset = cellMap[r][c]->offset;

					delete handler;

					handler = new Dynamic_Water_Cell(oldOffset, -1.0f, 1.0f, true);
					cellMap[r][c] = new Dynamic_Cell(ourOffset);

					delete this;
					return true;


				}

				

				

			}
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
					return true;


				}



			}

			velocity[0] = 0.0f;
			velocity[1] = 1.0f;

			//std::cout << offset << " Stuck :]\n";

			return false;

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
		Dynamic_Sand_Cell(int newOffset, const float& velocityX = 0.0f, const float& velocityY = 1.0f) : Dynamic_NonSolid_Cell(newOffset)
		{

			this->typeID = 'S';

			velocity[0] = velocityX;
			velocity[1] = velocityY;


		};

		bool think(Dynamic_Cell***& cellMap, const int& r, const int& c) override
		{

				if (((r - velocity[1]) > -1)) {

					Dynamic_Cell*& handler = cellMap[(r - (int)velocity[1])][(c - (int)velocity[0])];

					if ((handler->typeID == 'D')) {

						int oldOffset = handler->offset;
						int ourOffset = cellMap[r][c]->offset;

						delete handler;

						handler = new Dynamic_Sand_Cell(oldOffset, velocity[0], velocity[1] + 0.01f);
						cellMap[r][c] = new Dynamic_Cell(ourOffset);

						delete this;
						return false;
					}
					else if ((handler->typeID == 'S')) {

						if ((cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] + 1)]->typeID == 'D')) {

							int oldOffset = cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] + 1)]->offset;
							int ourOffset = cellMap[r][c]->offset;

							delete cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] + 1)];

							cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] + 1)] = new Dynamic_Sand_Cell(oldOffset, velocity[0], velocity[1] + 0.1f);
							cellMap[r][c] = new Dynamic_Cell(ourOffset);

							delete this;
							return true;

						}
						else if ((cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] - 1)]->typeID == 'D')) {

							int oldOffset = cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] - 1)]->offset;
							int ourOffset = cellMap[r][c]->offset;

							delete cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] - 1)];

							cellMap[(r - (int)velocity[1])][(c - (int)velocity[0] - 1)] = new Dynamic_Sand_Cell(oldOffset, velocity[0], velocity[1] + 0.1f);
							cellMap[r][c] = new Dynamic_Cell(ourOffset);

							delete this;
							return true;

						}

					}
					else if ((handler->typeID == 'W')) {

						int oldOffset = handler->offset;
						int ourOffset = cellMap[r][c]->offset;

						delete handler;

						handler = new Dynamic_Sand_Cell(oldOffset, velocity[0], velocity[1] + 0.01f);
						cellMap[r][c] = new Dynamic_Water_Cell(ourOffset);

						delete this;
						return false;

					}

				}

				velocity[1] = 1.0f;

				//std::cout << offset << " Stuck :]\n";

				return false;

		};

		void turnOn(Vertex*& bufferData) override
		{

			// Update all vertices in the buffer
			for (int i = this->offset, newOffset = this->offset + 4; i < newOffset; ++i) {
					bufferData[i].color = glm::vec3(0.75f, 0.75f, 0.25f);  // Equivalent to += bufferData[i].color;

			}

			this->state = true;

		};

};

#endif
