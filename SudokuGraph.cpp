//
//  SudokuGraph.cpp
//  Final2
//
//  Created by Charley Luckhardt on 4/28/15.
//  Copyright (c) 2015 Charley Luckhardt. All rights reserved.
//

#include "SudokuGraph.h"
#include <queue>
#include <tgmath.h>

using namespace std;

Graph::Graph()
{
    //string p1 = "006007300018009050500000064920080000000763000000090075630000008090300520002400600";
    //string p2 = "090000008200000190400201000030006087000709000710800030000503006086000001500000070";
    //string p3 = "000096000000000019028500030540600001007000800800002075050008740630000000000940000";
    //puzzles.push_back(p1);
    //puzzles.push_back(p2);
    //puzzles.push_back(p3);
    currentPuzzle = -1;
}

Graph::~Graph()
{
}

int getIndexOfVertex(int row, int column) {
    return (9 * row + column);
}

void Graph::addEdges()
{
    for (int i = 0; i < vertices.size(); i++) {
        
        //Add edges for row and column
        for (int j = 0; j < 9; j++) {
            //Row
            if (j != vertices[i].column) {
                adjVertex av;
                av.v = &vertices[getIndexOfVertex(vertices[i].row, j)];
                
                //Get relation
                if (vertices[i].column / 3 == j / 3) { //Check if vertices are in the same section
                    av.relation = "rs";//row-section relation
                }else {
                    av.relation = "r";//just row relation
                }
                
                vertices[i].adj.push_back(av);
            }
            //Column
            if (j != vertices[i].row) {
                adjVertex av;
                av.v = &vertices[getIndexOfVertex(j, vertices[i].column)];
                
                //Get relation
                if (vertices[i].row / 3 == j / 3) {  //Check if vertices are in the same section
                    av.relation = "cs";  //column-section relation
                }else {
                    av.relation = "c";  //just column relation
                }
                
                vertices[i].adj.push_back(av);
            }
        }
        
        //Add edges for sections
        int tlRow = vertices[i].row / 3 * 3;
        int tlCol = vertices[i].column / 3 * 3;
        
        for (int r = tlRow; r < (tlRow + 3); r++) {
            for (int c = tlCol; c < (tlCol + 3); c++)
            {
                if (r != vertices[i].row && c != vertices[i].column)
                {
                    adjVertex avSec;
                    avSec.relation = "ss";
                    avSec.v = &vertices[getIndexOfVertex(r, c)];

                    vertices[i].adj.push_back(avSec);
                }
            }
        }
    }
}

void Graph::addVertex(int value, int row, int column)
{
    vertex v;
    v.value = value;
    v.row = row;
    v.column = column;
    v.checked = false;

    vertices.push_back(v);
    
    if (value == 0) {
        unsolved.push_back(getIndexOfVertex(row, column));
    }
    for (int j = 1; j <= 9; j++) {
        pValue pv;
        
        //If the vertex already has a value set all other possible values to zero
        if (value > 0 && j != value) {
            pv.value = 0;
        }
        else pv.value = j;
        
        pv.rowConflicts = 0;
        pv.colConflicts = 0;
        pv.secConflicts = 0;
        
        vertices.back().possibleValues.push_back(pv);
    }
}

bool Graph::makeMove(int value, int row, int column) {
    
    int idx = getIndexOfVertex(row -  1, column - 1);
    string p = puzzles[currentPuzzle];
    
    if (p[idx] - '0' > 0) {
        cout << "That square is part of the puzzle. "; 
        return false;
    }
    vertices[idx].value = value;
    
    bool isValid = checkIfValid();
    
    if (isValid) {
        vertexValueSolved(&vertices[idx], value);
        moveList.push_back(idx);
    }else {
        vertices[idx].value = 0;
    }
    
    return isValid;
}

void Graph::undoMove () {
    
    if (moveList.empty()) {
        cout << "No moves to undo!" << endl;
        return;
    }
    
    int idx = moveList.back();
    vertices[idx].value = 0;
    
}

bool Graph::checkForWin() {
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i].value == 0) {
            return false;
        }
    }
    return true;
}

void Graph::solvePuzzle()
{
    //Get possible values for vertices
    for (int i = 0; i < unsolved.size(); i++) {
        getPossibleValues(&vertices[unsolved[i]]);
    }
    
    int step = 0;
    int j;
    int zeroCount;
    vector<int> nzVals;
    
    int failCount = 0;
    int guess = 0;
    int inguess = 0;
    
    int fVal = 0;
    
    
    while (unsolved.empty() != true) {
        int sizeCheck = unsolved.size();
        
        queue<int> guessq;
        
        for(int i = 0; i < unsolved.size(); i++) {
            
            compareVertices();
            
            zeroCount = 0;
            j = 0;
            
            while (j < 9) {
                
                pValue *pVal = &vertices[unsolved[i]].possibleValues[j];
                
                if (vertices[unsolved[i]].possibleValues[j].value == 0) {
                    zeroCount ++;
                }else {
                    
                    fVal = j+1;
                    
                    //checks if all squares in vertex row, column or section are restricted from a possible value
                    if (pVal->secConflicts == 8 || pVal->colConflicts == 8 || pVal->colConflicts == 8) {
                        
                        vertexValueSolved(&vertices[unsolved[i]], j + 1);
                        step++;
                        printPuzzle(step, currentPuzzle);
                        cout << j + 1 << " placed at row: " << vertices[unsolved[i]].row + 1 ;
                        cout << " column: " << vertices[unsolved[i]].column + 1 << endl;
                        
                        //Erase and decrement 'i'
                        unsolved.erase(unsolved.begin() + i);
                        i--;
                        
                        break;
                    }
                }
                
                //If there are no possible values for a vertex incorrect guess was made
                if (zeroCount == 9) {
                    dequeuePossibleSolutions(step);
                    inguess++;
                    if (step == 0) {
                        cout << "Puzzle has no solution... " << unsolved.size() << " missing values."<< endl;
                        cout << endl;
                        return;
                    }
                    break;
                }
                
                //Adjust number of possible values required to solve a vertex if no vertices were solved previous loop (GUESS)
                else if (j == 8 && zeroCount == 8 - failCount) {
                    
                    if (failCount > 0) {
                        //Enqueue this vertex as a guess
                        enqueuePossibleSolutions(step, unsolved[i]);
                        guess++;
                        
                        //Reset fail count
                        failCount = 0;
                    }
                    
                    else {
                        vertexValueSolved(&vertices[unsolved[i]], fVal);
                        step++;
                        printPuzzle(step, currentPuzzle);
                        cout << j + 1 << " placed at row: " << vertices[unsolved[i]].row + 1;
                        cout << " column: " << vertices[unsolved[i]].column + 1 << endl;
                        
                        //Erase and decrement 'i'
                        unsolved.erase(unsolved.begin() + i);
                        i--;
                    }
                }
                
                j++;//Increment j
            }
        }
        
        //Check if any vertices were solved and if
        if (unsolved.size() == sizeCheck) {
            failCount ++;
            
            if (failCount == 9) {
                cout << "ERROR: " << failCount << endl;
                cout << "Could not solve puzzle... " << unsolved.size() << " missing values."<< endl;
                cout << endl;
                return;
            }
        }
    }
    
    printPuzzle(0, currentPuzzle);
    cout << "Puzzle solved!!! - " << step << " steps" << endl;
    cout << endl;
}


//Restores graph to its previous state and dequeues the next possible guess at that point
void Graph::dequeuePossibleSolutions(int &step) {
    cout << "Guess failed... going back to step "<< snapShots.back().step << " and making a different one" << endl;

    snapShot *ss;
    vertex *v;
    while (snapShots.empty() == false) {
        
        //Get the last guess made and restore graph
        ss = &snapShots.back();
        vertices = ss->vertices;
        unsolved = ss->unsolved;
        
        v = &vertices[ss->guessIdx];
        
        //Check for any other possible values
        for (int k = ss->lastpValueIdx + 1; k < 9; k++) {
            if (v->possibleValues[k].value > 0) {
                
                ss->lastpValueIdx = k;
                vertexValueSolved(v, k+ 1);
                printPuzzle(ss->step + 1, currentPuzzle);
                cout << k + 1 << " placed at row: " << v->row + 1;
                cout << " column: " << v->column + 1 << endl;
                
                return;
                
            }
        }
        //ss->guessq.pop();
        //If no other possible values move to previous guess
        ss->lastpValueIdx = - 1;
        snapShots.pop_back();
        
    }
    step = 0;
    return;
}

//Stores graphs current state and enques all possible guesses at that point
void Graph::enqueuePossibleSolutions(int step, int guessIdx) {
    cout << "Making a guess!!" << endl;
    snapShot ss;
    ss.step = step;
    ss.vertices = vertices;
    ss.unsolved = unsolved;
    ss.guessIdx = guessIdx;
    //ss.guessq = guessq;
    snapShots.push_back(ss);
    
    vertex *v = &vertices[guessIdx];
    int k = 0;
    while (k < 9) {
        if (v->possibleValues[k].value > 0) {
            snapShots.back().lastpValueIdx = k;
            vertexValueSolved(v, k + 1);
            return;
        }
        k++;
    }
}

void Graph::compareVertices() {
    
    //Set all vertices to unchecked and reset adjacent conflicts to zero
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].checked = false;
        for (int j = 0; j < 9; j++) {
            pValue *pvals = &vertices[i].possibleValues[j];
            pvals->rowConflicts = 0;
            pvals->colConflicts = 0;
            pvals->secConflicts = 0;
        }
    }
    
    for (int i = 0; i < unsolved.size() ; i++) {
        
        //Find value conflicts with adjacent vertices
        comparePossibleValues(&vertices[unsolved[i]]);
        
        //Set vertex to solved
        vertices[unsolved[i]].checked = true;
    }
}

void Graph::comparePossibleValues(vertex *v) {
    
    for (int i = 0; i < v->adj.size(); i++){
        if (v->adj[i].v->checked == false) {
            for (int j = 0; j < 9; j++) {
                
                pValue *vValue = &v->possibleValues[j];
                pValue *adjValue = &v->adj[i].v->possibleValues[j];
                
                pValue *pvals = NULL;
                string rString = v->adj[i].relation;
                
                //Check if which vertex should be updated
                if (vValue->value > adjValue->value) {
                    pvals = &v->possibleValues[j];
                }
                else if (vValue->value < adjValue->value) {
                    pvals = &v->adj[i].v->possibleValues[j];
                }
                else {
                    rString = "xx"; //values are equal and nothing should be updated, relation = xx
                }
                
                //Update conflicts in possible values for vertices' relation
                if ( strncmp(&rString[0], "r", 1) == 0 ) {
                    pvals->rowConflicts++;
                }else if ( strncmp( &rString[0], "c", 1) == 0 ){
                    pvals->colConflicts++;
                }if ( strncmp(&rString[1], "s", 1) == 0 ) {
                    pvals->secConflicts++;
                }
            }
        }
    }
}

void Graph::getPossibleValues(vertex *v) {
    
    //Get possible values by iterating through solved adjacent vertices
    for (int j = 0; j < v->adj.size(); j++) {
        if (v->adj[j].v->value > 0) {
            //Set vertex value in the unsolved vertex possible values to zero
            v->possibleValues[v->adj[j].v->value - 1].value = 0;
        }
    }
}

void Graph::vertexValueSolved(vertex *v, int value) {
    
    //Set value of vertex
    v->value = value;
    
    //Set all other possible values for the vertex to zero
    for (int i = 1; i <= 9; i++) {
        if (i != value) {
            v->possibleValues[i - 1].value = 0;
        }else v->possibleValues[i - 1].value = value;
    }
    //On all adjacent vertices, set the possible value matching the solved vertex to zero
    for (int i = 0; i < v->adj.size(); i++) {
        if (v->adj[i].v->value == 0) {
            v->adj[i].v->possibleValues[value - 1].value = 0;
        }
    }
}

bool Graph::checkIfValid() {
    
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].checked = false;
    }
    
    for (int i = 0; i < vertices.size(); i++ ) {
        if (vertices[i].value > 0) {
            for (int j = 0; j < vertices[i].adj.size(); j++) {
                if (vertices[i].adj[j].v->value == vertices[i].value) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

void Graph::printPuzzle(int step, int pIdx) {
    
    //Check if puzzle is being solved
    cout << endl;
    if (step == 0) {
        cout << "====================" << endl;
    }else {
        cout << "====== Step " << step << " ======" << endl;
    }
    
    //Get puzzle string
    string p;
    if (pIdx == currentPuzzle) {
        p ="";
        for (int i = 0; i < vertices.size(); i++) {
            p += to_string(vertices[i].value);
        }
    }else {
        p = puzzles[pIdx];
    }
    
    //Display string in table
    for (int i = 0; i < p.length(); i++) {
        string pChar = to_string(p[i] - '0');
        if (pChar == "0") {
            pChar = " ";
        }
        if (i % 9 == 8) {
            cout << "|" << pChar << "|" << endl;
        }else {
            cout << "|"<< pChar;
        }
    }
    
    
    cout << "====================" << endl;
    cout << endl;
}

void Graph::printAllPuzzles() {
    string difficulty;
    for (int i = 0; i < puzzles.size(); i++) {
        if (i == 0) difficulty = "EASY";
        else if (i == 1) difficulty = "MEDIUM";
        else if (i == 2) difficulty = "HARD";
        else if (i == 3) difficulty = "VERY HARD";
        else difficulty = "USER ADDED";
        cout << "- Puzzle " << i + 1 << "(" << difficulty <<")";
        printPuzzle(0, i);
    }
}

void Graph::resetPuzzle(int pIdx) {
    
    //Clear curent vertices
    clearPuzzle();
    
    //Set current puzzle idx
    currentPuzzle = pIdx;
    
    //Build Puzzle graph
    string p = puzzles[pIdx];
    for (int i = 0; i < p.length(); i++) {
        int value = p[i] - '0';
        
        addVertex(value, i / 9, i % 9);
    }
    
    addEdges();
}

void Graph::clearPuzzle() {
    vertices.clear();
    snapShots.clear();
}


