// Zachary Schmalz
// December 15, 2017
// Main.cpp

#include <iostream>
#include "Maze.h"

using namespace std;

Maze CreateMaze();

// Main method
int main()
{
	// Quiet exceptions
	cimg::exception_mode(0);

	cout << "Schmalz Maze Solver v1.2.1 (x64)" << endl;
	cout << "Zachary Schmalz" << endl;
	cout << "December 15, 2017" << endl;

	// Create the maze object
	Maze maze = CreateMaze();

	int option = 0;
	while (true)
	{
		cout << "\nSelect option (" << maze.inputName << ")" <<endl;
		cout << "    (1). Depth First Seach" << endl;
		cout << "    (2). Breadth First Seach" << endl;
		cout << "    (3). Load New Image" << endl;
		cout << "    (4). Quit" << endl;

		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cin >> option;

		if (option == 1 || option == 2)
			maze.solve(option);

		else if (option == 3)
		{
			maze.clear();
			maze = CreateMaze();
		}

		else if (option == 4)
			return 0;
	}
}

// Creates the maze and handles I/O exceptions
Maze CreateMaze()
{
	Maze maze;

	string fileName = "";
	cout << "\nEnter maze file name (.bmp only):" << endl;
	cin >> fileName;

	// Check for File errors
	bool success = false;
	while (!success)
	{
		// File is not a BMP image
		if (fileName.find(".bmp") == string::npos)
		{
			cout << "ERROR: Invalid File Extension\n" << endl;
			cin.clear();
			cin.ignore(cin.rdbuf()->in_avail());
			cin >> fileName;
		}

		// Check that the file exists
		else
		{
			cout << endl;

			// The result of initializing the maze (returns false if file does not exist)
			success = maze.initialize(fileName);

			// File does not exist
			if (!success)
			{
				cout << "\nERROR: File Not Found\n\n";
				cin.clear();
				cin.ignore(cin.rdbuf()->in_avail());
				cin >> fileName;
			}
			else
				return maze;
		}
	}
}