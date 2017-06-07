// Zachary Schmalz
// June 6, 2017
// Maze.cpp

#include <iostream>		// standard I/O
#include <thread>		// mainly used by the "computing" function
#include <chrono>		// used for calculating operation timing
#include <iomanip>		// increase timing precision
#include "Maze.h"		// function declarations

typedef std::chrono::high_resolution_clock Clock;

// Constructor that takes the name of the image as a string parameter
Maze::Maze(std::string image)
{
	isMazeLoaded = false;
	std::thread info(&Maze::computing, this, "Loading Maze");
	std::thread loader(&Maze::mazeLoad, this, image);

	info.join();
	loader.join();
}

// Function that loads the maze image into memory
void Maze::mazeLoad(std::string image)
{
	auto start = Clock::now();
	maze = CImg<unsigned char>(image.c_str());
	auto end = Clock::now();
	isMazeLoaded = true;
	std::cout << "\rMaze Loaded\t" << std::setprecision(9) << (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000000 << " seconds" << std::endl;
}

// Function that displays currently executing operation
void Maze::computing(std::string message)
{
	int flag = false;
	while (true)
	{
		std::cout << message;
		for (int i = 0; i < 5; i++)
		{
			if(i == 4)
				std::cout << "\b\b\b   \r";
			else
			{
				isMazeLoaded == false ? Sleep(333) : flag = true;
				if (flag)
					break;
				std::cout << ".";
			}
		}
		if (flag)
			break;
	}
}

int main()
{
	Maze m("big.bmp");
}