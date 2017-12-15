// Zachary Schmalz
// December 9, 2017
// Maze.h

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <future>
#include "CImg.h"

using namespace cimg_library;

struct Node
{
	int posX;													// x-pixel coordinate of node location
	int posY;													// y-pixel coordinate of node location
	Node *parent = NULL;										// used in search algorithms to track which node the node was disocvered from
	std::vector<Node*> neighbors;								// node vector that contains this nodes neighboring nodes
};

class Maze
{
	public:
		// Public class members

		// Public class functions
		Maze();													// class constructor
		bool initialize(std::string image);						// initializes the maze, creating data etc.
		void solve(int algorithm);								// function that solves the maze using a user-specified algorithm

	private:
		// Private class members
		CImg<int> mazeImage;									// CImg object that contains the input image
		std::vector< std::vector<Node*> > mazeVector;			// 2D node vector that contains all nodes
		Node *start;											// start node
		Node *end;												// end node
		int nodeCount;											// total # of nodes created in the maze
		int nodesVisited;										// total # of nodes visited by a searching algorithm
		int pathNodeLength;										// total # of nodes that make up the maze solution
		int pathPixelLength;									// total # of pixels that make up the maze solution	
		int width;												// image height(pixels)
		int height;												// image width(pixels)
		double time;											// timing the lengths of operations
		double totalTime;										// timing of all operations
		bool isOperationComplete;								// signals if an executing operation is complete

		// Private class functions
		void imageLoad(std::string image, std::promise<bool> && p);// function that takes the name of input image as string param and loads the CImg object into memory
		void createMaze();										// function that allocates memory for node structures and assigns their data
		void imageSave(std::string solutionName);				// function that saves modified version of roginal maze with solution
		void computing(std::string message);					// function that displays currently executing operation
		void depthFirstSearch();								// function that solves the maze using DFS algorithm	
		void breadthFirstSearch();								// function that solves the maze using BFS algorithm
		void endOperation(std::chrono::time_point<std::chrono::steady_clock> startClock);	// function to trigger the end of an opeartion
};