// Zachary Schmalz
// June 6, 2017
// Maze.cpp

#include <iostream>		// standard I/O
#include <thread>		// mainly used by the "computing" function
#include <iomanip>		// increase timing precision
#include <stack>		// used for stack-implemented searching algorithms
#include <cmath>
#include "Maze.h"		// function declarations

typedef std::chrono::high_resolution_clock Clock;

// Constructor that takes the name of the image as a string parameter
Maze::Maze(std::string image)
{
	// Load the image
	isOperationComplete = false;
	std::thread info_t1(&Maze::computing, this, "Loading Image");
	std::thread operation_t1(&Maze::imageLoad, this, image);
	operation_t1.join();
	info_t1.join();
	std::cout << "\nImage Loaded\t\t\t" << std::setprecision(9) << time << " seconds\n" << std::endl;


	// Create maze
	std::cout << "Creating Maze." << std::endl;
	auto startClock = Clock::now();

	// Create the nodes
	isOperationComplete = false;
	std::thread operation_t2(&Maze::allocateNodes, this);
	std::thread info_t2(&Maze::computing, this, "Node Creation");
	info_t2.join();
	operation_t2.join();
	std::cout << "\rNodes Created\t\t\t" << nodeCount << " nodes" << std::endl;

	// Assign node data
	isOperationComplete = false;
	std::thread operation_t3(&Maze::assignNodeData, this);
	std::thread info_t3(&Maze::computing, this, "Assigning Node Data");
	info_t3.join();
	operation_t3.join();
	std::cout << "\rNode Data Assigned\t\t" << std::endl;

	auto endClock = Clock::now();
	time = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(endClock - startClock).count() / 1000000000;	// operation time in seconds with 9 digits of precision
	totalTime += time;
	std::cout << "Maze Created\t\t\t" << std::setprecision(9) << time << " seconds\n" << std::endl;
}

void Maze::solve(int algorithm, std::string solutionName)
{
	std::cout << "Searching Maze." << std::endl;

	// Find the path
	isOperationComplete = false;
	switch (algorithm)
	{
		case 1:
			std::thread info_t1(&Maze::computing, this, "Searching for path");
			std::thread operation_t1(&Maze::depthFirstSearch, this);
			operation_t1.join();
			info_t1.join();
			std::cout << "\rNodes Visited\t\t\t" << nodesVisited << " nodes" << std::endl;
			std::cout << "Path Length\t\t\t" << pathNodeLength << " nodes | " << pathPixelLength << " pixels" << std::endl;
			std::cout << "\rPath Found\t\t\t" << std::setprecision(9) << time << " seconds\n" << std::endl;
			break;
	}

	 // Edit the original image with the path and save
	isOperationComplete = false;
	std::thread info_t1(&Maze::computing, this, "Saving Image");
	std::thread operation_t1(&Maze::imageSave, this, solutionName);
	operation_t1.join();
	info_t1.join();
	std::cout << "\nImage Saved\t\t\t" << std::setprecision(9) << time << " seconds\n" << std::endl;
	std::cout << "Total Time\t\t\t" << std::setprecision(9) << totalTime << " seconds\n" << std::endl;

}

// Function that loads the maze image into memory
void Maze::imageLoad(std::string image)
{
	auto startClock = Clock::now();																// Begin timer
	mazeImage = CImg<unsigned char>(image.c_str());												// Intermediate CImg object
	width = mazeImage.width();
	height = mazeImage.height();
	mazeVector.resize(height, std::vector<Node*>(width));										// Resize to size of maze image
	visited.resize(height, std::vector<bool>(width, false));									// Resive to size of maze image
	auto endClock = Clock::now();																// End timer
	isOperationComplete = true;
	time = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(endClock - startClock).count() / 1000000000;	// operation time in seconds with 9 digits of precision
	totalTime += time;
}

// Scan the maze image and allocate node structures in memory
void Maze::allocateNodes()
{
	// Allocate and set start node
	for (int i = 0; i < mazeVector[0].size(); i++)
	{
		if (mazeImage(i, 0, 0, 0) == 255)
		{
			mazeVector[0][i] = new Node;
			start = mazeVector[0][i];
			nodeCount++;
			break;
		}
	}

	// Allocate and set end node
	for (int i = 0; i < mazeVector[height - 1].size(); i++)
	{
		if (mazeImage(i, height - 1, 0, 0) == 255)
		{
			mazeVector[height - 1][i] = new Node;
			end = mazeVector[height - 1][i];
			nodeCount++;
			break;
		}
	}

	// Allocate all other nodes
	for (int y = 1; y < mazeVector.size() - 1; y++)					// skips beginning and end nodes
	{
		for (int x = 0; x < mazeVector[y].size(); x++)
		{
			if (mazeImage(x, y, 0, 0) == 255)
			{
				int directions = 0;
				bool north = false, east = false, south = false, west = false;

				// Check north
				if (y-1 >= 0 && mazeImage(x, y - 1, 0, 0) == 255)
				{
					directions++;
					north = true;
				}
				// Check east
				if (x+1 < width && mazeImage(x + 1, y, 0, 0) == 255)
				{
					directions++;
					east = true;
				}
				// Check south
				if (y+1 <= height && mazeImage(x, y + 1, 0, 0) == 255)
				{
					directions++;
					south = true;
				}
				// Check west
				if (x-1 > 0 && mazeImage(x - 1, y, 0, 0) == 255)
				{
					directions++;
					west = true;
				}

				if (directions >= 2 && ((north == false && south == true) || (north == true && south == false) || (east == true && west == false) || (east == false && west == true || (north == true && east == true && south == true && west == true))))
				{
					mazeVector[y][x] = new Node;
					nodeCount++;
				}
				else
					mazeVector[y][x] = NULL;
			}
			
		}
	}
	isOperationComplete = true;
}

// Set the structure field data of each node
void Maze::assignNodeData()
{
	for (int y = 0; y < mazeVector.size(); y++)
	{
		for (int x = 0; x < mazeVector[y].size(); x++)
		{
			if (mazeVector[y][x] != NULL)
			{
				// Set node location
				mazeVector[y][x]->posX = x;
				mazeVector[y][x]->posY = y;

				// Check north pixel for a path
				//if (y - 1 >= 0 && mazeImage(x, y - 1, 0, 0) == 255)
					//mazeVector[y][x]->neighbors.push_back(mazeVector[y - 1][x]);
				int range = y - 1;
				while (range >= 0 && mazeImage(x, range, 0, 0) == 255)
				{
					if (mazeVector[range][x] != NULL)
					{
						mazeVector[y][x]->neighbors.push_back(mazeVector[range][x]);
						break;
					}
					else
						range--;
				}

				// Check east pixel for a path
				//if (x + 1 < width && mazeImage(x + 1, y, 0, 0) == 255)
					//mazeVector[y][x]->neighbors.push_back(mazeVector[y][x + 1]);
				range = x + 1;
				while (range < width && mazeImage(range, y, 0, 0) == 255)
				{
					if (mazeVector[y][range] != NULL)
					{
						mazeVector[y][x]->neighbors.push_back(mazeVector[y][range]);
						break;
					}
					else
						range++;
				}

				range = y + 1;
				while (range < height && mazeImage(x, range, 0, 0) == 255)
				{
					if (mazeVector[range][x] != NULL)
					{
						mazeVector[y][x]->neighbors.push_back(mazeVector[range][x]);
						break;
					}
					else
						range++;
				}

				// Check west pixel for a path
				//if (x - 1 > 0 && mazeImage(x - 1, y, 0, 0) == 255)
					//mazeVector[y][x]->neighbors.push_back(mazeVector[y][x - 1]);
				range = x - 1;
				while (range > 0 && mazeImage(range, y, 0, 0) == 255)
				{
					if (mazeVector[y][range] != NULL)
					{
						mazeVector[y][x]->neighbors.push_back(mazeVector[y][range]);
						break;
					}
					else
						range--;
				}
			}
		}
	}
	isOperationComplete = true;
}

// Color the path through the maze and save it
void Maze::imageSave(std::string solutionName)
{
	auto startClock = Clock::now();

	// Interpolate the path color from blue to red
	int r = 0;
	int i = 0;
	Node *t = end;
	// Trace path backwards from the end
	while(t->parent != NULL)
	{
		// Parent node lies on the X axis of mazeVector
		if (t->posX - t->parent->posX != 0)
		{
			int xDif = t->posX - t->parent->posX;
			// Parent node is west
			if (xDif > 0)
			{
				for (int j = 0; j < xDif; j++)
				{
					r = int((i * 255) / pathPixelLength);
					mazeImage(t->posX-j, t->posY, 0, 0) = 255 - r;
					mazeImage(t->posX-j, t->posY, 0, 1) = 0;
					mazeImage(t->posX-j, t->posY, 0, 2) = r;
					i++;
				}
			}
			// Parent node is east
			else
			{
				for (int j = 0; j < abs(xDif); j++)
				{
					r = int((i * 255) / pathPixelLength);
					mazeImage(t->posX+j, t->posY, 0, 0) = 255 - r;
					mazeImage(t->posX+j, t->posY, 0, 1) = 0;
					mazeImage(t->posX+j, t->posY, 0, 2) = r;
					i++;
				}
			}
		}
		// Parent node lies on the Y axis of mazeVector
		else
		{
			int yDif = t->posY - t->parent->posY;
			// Parent node is north
			if (yDif > 0)
			{
				for (int j = 0; j < yDif; j++)
				{
					r = int((i * 255) / pathPixelLength);
					mazeImage(t->posX, t->posY-j, 0, 0) = 255 - r;
					mazeImage(t->posX, t->posY-j, 0, 1) = 0;
					mazeImage(t->posX, t->posY-j, 0, 2) = r;
					i++;
				}
			}
			// Parent node is north
			else
			{
				for (int j = 0; j < abs(yDif); j++)
				{
					r = int((i * 255) / pathPixelLength);
					mazeImage(t->posX, t->posY+j, 0, 0) = 255 - r;
					mazeImage(t->posX, t->posY+j, 0, 1) = 0;
					mazeImage(t->posX, t->posY+j, 0, 2) = r;
					i++;
				}
			}
		}
		t = t->parent;
	}

	// Since start node does not have parent, manually color. Will be solid blue
	mazeImage(start->posX, start->posY, 0, 0) = 0;
	mazeImage(start->posX, start->posY, 0, 1) = 0;
	mazeImage(start->posX, start->posY, 0, 2) = 255;

	// Save the image
	mazeImage.save_bmp(solutionName.c_str());

	auto endClock = Clock::now();
	isOperationComplete = true;
	time = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(endClock - startClock).count() / 1000000000;
	totalTime += time;
}

// Function that displays currently executing operation
void Maze::computing(std::string message)
{
	bool flag = false;
	while (true)
	{
		std::cout << message;
		for (int i = 0; i < 5; i++)
		{
			if(i == 4)
				std::cout << "\b\b\b\b    \r";
			else
			{
				isOperationComplete == false ? Sleep(333) : flag = true;
				if (flag)
					break;
				std::cout << ".";
			}
		}
		if (flag)
			break;
	}
}

// Stack implementation of Depth First Search algorithm
void Maze::depthFirstSearch()
{
	auto startClock = Clock::now();

	std::stack<Node*> stack;										// Initialize stack
	stack.push(start);												// Push start node onto stack
	Node *v = NULL;
	while (!stack.empty())
	{
		v = stack.top();
		nodesVisited++;
		stack.pop();												// Pop off of stack
		visited[v->posY][v->posX] = true;							// Mark the node visited
		if (v == end)												// End node reached, break out of loop
			break;

		for (int i = 0; i < v->neighbors.size(); i++)				// Iterate through the node's neighbors
		{
			Node *t = v->neighbors[i];
			if (!visited[t->posY][t->posX])							// If the neighbor hasnt been visited, add node to the stack
			{
				t->parent = v;										// Assign the neighbors parent (the node the neighbor was reached from)
				stack.push(t);
			}
		}
	}

	// Calculate # of pixels to color
	Node *t = end;
	while (t->parent != NULL)
	{
		pathPixelLength += abs(t->posX - t->parent->posX);
		pathPixelLength += abs(t->posY - t->parent->posY);
		t = t->parent;
		pathNodeLength++;
	}
	pathPixelLength++;				// the start node is not covered above. Add it here
	pathNodeLength++;
	
	auto endClock = Clock::now();
	isOperationComplete = true;
	time = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(endClock - startClock).count() / 1000000000;
	totalTime += time;
}