// Zachary Schmalz
// June 7, 2017
// Main.cpp

#include <iostream>
#include "Maze.h"

using namespace std;

// Main method
int main()
{
	cout << "Schmalz Maze Solver v1.1.0 (x64)" << endl;
	cout << "Zachary Schmalz" << endl;
	cout << "June 17, 2017\n" << endl;

	int searchAlgorithm = 0;
	string fileName = "";
	string solutionName = "";

	cout << "Enter maze file name (*.bmp format only):" << endl;
	cin >> fileName;
	cout << "\nSelect search algorithm:" << endl;
	cout << "    (1). Depth First Seach" << endl;
	cin >> searchAlgorithm;
	cout << "\nEnter solution file name (*.bmp format only):" << endl;
	cin >> solutionName;

	cout << "\n\n\nSolving \"" << fileName << "\"" << endl << endl;

	Maze maze(fileName);
	maze.solve(searchAlgorithm, solutionName);
	
	system("pause");
}