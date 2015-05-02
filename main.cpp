//
//  main.cpp
//  Final2
//
//  Created by Charley Luckhardt on 4/28/15.
//  Copyright (c) 2015 Charley Luckhardt. All rights reserved.
//

#include <iostream>
#include "SudokuGraph.h"
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <cstring>

using namespace std;

//Helper functions
void displayMenu();
//Adds puzzles from Puzzle.txt into graph
void readFileIntoGraph(Graph & sGraph, string fileName);
//Writes user created puzzle to Puzzle.txt
void writePuzzleIntoFile(string puzzle, string fileName);
//Prompts user to solve puzzle
void startSolving (Graph &sGraph);
//Prompts user to build puzzle
void buildPuzzle (Graph & sGraph, string filePath);

int main(int argc, const char * argv[]) {
    
    Graph sGraph;
    
    string filePath = argv[1];
    filePath += "/Puzzles.txt";
    cout << filePath << endl;
    
    bool quit = false;
    int input;
    int input2;
    
    readFileIntoGraph(sGraph, filePath);
    
    //Initial puzzle selection
    cout << "WELCOME TO SUDOKU SOLVER!" << endl;
    cout << "=========================" << endl;
    cout << endl;
    cout << "Would you like to..." << endl;
    cout << "1. Select a pre-made puzzle" << endl;
    cout << "2. Add in your own puzzle" << endl;
    
    cin >> input;
    
    cin.clear();
    cin.ignore(1000,'\n');
    
    switch (input) {
        case 1:
            
            //Prints puzzle library
            sGraph.printAllPuzzles();
            cout << "Enter the number of the puzzle you would like: " << endl;
            cin >> input2;
            
            cin.clear();
            cin.ignore(1000,'\n');
            
            //Set to selected puzzle
            sGraph.resetPuzzle(input2 - 1);
            sGraph.printPuzzle(0, sGraph.currentPuzzle);
            break;
            
        case 2:
            buildPuzzle(sGraph, filePath);
            break;
            
        default:
            sGraph.resetPuzzle(0);
            sGraph.printPuzzle(0, sGraph.currentPuzzle);
            break;
    }
    
    while (quit != true) {
        
        displayMenu();
        cin >> input;
        
        cin.clear();
        cin.ignore(1000,'\n');
        
        switch (input) {
                
            case 1:
                sGraph.solvePuzzle();
                break;
        
            case 2:
                startSolving(sGraph);
                break;
                
            case 3:
                sGraph.undoMove();
                break;
                
            case 4:
                sGraph.resetPuzzle(sGraph.currentPuzzle);
                sGraph.printAllPuzzles();
                
                cout << "Enter the number of the puzzle you would like: " << endl;
                cin >> input2;
                
                cin.clear();
                cin.ignore(1000,'\n');
                
                sGraph.resetPuzzle(input2 - 1);
                sGraph.printPuzzle(0, sGraph.currentPuzzle);
                break;
                
            case 5:
                buildPuzzle(sGraph, filePath);
                break;
            
            case 6:
                sGraph.resetPuzzle(sGraph.currentPuzzle);
                sGraph.printPuzzle(0, sGraph.currentPuzzle);
                break;
                
            case 7:
                sGraph.printPuzzle(0, sGraph.currentPuzzle);
                break;
            
            case 8:
                cout << "Goodbye!" << endl;
                quit = true;
                break;

                
            default:
                cout << "Please enter valid menu option" << endl;
                cin.clear();
                cin.ignore(1000,'\n');
                break;
        }
        
    }
    
    return 0;
}

void displayMenu() {
    
    cout << "======Main Menu=====" << endl;
    cout << "1. USE AUTOSOLVER!!" << endl;
    cout << "2. Solve it yourself" << endl;
    cout << "3. Undo last move" << endl;
    cout << "4. Choose new puzzle" << endl;
    cout << "5. Enter own puzzle" << endl;
    cout << "6. Reset puzzle" << endl;
    cout << "7. Display current puzzle" << endl;
    cout << "8. Quit" << endl;
    
}

//Prompts user for their move and checks if its valid
void startSolving (Graph &sGraph) {
    
    string inValue, inRow, inCol;
    int loop = 0;
    
    while (inValue != "s" && inRow != "s" && inCol != "s") {
        if (loop > 0) { //User has pressed 'u' for undo
            loop--;
            sGraph.undoMove();
            sGraph.printPuzzle(loop, sGraph.currentPuzzle);
        }
        //Get values for value, row, and column
        while (true) {
            cout << "Enter 'u' to undo your last move or 's' to stop solving" << endl;
            cout << "Enter value:  " << endl;
            getline(cin, inValue);
            if (inValue == "s") break;
            else if (inValue == "u") break;
            
            cout << "Enter row:  " << endl;
            getline(cin,inRow);
            if (inRow == "s") break;
            else if (inRow == "u") break;
            
            cout << "Enter column:  " << endl;
            getline(cin,inCol);
            if (inRow == "s") break;
            else if (inCol == "u") break;
            
            //Makes move if valid if not returns false
            bool isValid = sGraph.makeMove(atoi(inValue.c_str()), atoi(inRow.c_str()), atoi(inCol.c_str()));
            if(!isValid) {
                cout << "Not a valid move! Please try again" << endl;
            }else {
                loop++;
                sGraph.printPuzzle(loop, sGraph.currentPuzzle);
                //Check if all values are solved
                if (sGraph.checkForWin()) {
                    cout << "You solved the puzzle!!"<< endl;
                    return;
                }
            }
        }
    }
}

//Reads puzzles from txt file and stores them in graph class instance
void readFileIntoGraph(Graph & sGraph, string fileName)
{
    ifstream in_stream;
    cout << fileName << endl;
    in_stream.open(fileName);
    if (!in_stream)
    {
        cout << "Could not open file\n";
        return;
    }
    
    string line;
    while (!in_stream.eof())
    {
        getline(in_stream, line);
        if (line != "") {
            sGraph.puzzles.push_back(line);
        }
    }
    in_stream.close();
    
}

//Writes puzzle to puzzle.txt
void writePuzzleIntoFile(string puzzle, string fileName) {
    ofstream outfile;
    outfile.open( fileName, ios::out | ios::app );
    if (outfile.is_open())
    {
        outfile << puzzle << endl;
        outfile.close();
    }
    else cout << "Unable to open file";
}

//Prompts users for rows to build puzzle and if its valid stores it
void buildPuzzle (Graph & sGraph, string filePath) {
    
    string p = "";
    string inRow;
    int nonZeros = 0;
    
    sGraph.clearPuzzle();
    
    cout << "Please make sure you are entering a valid Sudoku puzzle.";
    cout << "The solver is programmed to make guesses and correct those guesses if they are wrong so as long as ";
    cout << "the puzzle is valid it should come to a conclusion." << endl;
    cout << endl;
    cout << "'0' represents a space when entering in the rows" << endl;
    cout << "Enter 'c' at any time to cancel" << endl;
    
    //Iterate through each row
    for (int row = 0; row < 9; row++) {
        
        if (inRow == "c") {
            break;
        }
        
        bool validEntry = false;
        while (validEntry == false) {
            
            cout << "Enter row " << row + 1 << ":" << endl;
            getline(cin,inRow);
            if (inRow == "c") {
                sGraph.clearPuzzle();
                break;
            }
            if (inRow.length() != 9) {
                cout << "Row must be exactly nine digits long. Please retry" << endl;
            }
            else {
                validEntry = true;
                //Check each value in row
                for (int col = 0; col < 9; col++)
                {
                    int value = inRow[col] - '0';
                    sGraph.addVertex(value, row, col);
                    if (value > 0 && value < 10) {
                        nonZeros++;
                    }else if (value != 0) {
                        nonZeros -= col;
                        validEntry = false;
                        cout << "Row cannot contain negative values or values above 9. Please retry" << endl;
                        break;
                    }
                }
                if (validEntry == true) p += inRow;; //Add row to puzzle
            }
        }
    }
    //Add edges
    sGraph.addEdges();
    
    if (nonZeros < 10) {
        cout << "Invalid puzzle need atleast 10 nonzero integers." << endl;
        sGraph.resetPuzzle(0);
    }else if (!sGraph.checkIfValid()) {
        cout << endl;
        cout << "Invalid puzzle equal digits are in the same row, column, or section." << endl;
        cout << endl;
        sGraph.resetPuzzle(0);
    }else if (inRow != "c") {
        writePuzzleIntoFile(p, filePath);
        sGraph.puzzles.push_back(p);
        sGraph.currentPuzzle = sGraph.puzzles.size() - 1;//Set the current puzzle as this puzzle
        sGraph.printPuzzle(0, sGraph.currentPuzzle);
        cout << "Puzzle added to selection" << endl;
        cout << endl;
        
    }
}
