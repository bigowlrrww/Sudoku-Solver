/***********************************************************************
* Program:
*    Project Final, Sudoku
*    Brother Ellsworth, CS124
* Author:
*    Dean Hovinghoff
* Summary:
*    This program uses a sudoku board for gameplay. Got tired of switching
*  color definitions so there is a define structure for windows.
*
*    Estimated:  0.5 hrs
*    Actual:     0.5 hrs
*      The hardest part was solving all of the little convience items.
************************************************************************/
//Windows OS Compatiblity
#define WINDOWS
//Color for board display
#define COLORS 
#ifdef COLORS
   #ifdef WINDOWS
      #define colorRed(x) SetConsoleTextAttribute(x, 12)
      #define colorGreen(x) SetConsoleTextAttribute(x, 10)
      #define colorWhite(x) SetConsoleTextAttribute(x, 15)
      #define clearScreen system("cls")
      #include <Windows.h>
   #else
      #define colorRed(x) cout << "\033[" << "31" << "m"
      #define colorGreen(x) cout << "\033[" << "22;32" << "m"
      #define colorWhite(x) cout << "\033[" << "0" << "m"
      #define clearScreen system("clear")
   #endif
#else
   #define colorRed(x)
   #define colorGreen(x)
   #define colorWhite(x)
#endif
//SetConsoleTextAttribute(hConsole, 10);

#include <iostream>
#include <fstream>
#include <ctype.h>

//globals
#define ROW 9
#define COL 9

//header
int readFile(char fileName[], int board[ROW][COL]);
int writeFile(char fileName[], int board[ROW][COL], bool checkSum[ROW][COL]);
void clearBoard(int board[ROW][COL]);
void clearSolutions(int array[], int length);
void dispCommands();
void dispBoard(int board[ROW][COL]);
bool interact(char fileName[256], int board[ROW][COL], 
              bool checkSum[ROW][COL]);
void editSquare(int board[ROW][COL], bool checkSum[ROW][COL]);
int computeValues(int board[ROW][COL], int posRow, int posCol, 
                  int solutions[10]);
int findBox(int posRow, int posCol, int * pPosBoxRow, int * pPosBoxCol);
void generateCheckSum(int board[ROW][COL], bool checkSum[ROW][COL]);
//NEW
void dispBoardColor(int board[ROW][COL], bool checkSum[ROW][COL]);
bool validateNumber(int board[ROW][COL], int posRow, int posCol, int number);
void showSolution(int board[ROW][COL]);
bool validBoard(int board[ROW][COL]);
char intColToChar(int j);
//Solver
int solveBoard(int board[ROW][COL]);
void testAllSquares(int board[ROW][COL], int posibleSolution[ROW][COL][9]);
void copyBoard(int boardToCopy[ROW][COL], int boardCopiedTo[ROW][COL]);
void generateCheckSum(int board[ROW][COL], bool checkSum[ROW][COL]);
bool solveRecusive(int board[ROW][COL], bool checkSum[ROW][COL],
                   int posSolutions[ROW][COL][9], int *row, int *col,
                   int *index, int speed);
bool solve(int board[ROW][COL], int posibleSolution[ROW][COL][9]);
void startSolve(int board[ROW][COL], bool checkSum[ROW][COL]);

using namespace std;

/**********************************************************************
* main() runs the backbone of the program.
***********************************************************************/
int main()
{
   char fileName[256] = "";
   int board[ROW][COL];
   int test;
   bool saved = false;
   bool checkSum[ROW][COL];

   cout << "Where is your board located? ";
   cin >> fileName;

   clearBoard(board);
   test = readFile(fileName, board);
   if (test == -1)
   {
      cout << "Invalid file!";
      return 1;
   }
   //generage a checksum for board
   generateCheckSum(board, checkSum);
   if (!validBoard(board))
   {
      return 2;
   }
   dispCommands();
   
   dispBoardColor(board, checkSum);
   //dispBoard(board);
   cout << "\n";

   
   while (!saved)
   {
      saved = interact(fileName, board, checkSum);
   }
   return 0;
}

/**********************************************************************
* readFile(fileName[], board[][]) writes the file and returns -1 if it
* could not open the file. returns -2 if it finished reading the tick Tack
* Toe before end of file.
***********************************************************************/
int readFile(char fileName[], int board[ROW][COL])
{
   char word;

   ifstream fin(fileName);
   if (fin.fail())
      return -1;

   for (int i = 0; !fin.eof(); i++)
   {
      for (int j = 0; j < COL; j++)
      {
         fin >> board[i][j]; //i = row // j = col
      }
      if (i >= ROW && !fin.eof())
      {
         fin.close();
         return -2;
      }
   }
   fin.close();
   return 0;
}

/**********************************************************************
* writeFile(fileName[], board[][]) writes the file and returns -1 if it
* could not write the file.
***********************************************************************/
int writeFile(char fileName[], int board[ROW][COL], bool checkSum[ROW][COL])
{
   ofstream fout(fileName);
   if (fout.fail())
      return -1;

   for (int i = 0; i < ROW; i++)
   {
      for (int j = 0; j < COL; j++)
      {
         fout << (j == 0 ? "" : " ");
         if (checkSum[i][j])
         {
            fout << board[i][j];
         }
         else
         {
            if (board[i][j] != 0)
            {
               fout << board[i][j] + 10;
            }
            else
            {
               fout << board[i][j];
            }
         }
      }
      fout << "\n";
   }
   fout.close();
   cout << "Board written successfully\n";
   return 0;
}

/**********************************************************************
* clearBoard(board[][]) sets all of the elements of board to 0
***********************************************************************/
void clearBoard(int board[ROW][COL])
{
   for (int i = 0; i < ROW; i++)
   {
      for (int j = 0; j < COL; j++)
      {
         board[i][j] = 0;
      }
   }
}

/**********************************************************************
* clearSolutions(array[], lenght) sets all of the elements of array to 0
***********************************************************************/
void clearSolutions(int array[], int length)
{
   for (int i = 0; i < length; i++)
   {
      array[i] = -1;
   }
}

/**********************************************************************
* dispCommands() displays the commands for prompt to console
***********************************************************************/
void dispCommands()
{
   cout << "Options:\n";
   cout << "   ?  Show these instructions\n";
   cout << "   D  Display the board\n";
   cout << "   E  Edit one square\n";
   cout << "   S  Show the possible values for a square\n";
   cout << "   Q  Save and Quit\n";
   cout << "   F  Finish The Puzzle\n";
   cout << "\n";
}

/**********************************************************************
* dispBoard(board[][]) displays board to console
***********************************************************************/
void dispBoard(int board[ROW][COL])
{
   cout << "   A B C D E F G H I\n";
   for (int i = 0; i < ROW; i++)
   {
      cout << i + 1 << "  ";
      for (int j = 0; j < COL; j++)
      {
         if (board[i][j] == 0)
         {
            cout << " ";
         }
         else
         {
            cout << board[i][j];
         }

         if ((j + 1) % 3 == 0 && j != 8)
         {
            cout << "|";
         }
         else if (j != 8)
         {
            cout << " ";
         }
      }
      cout << "\n";
      if ((i + 1) % 3 == 0 && i != 8)
         cout << "   -----+-----+-----\n";
   }
}

/**********************************************************************
* interact(fileName, board[][], checkSum[][]) prompts the user for 
*   interaction in console format.
***********************************************************************/
bool interact(char fileName[256], int board[ROW][COL], 
               bool checkSum[ROW][COL])
{
   char filePoint[256] = { 0 };
   char input;
   cout << "> ";
   cin >> input;
   switch ((char)toupper(input))
   {
      case '?':
         dispCommands();
         cout << "\n";
         break;
      case 'D':
         dispBoardColor(board, checkSum);
         cout << "\n";
         break;
      case 'E':
         editSquare(board, checkSum);
         break;
      case 'S':
         showSolution(board);
         break;
      case 'Q':
         cout << "What file would you like to write your board to: ";
         cin >> fileName;
         writeFile(fileName, board, checkSum);
         return true;
         break;
      case 'F':
         startSolve(board, checkSum);
         break;
      default:
         cout << "ERROR: Invalid command\n\n";
   }
   return false;
}

/**********************************************************************
* editSquare(board[][]) prompts user for square to edit and what to place
*  in the square.
***********************************************************************/
void editSquare(int board[ROW][COL], bool checkSum[ROW][COL])
{
   char col = ' ';
   char cRow = ' ';
   int row = 0;
   char colPHolder = ' ';
   int input = 0;
   bool validLocation = false;
   while (!validLocation)
   {
      validLocation = true;
      cout << "What are the coordinates of the square: ";
      cin >> col >> cRow;
      //check for what you got
      if ((int)cRow > 57 || (int)cRow < 49)
      {
         colPHolder = col;
         col = cRow;
         cRow = colPHolder;
      }
      //preform char to int conversion ascii 0 = 48
      row = (int)cRow - 48;
      row--; //adjust for sliding
      //check for valid square
      if (row < 0 || row > 9 || 
          toupper(col) - 65 < 0 || toupper(col) - 65 > 9)
      {
         cout << "ERROR: Square '" << col << cRow;
         cout << "' is invalid\n";
         validLocation = false;
      }
   }
   //check for read protection
   if (checkSum[row][toupper(col) - 65])
   {
      cout << "ERROR: Square '" << (char)toupper(col) << row + 1;
      cout << "' is read-only\n\n";
      return;
   }

   cout << "What is the value at '" << (char)toupper(col) << row + 1
      << "': ";
   cin.ignore();
   cin >> input;
   //check if the number is valid
   if (!validateNumber(board, row, toupper(col) - 65, input))
   {
      cout << "ERROR: Value '" << input << "' in square '"
           << (char)toupper(col) << row + 1 << "' is invalid\n\n";
      return;
   }
   switch ((char)toupper(col))
   {
      case 'A':
         board[row][0] = input;
         break;
      case 'B':
         board[row][1] = input;
         break;
      case 'C':
         board[row][2] = input;
         break;
      case 'D':
         board[row][3] = input;
         break;
      case 'E':
         board[row][4] = input;
         break;
      case 'F':
         board[row][5] = input;
         break;
      case 'G':
         board[row][6] = input;
         break;
      case 'H':
         board[row][7] = input;
         break;
      case 'I':
         board[row][8] = input;
         break;
      default:
         cout << "ERROR217:";
   }
   cout << "\n";
}

/**********************************************************************
* computeValues(board, posRow, posCol, solutions) computes the valid
* numbers for a given square. returns the values in an array solutions
***********************************************************************/
int computeValues(int board[ROW][COL], int posRow, int posCol, 
                  int solutions[10])
{
   bool validNum[10];
   for (int i = 0; i < 9; i++)
      validNum[i] = true;
   int box = 0;
   int posBoxRow = 0;
   int posBoxCol = 0;
   int * pPosBoxRow = &posBoxRow;
   int * pPosBoxCol = &posBoxCol;

   //checking row
   for (int j = 0; j < COL; j++)
   {
      if (board[posRow][j] != 0)
      {
         validNum[board[posRow][j] - 1] = false;
      }
   }

   //checking columns
   for (int i = 0; i < ROW; i++)
   {
      if (board[i][posCol] != 0)
      {
         validNum[board[i][posCol] - 1] = false;
      }
   }

   //consider the box it is in
   if (findBox(posRow, posCol, pPosBoxRow, pPosBoxCol) != 0)
   {
      cout << "ERROR INVALID BOX";
   }

   for (int i = posBoxRow * 3; i < (posBoxRow + 1) * 3; i++)
   {
      for (int j = posBoxCol * 3; j < (posBoxCol + 1) * 3; j++)
      {
         if (board[i][j] != 0)
            validNum[board[i][j] - 1] = false;
      }
   }
   //copy valid numbers over
   clearSolutions(solutions, 10);
   for (int i = 0; i < 9; i++)
   {
      if (validNum[i])
      {
         solutions[i] = i + 1;
      }
   }
   return 0;
}

/**********************************************************************
* findBox(posRow, posCol, pPosBoxRow, pPosBoxCol) calculates the position
*  of a given box in a square and returns the results through the pointers
*  pPosBoxRow and pPosBoxCol.
***********************************************************************/
int findBox(int posRow, int posCol, int *pPosBoxRow, int *pPosBoxCol)
{
   if (posRow < 3)
   {
      *pPosBoxRow = 0;
      if (posCol < 3)
      {
         *pPosBoxCol = 0;
         return 0;
      }
      if (posCol < 6 && posCol >= 3)
      {
         *pPosBoxCol = 1;
         return 0;
      }
      if (posCol < 9 && posCol >= 6)
      {
         *pPosBoxCol = 2;
         return 0;
      }
   }
   if (posRow < 6 && posRow >= 3)
   {
      *pPosBoxRow = 1;
      if (posCol < 3)
      {
         *pPosBoxCol = 0;
         return 0;
      }
      if (posCol < 6 && posCol >= 3)
      {
         *pPosBoxCol = 1;
         return 0;
      }
      if (posCol < 9 && posCol >= 6)
      {
         *pPosBoxCol = 2;
         return 0;
      }
   }
   if (posRow < 9 && posRow >= 6)
   {
      *pPosBoxRow = 2;
      if (posCol < 3)
      {
         *pPosBoxCol = 0;
         return 0;
      }
      if (posCol < 6 && posCol >= 3)
      {
         *pPosBoxCol = 1;
         return 0;
      }
      if (posCol < 9 && posCol >= 6)
      {
         *pPosBoxCol = 2;
         return 0;
      }
   }
   return -1;
}

/**********************************************************************
* generageCheckSum(board, checkSum) sets box to true if it already has
*  a number in it.
***********************************************************************/
void generateCheckSum(int board[ROW][COL], bool checkSum[ROW][COL])
{
   for (int i = 0; i < ROW; i++)
   {
      for (int j = 0; j < COL; j++)
      {
         if (board[i][j] != 0) //already has information then set true
         {
            if (board[i][j] - 10 < 0)
            {
               checkSum[i][j] = true;
            }
            else
            {
               checkSum[i][j] = false;
               board[i][j] = board[i][j] - 10;
            }
         }
         else
         {
            checkSum[i][j] = false;
         }
      }
   }
}
//NEW FUNCTIONS

/**********************************************************************
* dispBoardColor(board, checkSum) will display the board with color 
*   delineating user input from file. Uses ascii escape codes for color
*   WILL NOT work with windows os included are compiler definitions for
*   use with windows os use #define WINDOWS.
***********************************************************************/
void dispBoardColor(int board[ROW][COL], bool checkSum[ROW][COL])
{
   #ifdef WINDOWS
      HANDLE hConsole;
      hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //is valid for windows
   #endif

   cout << "   A B C D E F G H I\n";
   for (int i = 0; i < ROW; i++)
   {
      cout << i + 1 << "  ";
      for (int j = 0; j < COL; j++)
      {
         if (board[i][j] == 0)
         {
            cout << " ";
         }
         else
         {
            if (checkSum[i][j])
            {
               colorGreen(hConsole);
               cout << board[i][j];
               colorWhite(hConsole);
            }
            else
            {
               cout << board[i][j];
            }
         }
         
         if ((j + 1) % 3 == 0 && j != 8)
         {
            cout << "|";
         }
         else if (j != 8)
         {
            cout << " ";
         }
      }
      cout << "\n";
      if ((i + 1) % 3 == 0 && i != 8)
         cout << "   -----+-----+-----\n";
   }
}

/**********************************************************************
* validateNumber(board, posRow, posCol, number) This function checks
*   board at location posRow & posCol, to see if number is valid 
*   returns true if number = valid
*   returns false if number !isValid
***********************************************************************/
bool validateNumber(int board[ROW][COL], int posRow, int posCol, int number)
{
   bool validNum[9];
   for (int i = 0; i < 9; i++)
      validNum[i] = true;
   int box = 0;
   int posBoxRow = 0;
   int posBoxCol = 0;

   //checking row
   for (int j = 0; j < COL; j++)
   {
      if (j == posCol)
      {
         if (j == 8)
            break;
         j++; //do not scan the square you are checking
      }
      if (board[posRow][j] != 0)
      {
         validNum[board[posRow][j] - 1] = false;
      }
   }

   //checking columns
   for (int i = 0; i < ROW; i++)
   {
      if (i == posRow)
      {
         if (i == 8)
            break;
         i++; //do not scan the square you are checking
      }
      if (board[i][posCol] != 0)
      {
         validNum[board[i][posCol] - 1] = false;
      }
   }

   //consider the box it is in
   if (findBox(posRow, posCol, &posBoxRow, &posBoxCol) != 0)
   {
      cout << "ERROR INVALID BOX";
   }

   for (int i = posBoxRow * 3; i < (posBoxRow + 1) * 3; i++)
   {
      for (int j = posBoxCol * 3; j < (posBoxCol + 1) * 3; j++)
      {
         if (j == posCol && i == posRow)
         {
            break; //do not scan the square you are checking
         }
         if (board[i][j] != 0)
            validNum[board[i][j] - 1] = false;
      }
   }
   for (int i = 0; i < 9; i++)
   {
      if (validNum[i])
      {
         if (number == i + 1)
         {
            return true;
         }
      }
   }
   return false;
}

/**********************************************************************
* showSolution(board) This function requests a square from the user and
*   then checks the square for vaild numbers which are outputed to the
*   screen.
***********************************************************************/
void showSolution(int board[ROW][COL])
{
   char col = ' ';
   char cRow = ' ';
   int row = 0;
   char colPHolder = ' ';
   cout << "What are the coordinates of the square: ";
   cin >> col >> cRow;
   //check for what you got
   if ((int)cRow > 57 || (int)cRow < 49)
   {
      colPHolder = col;
      col = cRow;
      cRow = colPHolder;
   }
   //preform char to int conversion ascii 0 = 48
   row = (int)cRow - 48;
   row--;
   int solutions[10];
   bool first = true;
   cout << "The possible values for '" << (char) toupper(col)
        << row + 1 << "' are: ";
   computeValues(board, row, toupper(col) - 65, solutions);
   int test = toupper(col) - 65;
   for (int i = 0; i < 9; i++)
   {
      if (solutions[i] != -1)
      {
         if (first)
         {
            cout << i + 1;
            first = false;
         }
         else
         {
            cout << ", " << i + 1;
         }
      }
   }
   cout << "\n\n";
}

/**********************************************************************
* validBoard(board) This function checks the file's board for errors
*    returns true if valid board.
***********************************************************************/
bool validBoard(int board[ROW][COL])
{
   for (int i = 0; i < ROW; i++)
   {
      for (int j = 0; j < COL; j++)
      {
         if (board[i][j] != 0)
         {
            if (!validateNumber(board, i, j, board[i][j]))
            {
               cout << "ERROR: Duplicate value '" << board[i][j]
                    << "' in inside square represented by '"
                    << intColToChar(j) << i + 1 << "'\n";
               return false;
            }
         }
      }
   }
   return true;
}

/**********************************************************************
* intColToChar(board) Converts from int to char for 0-8 for A-I Respectfully
***********************************************************************/
char intColToChar(int j)
{
   switch (j)
   {
      case 0:
         return 'A';
      case 1:
         return 'B';
      case 2:
         return 'C';
      case 3:
         return 'D';
      case 4:
         return 'E';
      case 5:
         return 'F';
      case 6:
         return 'G';
      case 7:
         return 'H';
      case 8:
         return 'I';
   }
}
//SOLVER SECTION

/**********************************************************************
* solveBoard(board) preforms a solve using the only one possible solution
*   for the square method did not solve the boards but I left it in for 
*   the heck of it.
***********************************************************************/
int solveBoard(int board[ROW][COL])
{
   bool toSolve[ROW][COL];
   int testSolve[ROW][COL];
   bool isFirst = true;
   int posibleSolution[ROW][COL][9]; //row col possible solutions
   int w = 0;

   testAllSquares(board, posibleSolution);
   // okay now we have the posible solutions for every square ...
   // Lets solve this monster
   //but first a test of the contents of posibleSolutions
   for (int i = 0; i < ROW; i++)
   {
      for (int j = 0; j < COL; j++)
      {
         for (int k = 0; k < 9; k++)
         {
            if (posibleSolution[i][j][k] != -1)
            {
               cout << posibleSolution[i][j][k];
               w++;
            }
            else
            {
               cout << "_";
            }
         }
         cout << "   ";
      }
      cout << "\n";
   }
   cout << "\n";
   generateCheckSum(board, toSolve);
   dispBoard(board);
   cout << "\n";

   int times = 0;
   while (true)
   {
      if (!solve(board, posibleSolution))
         break;
      dispBoard(board);
      cout << "\n";
      testAllSquares(board, posibleSolution);
      if (times == 81)
      {
         break;
      }
      else
      {
         times++;
      }
      bool finished = true;
      for (int i = 0; i < ROW; i++)
      {
         for (int j = 0; j < COL; j++)
         {
            for (int k = 0; k < 9; k++)
            {
               if (posibleSolution[i][j][k] != -1)
                  finished = false;
            }
         }
      }
      if (finished)
         break;
   }
   cout << "\n\n SOLVED\n";
   dispBoard(board);

   return 0;
}

/**********************************************************************
* testAllSquares(board, posibleSolution) This function tests every square
*   in the board for posible solutions and returns them in posibleSolutions
***********************************************************************/
void testAllSquares(int board[ROW][COL], int posibleSolution[ROW][COL][9])
{
   int solutions[10];
   clearSolutions(solutions, 9);
   for (int i = 0; i < ROW; i++)
   {
      for (int j = 0; j < COL; j++)
      {
         if (board[i][j] == 0)
         {
            computeValues(board, i, j, solutions);
            for (int k = 0; k < 9; k++)
            {
               posibleSolution[i][j][k] = solutions[k];
            }
         }
         else
         {
            for (int k = 0; k < 9; k++)
            {
               posibleSolution[i][j][k] = -1;
            }
         }
      }
   }
}

/**********************************************************************
* copyBoard(boardToCopy, boardCopiedTo) copies contents of one board to 
*   another.
***********************************************************************/
void copyBoard(int boardToCopy[ROW][COL], int boardCopiedTo[ROW][COL])
{
   for (int i = 0; i < ROW; i++)
   {
      for (int j = 0; j < COL; j++)
      {
         boardCopiedTo[i][j] = boardToCopy[i][j];
      }
   }
}

/**********************************************************************
* solveRecursive(board, checkSum, posSolutions, row, col, index)
*   This is the way how everything gets solved, it is very complicated
*   but I will try to explain it. The way how it works is it checks the
*   current square to see if it has content and is not part of the 
*   original board. if it is not increment the square by one untill it is
*   a valid solution for the box. Then it assumes it is correct and moves
*   forward to the next box. It continues this way untill it finds a box
*   that does not have a valid solution. It then moves backwards to the 
*   previous box and increments it by one, if it does not have any valid
*   solutions left it moves back again.
*      The function has two exit conditions if the puzzle cannot be solved
*   returns false with an error message. 
*      If the rows hit 9 then it has solved the puzzle and returns false
*   and displays the finished board to the screen.
*      it will display the current state of solve every 1000000 times it 
*   touches a box. It took 2 minutes to solve board 100 on my i7 processor
*   So be patient please.
***********************************************************************/
bool solveRecusive(int board[ROW][COL], bool checkSum[ROW][COL],
   int posSolutions[ROW][COL][9], int *row, int *col, int *index, int speed)
{
   int i = board[*row][*col];
   int prevI = i;
   //if row is 9 then we are done.
   if (*row == 9)
   {
      return false;
   }
   if (*col >= 9)
   {
      *row += 1;
      *col = 0;
      return true;
   }
   //importaint to ensure this is to looped on when recusing
   if (checkSum[*row][*col])
   {
      if (*col >= 9)
      {
         *row += 1;
         *col = 0;
         return true;
      }
      else
      {
         *col += 1;
         return true;
      }
   }
   if (*index % speed == 0) //TODO SET TO 1000000
   {
      clearScreen;
      cout << "\n indexing " << *index << "\n";
      dispBoardColor(board, checkSum);
   }
   *index += 1;
   //was here
   for (; i < 10; i++)
   {
      //check if it is still valid
      if (validateNumber(board, *row, *col, i))
      {
         //if i is curently a valid solution
         if (i != prevI && i == posSolutions[*row][*col][i - 1])
         {
            board[*row][*col] = i;
            //solution found so move on to the next square
            *col += 1;
            return true;
         }
      }
   }
   //no valid square for the current square
   board[*row][*col] = 0; //set square to zero
   if (*col == 0) //if at left side of board
   {
      *row -= 1; //recuse one row
      *col = 8; // start recusive at right side
   }
   else //otherwise
   {
      *col -= 1; // recuse a square to the left
   }
   //while the current square is preset by file
   while (checkSum[*row][*col])
   {
      //same as above
      if (*col == 0)
      {
         *row -= 1;
         *col = 8;
      }
      else
      {
         *col -= 1;
      }
      //if you are back before the beginning then no valid
      //solution avaliable.
      if (*row < 0)
         return -1; //return error
   }
   //don't have to worry about it being solved as we know
   //we are in an error state for current square.
   //else move to the square specifed above;
   return true;
}

/**********************************************************************
* solve(board, posibleSolution) checks the square to see if it only
*   has one valid solution.
***********************************************************************/
bool solve(int board[ROW][COL], int posibleSolution[ROW][COL][9])
{
   for (int i = 0; i < ROW; i++)
   {
      for (int j = 0; j < COL; j++)
      {
         int counter = 0;
         int solution = 0;
         for (int k = 0; k < 9; k++)
         {
            if (posibleSolution[i][j][k] != -1)
            {
               solution = posibleSolution[i][j][k];
               counter++;
            }
         }
         if (counter == 1)
            board[i][j] = solution;
      }
   }
   return true;
}

/**********************************************************************
* startSolve(board, checkSum) starts the solver for sudoku board
***********************************************************************/
void startSolve(int board[ROW][COL], bool checkSum[ROW][COL])
{
   int row = 0;
   int col = 0;
   int index = 0;
   

   cout << "THIS WILL TAKE A WHILE!\n";
   cout << "It takes 2 minutes at 2,800,000 instructions per second\n";
   cout << "your mileage may vary :) !\n";
   int speed = 0;
   cout << "Choose Solving Speed 0 is slowest 2 is fastest: ";
   cin >> speed;
   switch (speed)
   {
   case 0:
      speed = 1;
      break;
   case 1:
      speed = 10000;
      break;
   case 2:
      speed = 1000000;
      break;
   default:
      speed = 1000000;
   }
   int posibleSolution[ROW][COL][9];
   testAllSquares(board, posibleSolution);
   dispBoardColor(board, checkSum);
   bool notSolved = true;
   while (notSolved)
   {
      notSolved = solveRecusive(board, checkSum, posibleSolution, &row, &col, &index, speed);
   }
   clearScreen;
   cout << "\n This took '" << index << "' many steps\n";
   dispBoardColor(board, checkSum);
}
