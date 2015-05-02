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

/*
Function prototype:
void Graph::solvePuzzle()

Function description:
This is the main function that auto solves the puzzle. It use several helper functions to do this including-
void Graph::dequeuePossibleSolutions(int &step)
void Graph::enqueuePossibleSolutions(int step, int guessIdx) 
void Graph::compareVertices()
void Graph::comparePossibleValues(vertex *v)
void Graph::getPossibleValues(vertex *v)
void Graph::vertexValueSolved(vertex *v, int value)
See below for method details

Example:
HashTable ht;
ht.deleteMovie("The Usual Suspects")

Precondition: Puzzle is reset to stock state with no user answers.
Post condition: All puzzle vertices have been solved with a value
*/
void Graph::solvePuzzle()
{
    //Get possible values for vertices
    for (int i = 0; i < unsolved.size(); i++) {
        getPossibleValues(&vertices[unsolved[i]]);
    }
    
    int step = 0; //Counts what step the solvers on
    int j;//count for possible values (1-9)
    int zeroCount;//Counts the number of possible values for a vertex that have been eliminated
    
    int failCount = 0;//counts the number have times the times has failed to solve/guess a vertex
    
    int fVal = 0;//Stores non zero possible values in the case 8 other possible values have been eliminated
    
    
    while (unsolved.empty() != true) {
        int sizeCheck = unsolved.size();
        
        for(int i = 0; i < unsolved.size(); i++) {
            
            compareVertices();//See function description below
            
            zeroCount = 0;
            j = 0;
            
            //Iterate through each possible value
            while (j < 9) {
                
                pValue *pVal = &vertices[unsolved[i]].possibleValues[j];
                
                if (vertices[unsolved[i]].possibleValues[j].value == 0) {//If value not possible
                    zeroCount ++;
                }else {
                    //Store the possible value
                    fVal = j+1;
                    
                    //checks if all adjacent squares in the vertex's row, column or section are restricted from this possible value
                    if (pVal->secConflicts == 8 || pVal->colConflicts == 8 || pVal->colConflicts == 8) {
                        
                        //Solve vertex and print puzzle
                        vertexValueSolved(&vertices[unsolved[i]], j + 1);
                        step++;
                        printPuzzle(step, currentPuzzle);
                        cout << j + 1 << " placed at row: " << vertices[unsolved[i]].row + 1 ;
                        cout << " column: " << vertices[unsolved[i]].column + 1 << endl;
                        
                        //Erase and decrement 'i' so we remain at the correct position in the unsolved vector
                        unsolved.erase(unsolved.begin() + i);
                        i--;
                        
                        break;
                    }
                }
                
                //If there are no possible values for a vertex puzzle is impossible and incorrect guess was made
                if (zeroCount == 9) {
                    dequeuePossibleSolutions(step);//See description below
                    
                    //if snap shot queue is empty the puzzle is impossible
                    if (step == 0) {
                        cout << "Puzzle has no solution... " << unsolved.size() << " missing values."<< endl;
                        cout << endl;
                        return;
                    }
                    break;
                }
                
                /*Checks the number of possible values that exist for the vertex.
                The larger the number of times the program has solved/guessed a vertex value the 
                more possible values a vertex can have to be guessed on.
                ex:
                - failCount = 1 there is no vertex with one possible solution vertex can now have two possible solutions
                            (8 - failCount) = 7 or 7 eliminated solutions out of 9
                - failCount = 2 there is no vertex with one possible solution vertex can now have tree possible solutions 
                            (8 - failCount) = 6 or 6 eliminated solutions out of 9
                */
                else if (j == 8 && zeroCount == 8 - failCount) {
                    
                    //If program has failed already vertex contains atleast two possible solutions. Guess must be made
                    if (failCount > 0) {
                        //Enqueue this vertex as a guess
                        enqueuePossibleSolutions(step, unsolved[i]);
                        //Vertex has been guessed reset fail count
                        failCount = 0;
                    }
                    //Vertex only has one possible solution so solve
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
        
        //Check if no vertices were solved
        if (unsolved.size() == sizeCheck) {
            failCount ++;
            
            if (failCount == 9) { //Shouldnt happen but to keep the program from running forever
                cout << "ERROR: " << failCount << endl;
                cout << "Could not solve puzzle... " << unsolved.size() << " missing values."<< endl;
                cout << endl;
                return;
            }
        }
    }
    
    //unsolved vector is empty puzzle solved
    printPuzzle(0, currentPuzzle);
    cout << "Puzzle solved!!! - " << step << " steps" << endl;
    cout << endl;
}


/*
Function prototype:
void Graph::dequeuePossibleSolutions(int &step)

Function description:
Dequeues most recent snapshot and restores the vertices vector and unsolved vector to their state at the point of the guess.
Gets the vertex that was guessed on (using guessIdx) and resumes iteration over at the last guessed value (using lastpValueIdx) and will check the next possible value.
If no other possible value exists for that vertex, snapshot is removed and the process is repeated for the previous snapshot.
The variable step in only in the scope of the function so it is entered as a constant argument.

Example:
SudokuGraph g;
g.dequeuePossibleSolutions(1)

Precondition: snapshot vector is not empty. Guess has been made on some vertex.
Post condition: Graph is restored to a previous state. And previously guessed vertex has been solved with a new value
*/
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

/*
Function prototype:
void Graph::enqueuePossibleSolutions(int step, int guessIdx)

Function description:
Stores current state of graph(vertices vector and unsolved vector), current step and the idx of the vertex that its making a guess on(guessIdx) in a snapShot struct.
Adds the snapShot struct to the Graph class snapshots vector.
It then iterates over the possible values and stores the index of first one available in "lastpValueIdx"
Then solves the vertex for that value using void Graph::vertexValueSolved(vertex *v, int value) and returns.
The variable step in only in the scope of the function so it is entered as a constant argument.

Example:
SudokuGraph g;
g.enqueuePossibleSolutions(10, 25)

Precondition: Vertex with correct number of possible values has been found and its index is passed with the step.
Post condition: A vertex is now solved and its possible values updated
*/
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

/*
Function prototype:
void Graph::compareVertices()

Function description:
Compares the possible values of all vertices and their adjacent vertices using the helper function void Graph::comparePossibleValues(vertex *v) *details below
It does a a sort of bftraversal but instead of queueing vertices it pulls them from the graphs unsolved vector since we do not need to update solved vertices.
For an unsolved vertex it checks all adj vertices possible values
    -If an adj vertex is solved it sets the value of that possible value for the unsolved vertex to zero
    -For adj vertices that are unsolved or solved it compares the possible values to the vertex's possible values and updates the row, columns, and section conflicts accordingly
        -if the vertexs pvalue is possible and the adj vertex is not, the vertex pvalue contflict count is incremented
        -if the vertexs pvalue is not possible and the adj vertex is, the adj vertex pvalue contflict count is incremented
        -if both are impossible/possible nothing is incremented
Vertex is set to checked so that when adj unsolved verices wont compare twice

Precondition: Graph::getPossibleValues(vertex *v) has been run so that all vertices possible values have been set
Post condition: All vertices have up to date possible values and conflict counts
*/
void Graph::compareVertices() {
    
    //Set all vertices to unchecked and their adjacent conflicts to zero
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].checked = false;
        for (int j = 0; j < 9; j++) {
            pValue *pvals = &vertices[i].possibleValues[j];
            pvals->rowConflicts = 0;
            pvals->colConflicts = 0;
            pvals->secConflicts = 0;
        }
    }
    //iterate through unsolved vertices
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


