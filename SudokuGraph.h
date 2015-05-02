//
//  SudokuGraph.h
//  Final2
//
//  Created by Charley Luckhardt on 4/28/15.
//  Copyright (c) 2015 Charley Luckhardt. All rights reserved.
//

#ifndef __Final2__SudokuGraph__
#define __Final2__SudokuGraph__

#include <stdio.h>
#include <vector>
#include <iostream>
#include <queue>

struct vertex;

struct adjVertex{
    vertex *v;
    std::string relation;
    /*
    ==Relations==
    r = row
    c = column
    ss = section only
    rs = row and section
    cs = column and section
    */
};

//Possible value for an unsolved vertex
struct pValue {
    int value; //Set to an int 1-9 or zero if the vertex has an adjacent vertex that has been solved for that value
    
    //The number of adjacent vertices that cannot have this value
    int rowConflicts;
    int colConflicts;
    int secConflicts;
};

struct vertex{
    int row;
    int column;
    int value;
    bool checked;
    std::vector<pValue> possibleValues;
    std::vector<adjVertex> adj;
};

// Class stores graph class properties when auto solver guesses
// so they can be restored in case the guess is wrong
struct snapShot {
    int step; // Stores the current step
    std::vector<vertex> vertices; //Stores graphs vertices vector at a point
    std::vector<int> unsolved; //Stores graphs unsolved vector at a point
    int guessIdx; //Index of the vertex that the program guessed the value of
    int lastpValueIdx; //Index of pValue that was last guessed on
};


class Graph
{
public:
    Graph();
    ~Graph();
    void addEdges();
    void addVertex(int value, int row, int column);
    bool makeMove(int value, int row, int column);
    void undoMove();
    bool checkForWin();
    void solvePuzzle();
    bool checkIfValid();
    void printPuzzle(int step, int pIdx);
    void printAllPuzzles();
    void clearPuzzle();
    void resetPuzzle(int pIdx) ;
    std::vector<std::string> puzzles;
    int currentPuzzle;
    
protected:
private:
    void compareVertices();
    void comparePossibleValues(vertex *v);
    void getPossibleValues(vertex *v);
    void vertexValueSolved(vertex *v, int value);
    void dequeuePossibleSolutions(int &step);
    void enqueuePossibleSolutions(int step, int guessIdx);
    std::vector<vertex> vertices;
    std::vector<int> unsolved;
    std::vector<snapShot> snapShots;
    std::vector<int> moveList;
    
};

#endif /* defined(__Final2__SudokuGraph__) */
