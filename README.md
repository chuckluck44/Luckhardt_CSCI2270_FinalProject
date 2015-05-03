# Luckhardt_CSCI2270_FinalProject

Project Summary:

-This program is a text-based sudoku puzzle game/automatic solver. The program has several puzzles built in and gives the user the option to add in any more valid puzzle they find or create. The main function of the program is the automatic solver feature which will solve any valid puzzle that the user inputs and display the steps that it took. Each square in the puzzle is added as a vertex in a graph, with adjacent vertices of a square being all the squares in the same row, column, or section.  The edges are weighted based the relationship the squares have(row, column, or section). Each vertex/square also has a vector of possible values.  The program automatically solves the puzzle by looking for squares that only have one possible value, either because all the adjacent squares in a single relationship have been solved or they all have had a possible value eliminated.  If there is no square with one possible value the program stores the current state of the graph and makes a guess on a square with the fewest possible values, adding that guess to a queue as there may be more guesses required down the line. If the puzzle becomes impossible the program dequeues the most recent guess and guesses a new value for the square that was guessed on.  If all possible values for that square have been tried the previous guess must have been incorrect so the current guess is removed from the queue and the process is repeated with the guess before it. Functionality to allow the users to attempt to solve the puzzles themselves has been added as well.

How to Run: 

1.) Clone the file directory and extract all of the files into a separate folder. Make sure all the files, including 'Puzzles.txt', are in the same folder!

2.) In the terminal, navigate to the directory where the files are stored

3.) Compile the 'main.cpp' and 'SudokuGraph.ccp' files together by typing:

    g++ main.cpp SudokuGraph.cpp -std=c++11
    
4.) Run the executable with the current directory as an argument by typing:

    ./a.out $(pwd)
    
5.) This should compile and run the program. Then just follow the prompts

Dependencies: 

-Requires C++ 11, fstream, iostream, and standard use library.
  

System Requirements: 

-Run using terminal on any machine.
  

Group Members: 

-Charley Luckhardt
  
  
Open issues/bugs:
