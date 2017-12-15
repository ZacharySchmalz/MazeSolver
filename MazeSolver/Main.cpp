// Zachary Schmalz
// December 9, 2017
// Main.cpp

#include <iostream>
#include "Maze.h"

using namespace std;

// Main method
int main()
{
	cimg::exception_mode(0);

	cout << "Schmalz Maze Solver v1.2.0 (x64)" << endl;
	cout << "Zachary Schmalz" << endl;
	cout << "December 9, 2017\n" << endl;

	// Create the maze object
	Maze maze;

	string fileName = "";
	cout << "Enter maze file name (.bmp or .png):" << endl;
	cin >> fileName;

	// Check for File errors
	bool success = false;
	while (!success)
	{
		// File is not a BMP or PNG image
		if (fileName.find(".png") == string::npos && fileName.find(".bmp") == string::npos)
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
			if(!success)
			{
				cout << "\nERROR: File Not Found\n\n";
				cin.clear();
				cin.ignore(cin.rdbuf()->in_avail());
				cin >> fileName;
			}
		}
	}

	int option = 0;
	while (true)
	{
		cout << "\nSelect option:" << endl;
		cout << "    (1). Depth First Seach" << endl;
		cout << "    (2). Breadth First Seach" << endl;
		cout << "    (3). Quit" << endl;

		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cin >> option;

		if (option == 1 || option == 2)
			maze.solve(option);

		else if (option == 3)
			return 0;
	}
}