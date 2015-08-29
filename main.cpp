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
void MaximizeWindow(int &horizontal, int &vertical);
void ShowHelp();
void ExitProgram();
void AutoTests(int &year, int &month, int &day);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //set up date variables which will be default set to current date and can be custom-set and restored to default by user. Then display welcome and version.
    int year = 0;
    int month = 0;
    int day = 0;
    cout << "Welcome to potatOS!\n";
    DisplayVersion();
    //get and store screen resolution. The functions I've used to get resolution work (as the cout function below shows) but for some reason setting the window size won't work
    int horizontal = 0;
    int vertical = 0;
    MaximizeWindow(horizontal, vertical);
    cout << "Screen Resolution: " << horizontal << " X " << vertical << endl;
    //Call the AutoTests function to show some functionality automatically
    AutoTests(year, month, day);

    InputLoop();
    ExitProgram();
    return a.exec();
}

void InputLoop()
{
    int year;
    int month;
    int day;
    RestoreCurrentDate(year, month, day);
    while (1)
    {
        string in;
        cout << "Enter a command: ";
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
            day = GetValidInt(0,31);
            month = GetValidInt(1,12);
            year = GetValidInt(0,9999);
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
    cout << "DATE:" << month << '-' << day << '-' << year << endl;
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
    //can't figure out how to get the filepath of the current directory but using an absolute filepath works
    if ((dir = opendir ("C:/Users/Sam/Documents/GitHub/potatOS")) != NULL)
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
void MaximizeWindow(int &horizontal, int &vertical)
{
   RECT screen;
   const HWND hDesktop = GetDesktopWindow();
   GetWindowRect(hDesktop, &screen);
   horizontal = screen.right;
   vertical = screen.bottom;

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
