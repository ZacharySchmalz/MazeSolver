// Zachary Schmalz
// June 6, 2017
// Maze.h

#pragma once
#include <string>
#include "CImg.h"

using namespace cimg_library;

class Maze
{
	public:
		CImg<unsigned char> maze;
		Maze(std::string image);
	private:
		bool isMazeLoaded;
		void computing(std::string message);
		void mazeLoad(std::string image);
};