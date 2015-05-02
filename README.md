# Luckhardt_CSCI2270_FinalProject

Project Summary:

-This program is a text-based sudoku puzzle game/automatic solver. The program has several puzzles built in and gives the user the option to add in any more valid puzzle they find or create. The main function of the program is the automatic solver feature which will solve any valid puzzle that the user inputs and display the steps that it took. However, functionality to allow the users to attempt to solve the puzzles themselves has been added as well.

How to Run: 

1.) Clone the file directory and extract all of the files into a separate folder

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

-Program does not always write user added puzzles to the Puzzle.txt file correctly
