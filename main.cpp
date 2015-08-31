#include <QCoreApplication>
#include <iostream>
#include "wtypes.h"
#include <windows.h>
#include <ctime>
#include <limits>
#include "dirent.h"
#include <algorithm>
#include <string>
#include <unistd.h>
using namespace std;

void InputLoop();
void DisplayDate(int year, int month, int day);
void RestoreCurrentDate(int &year, int &month, int &day);
int GetValidInt(int min, int max);
void DirectoryFiles();
void DisplayVersion();
void MaximizeWindow();
void ShowHelp();
void ExitProgram();
void AutoTests(int &year, int &month, int &day);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Call the AutoTests function to show some functionality automatically
    //AutoTests(year, month, day);

    InputLoop();
    return a.exec();
}

//a big loop set up to implement the command prompt, lets the user repeatedly enter in commands
void InputLoop()
{
    //display a welcome message and the version number as well as set up the date variables and initialize them to the current date retrieved from the OS
    cout << "Welcome to potatOS!\n";
    DisplayVersion();
    int year;
    int month;
    int day;
    RestoreCurrentDate(year, month, day);

    //The functions I've used to get resolution work but for some reason setting the window size won't work
    MaximizeWindow();

    while (1)
    {
        string in;
        cout << "\nEnter a command: ";
        cin >> in;
        transform(in.begin(), in.end(), in.begin(), ::tolower);
        if (in.compare("version") == 0)
        {
            DisplayVersion();
        }
        else if (in.compare("displaydate") == 0)
        {
            DisplayDate(year, month, day);
        }
        else if (in.compare("setdate") == 0)
        {
            //Prompt the user to enter a custom date
            day = GetValidInt(1,31);
            month = GetValidInt(1,12);
            year = GetValidInt(1,9999);
            DisplayDate(year, month, day);
        }
        else if (in.compare("restoredate") == 0)
        {
            RestoreCurrentDate(year, month, day);
        }
        else if (in.compare("directoryfiles") == 0)
        {
            DirectoryFiles();
        }
        else if (in.compare("help") == 0)
        {
            ShowHelp();
        }
        else if (in.compare("exit") == 0)
        {
            ExitProgram();
        }
        else
        {
            cout << "Invalid command (try 'help' for a list of valid commands)" << endl;
        }
    }
}

//Simple print date function
void DisplayDate(int year, int month, int day)
{
    cout << "DATE:" << month << '-' << day << '-' << year << "\n\n";
}

//get current date info and set variable accordingly
void RestoreCurrentDate(int &year, int &month, int &day)
{
    time_t t = time(0);
    struct tm * now = localtime( & t );
    year = now->tm_year + 1900;
    month = now->tm_mon + 1;
    day = now->tm_mday;
}

//check to see if user input number is valid based on bounds. Code borrowed from comment section of:  http://r3dux.org/2011/11/how-to-get-valid-integer-input-in-c-a-stupidly-long-solution-to-a-stupidly-simple-problem/
int GetValidInt(int min, int max)
{
    int number;
    if (max == 31)
    {
        cout << "Changing Day: ";
    }
    else if (max == 12)
    {
        cout << "Changing Month: ";
    }
    else
    {
        cout << "Changing Year: ";
    }
    while ( (cout << "Enter a valid number: " && !(cin >> number) )  || (number < min) || (number > max) || cin.peek() != '\n')
    {
        cout << "You must input a whole number between " << min << " and " << max << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return number;
}

//print all of the file names in the current directory
void DirectoryFiles()
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("../potatOS")) != NULL)
    {
      //print all the files and directories within directory
      cout << "\nDIRECTORY FILES: ";
      while ((ent = readdir (dir)) != NULL)
      {
        printf ("%s\n", ent->d_name);
      }
      closedir (dir);
    }
    else
    {
      /* could not open directory */
      perror ("");
    }
}

//simple print function to display version number
void DisplayVersion()
{
    cout << "\n--potatOS Version 0.0.5--\n\n";
}

/*This code was borrowed from: http://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c
 * This function gets the resolution of the computer screen and sets the size but for some reason it won't work*/
void MaximizeWindow()
{
   RECT screen;
   const HWND hDesktop = GetDesktopWindow();
   GetWindowRect(hDesktop, &screen);
   int horizontal = screen.right;
   int vertical = screen.bottom;

   _COORD coord;
   coord.X = horizontal - 1;
   coord.Y = vertical - 1;

   _SMALL_RECT Rect;
   Rect.Top = 0;
   Rect.Left = 0;
   Rect.Bottom = screen.bottom;
   Rect.Right= screen.right;

   HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
   SetConsoleScreenBufferSize(Handle, coord);
   SetConsoleWindowInfo(Handle, TRUE, &Rect);

   //this print statement proves that the screen resolution values are accurately being attained
   //cout << "Screen Resolution: " << horizontal << " X " << vertical << endl;
}

//display each valid user command as well as what they do
void ShowHelp()
{
    cout << "VALID COMMANDS:\n";
    cout << "version -------- Displays the current version of the OS.\n" ;
    cout << "displaydate ---- Displays the date.\n";
    cout << "setdate -------- Allows user to set the date.\n";
    cout << "restoredate ---- Sets the date to the current date.\n";
    cout << "directoryfiles - Displays a list of all of the files in the OS directory.\n";
    cout << "help ----------- Displays a list of all valid user commands as well as their function.\n";
    cout << "exit ----------- Ends the current session and exits the OS.\n\n";
}

//Exits the program
void ExitProgram()
{
    while (1)
    {
        string in;
        cout << "\nARE YOU SURE YOU WISH TO EXIT?\n'y' or 'yes' will terminate the program\n'n' or 'no' will return back to the command prompt: ";
        cin >> in;
        transform(in.begin(), in.end(), in.begin(), ::tolower);
        if (in.compare("yes") == 0 || in.compare("y") == 0)
        {
            break;
        }
        else if (in.compare("no") == 0 || in.compare("n") == 0)
        {
            cout << "==== PROGRAM EXIT ABORTED ====" << endl;
            return;
        }
        else
        {
            cout << "ERROR: You have entered an invalid response." << endl;
        }

    }
    cout << "\n\n==== PROGRAM EXECUTION COMPLETE ====\n\n";
    exit(0);
}

//Perform some automated tests to show that certain functions work
void AutoTests(int &year, int &month, int &day)
{
    cout << "\n==AUTOMATED TESTS==\n\n";
    //default set date values to current date and then display date.
    RestoreCurrentDate(year, month, day);
    DisplayDate(year, month, day);
    //Displays all of the files located in the OS directory
    DirectoryFiles();
    //Example to show that commands are not case sensitive
    string in;
    cout << "Enter a command: ";
    in = "VERSION";
    //transform function changes to all lowercase
    transform(in.begin(), in.end(), in.begin(), ::tolower);
    cout << in << endl;
    if (in.compare("version") == 0)
    {
        DisplayVersion();
    }
    ShowHelp();
}
