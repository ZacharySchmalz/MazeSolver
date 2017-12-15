// Zachary Schmalz
// December 9, 2017
// Maze.cpp

#include <iostream>		// standard I/O
#include <thread>		// mainly used by the "computing" function
#include <iomanip>		// increase timing precision
#include <stack>		// used for stack-implemented searching algorithms
#include <queue>		// used in breadth first search algorithm
#include <cmath>		// math functions
#include "Maze.h"		// function declarations

typedef std::chrono::high_resolution_clock Clock;

// Constructor that takes the name of the image as a string parameter
Maze::Maze()
{

}

// Initialize the maze
bool Maze::initialize(std::string image)
{
	// Load the image
	isOperationComplete = false;

	// Retrieve result of imageLoad function
	std::promise<bool> result;
	auto operation = result.get_future();

	std::thread info_t1(&Maze::computing, this, "Loading Image");
	std::thread operation_t1(&Maze::imageLoad, this, image, std::move(result));
	info_t1.join();
	operation_t1.join();

	// Return if the image was not loaded correctly
	if (!operation.get())
		return false;

	std::cout << "\nImage Loaded\t\t\t" << std::setprecision(9) << time << " seconds\n" << std::endl;

	// Create the maze
	std::cout << "Creating Maze." << std::endl;
	isOperationComplete = false;
	std::thread operation_t2(&Maze::createMaze, this);
	std::thread info_t2(&Maze::computing, this, "Node Creation");
	info_t2.join();
	operation_t2.join();
	std::cout << "\rNodes Created\t\t\t" << nodeCount << " nodes" << std::endl;
	std::cout << "Maze Created\t\t\t" << std::setprecision(9) << time << " seconds\n" << std::endl;
	return true;
}

// Solves the maze using node data and the algorithm type
void Maze::solve(int algorithm)
{
	// Reset all values 
	isOperationComplete = false;
	nodesVisited = 0;
	pathNodeLength = 0;
	pathPixelLength = 0;

	switch (algorithm)
	{
		case 1:
		{
			std::cout << "\nSolving Maze using DFS:" << std::endl;
			std::thread info_t1(&Maze::computing, this, "Searching for path");
			std::thread operation_t1(&Maze::depthFirstSearch, this);
			operation_t1.join();
			info_t1.join();
			break;
		}

		case 2:
		{
			std::cout << "\nSolving Maze using BFS:" << std::endl;
			std::thread info_t1(&Maze::computing, this, "Searching for path");
			std::thread operation_t1(&Maze::breadthFirstSearch, this);
			operation_t1.join();
			info_t1.join();
			break;
		}
	}

	std::cout << "\rNodes Visited\t\t\t" << nodesVisited << " nodes" << std::endl;
	std::cout << "Path Length\t\t\t" << pathNodeLength << " nodes | " << pathPixelLength << " pixels" << std::endl;
	std::cout << "\rPath Found\t\t\t" << std::setprecision(9) << time << " seconds\n" << std::endl;

	// Ask user to save image or not
	std::string save = "";
	std::cout << "Save Result? Y/N" << std::endl;
	std::cin >> save;
	while (save != "Y" && save != "y" && save != "N" && save != "n")
	{
		std::cout << "Save Image? Y/N" << std::endl;
		std::cin >> save;
	}

	// Edit the original image with the path and save
	if (save == "Y" || save == "y")
	{
		std::string solutionName = "";
		std::cout << "\nEnter a file name: ";
		std::cin >> solutionName;
		solutionName += ".bmp";
		isOperationComplete = false;
		std::thread info_t1(&Maze::computing, this, "Saving " + solutionName);
		std::thread operation_t1(&Maze::imageSave, this, solutionName);
		operation_t1.join();
		info_t1.join();
		std::cout << "\nImage Saved\t\t\t" << std::setprecision(9) << time << " seconds\n" << std::endl;
	}
}

// Function that loads the maze image into memory
void Maze::imageLoad(std::string image, std::promise<bool> && p)
{
	auto startClock = Clock::now();																// Begin timer
	// Check that file exists
	try{Maze::mazeImage = CImg<int>(image.c_str());}
	catch (CImgIOException e) { p.set_value(false);  isOperationComplete = true;  return; }
	width = mazeImage.width();
	height = mazeImage.height();
	mazeVector.resize(height, std::vector<Node*>(width));										// Resize to size of maze image
	p.set_value(true);
	endOperation(startClock);
}

// Scan the maze image and allocate node structures in memory
void Maze::createMaze()
{
	auto startClock = Clock::now();

	// Manually allocate Start node.
	for (int i = 0; i < mazeVector[0].size(); i++)
	{
		if (mazeImage(i, 0, 0, 0) == 255)
		{
			mazeVector[0][i] = new Node;
			mazeVector[0][i]->posX = i;
			mazeVector[0][i]->posY = 0;
			start = mazeVector[0][i];
			nodeCount++;
			break;
		}
	}

	// Manually allocate End node
	for (int i = 0; i < mazeVector[height - 1].size(); i++)
	{
		if (mazeImage(i, height - 1, 0, 0) == 255)
		{
			mazeVector[height - 1][i] = new Node;
			mazeVector[height-1][i]->posX = i;
			mazeVector[height-1][i]->posY = height-1;
			end = mazeVector[height - 1][i];
			nodeCount++;
			break;
		}
	}

	// Allocate all other nodes
	// Start and end nodes are not visited since the algorithm would not assign data to them.
	// Start node neighbors are assigned since it scans up->down and left->right, but end node neighbors aren't assigned
	for (int y = 1; y < mazeVector.size() - 1; y++)
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

				// If no more than 1 direction to go, don't create a node
				if (directions < 2)
					mazeVector[y][x] = NULL;

				// If theres 2 directions to go, check that they are not on the same axis, otherwise don't place a node
				else if (directions == 2)
				{
					if (!(north && south) && !(west && east))
					{
						mazeVector[y][x] = new Node;
						mazeVector[y][x]->posX = x;
						mazeVector[y][x]->posY = y;
						nodeCount++;
					}
					else
						mazeVector[y][x] = NULL;
				}

				// If theres 3 directions to go, then there must be a decision point so place a node
				else
				{
					mazeVector[y][x] = new Node;
					mazeVector[y][x]->posX = x;
					mazeVector[y][x]->posY = y;
					nodeCount++;
				}

				// If node was created, link to previous nodes that have been assigned.
				if (mazeVector[y][x])
				{
					// Check for nodes north of current node
					int range = y - 1;
					while (range >= 0 && mazeImage(x, range, 0, 0) == 255)
					{
						if (mazeVector[range][x] != NULL)
						{
							mazeVector[y][x]->neighbors.push_back(mazeVector[range][x]);
							mazeVector[range][x]->neighbors.push_back(mazeVector[y][x]);
							break;
						}
						else
							range--;
					}

					// Check west pixels for a path
					range = x - 1;
					while (range > 0 && mazeImage(range, y, 0, 0) == 255)
					{
						if (mazeVector[y][range] != NULL)
						{
							mazeVector[y][x]->neighbors.push_back(mazeVector[y][range]);
							mazeVector[y][range]->neighbors.push_back(mazeVector[y][x]);
							break;
						}
						else
							range--;
					}
				}
			}
		}
	}
	
	// Manually assign neighbors for end node since the above loop does not visit the end node to assign the neighbors
	int range = end->posY - 1;
	while (range >= 0 && mazeImage(end->posX, range, 0, 0) == 255)
	{
		if (mazeVector[range][end->posX] != NULL)
		{
			end->neighbors.push_back(mazeVector[range][end->posX]);
			mazeVector[range][end->posX]->neighbors.push_back(end);
			break;
		}
		else
			range--;
	}
	endOperation(startClock);
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
			// Parent node is south
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
	mazeImage.save(solutionName.c_str());

	endOperation(startClock);
}

// Stack implementation of Depth First Search algorithm
void Maze::depthFirstSearch()
{
	auto startClock = Clock::now();

	std::stack<Node*> stack;										// Initialize stack
	std::vector< std::vector<bool> > visited(height, std::vector<bool>(width, 0));

	stack.push(start);												// Push start node onto stack
	Node *v = NULL;
	while (!stack.empty())
	{
		v = stack.top();
		nodesVisited++;
		stack.pop();												// Pop off of stack
		visited[v->posY][v->posX] = true;							// Mark the node visited
		if (v == end)												// End node reached, break out of loop
		{
			break;
		}

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
	
	endOperation(startClock);
}

// Breadth First Search Implementation algorithm
void Maze::breadthFirstSearch()
{
	auto startClock = Clock::now();

	std::queue<Node*> queue;
	std::vector< std::vector<bool> > visited(height, std::vector<bool>(width, 0));

	visited[start->posY][start->posX] = true;
	queue.push(start);
	nodesVisited++;

	while (!queue.empty())
	{
		Node *s = queue.front();
		queue.pop();
		if (s == end)
			break;

		for each (Node* n in s->neighbors)
		{
			if (!visited[n->posY][n->posX])
			{
				visited[n->posY][n->posX] = true;
				nodesVisited++;
				n->parent = s;
				queue.push(n);
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

	endOperation(startClock);
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
			if (i == 4)
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

// Ends a stage of execution and calculates time
void Maze::endOperation(std::chrono::time_point<std::chrono::steady_clock> startClock)
{
	isOperationComplete = true;
	auto endClock = Clock::now();
	time = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(endClock - startClock).count() / 1000000000;
	totalTime += time;
}