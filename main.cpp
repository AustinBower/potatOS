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
#include <queue>
#define APP true
#define SYS false
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define NOT_SUSPENDED 0
#define SUSPENDED 1
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
//PART 2 FUNCTIONS
struct PCB* AllocatePCB();
void FreePCB(struct PCB* inPCB);
struct PCB* SetupPCB(string name, int priority, bool classType);
struct PCB* FindPCB(string name, queue<struct PCB*> ready, queue<struct PCB*> blocked);
void InsertPCB(struct PCB* inPCB, queue<struct PCB*> *ready, queue<struct PCB*> *blocked);
void RemovePCB(struct PCB* inPCB, queue<struct PCB*> *ready, queue<struct PCB*> *blocked);
string GetPCBname();

struct PCB{
    string processName;
    bool processClass;//application(true) or system(false) type
    int priority;//int from -127 to +128
    int stateOne;//0=running, 1=ready, 2=blocked
    int stateTwo;//0=not suspended, 1=suspended
    int memory;
    struct PCB* next;
    struct PCB* previous;
};

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
    queue<struct PCB*> readyQueue;
    queue<struct PCB*> blockedQueue;

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
        else if (in.compare("createpcb") == 0)
        {
            while (1)
            {
                //check to see if the user enters in a valid, unique name for the PCB
                string name = GetPCBname();
                if (FindPCB(name, readyQueue, blockedQueue) == NULL)
                {
                    while (1)
                    {
                        //check to see if the user enters in a valid class type
                        int classTypeIn;
                        cout << "\nEnter the class type of the new PCB (*must be application or system type, enter '1' for application or '0' for system*): ";
                        cin >> classTypeIn;
                        if (classTypeIn == 1 || classTypeIn == 0)
                        {
                            //set class type boolean variable accordingly
                            bool classType;
                            if (classTypeIn == 1)
                            {
                                classType = APP;
                            }
                            else
                            {
                                classType = SYS;
                            }
                            while (1)
                            {
                                //check to see if the user enters a valid integer for priority
                                int priority;
                                cout << "\nEnter the priority of the new PCB (*must be in the range of -127 to +128*): ";
                                cin >> priority;
                                if (priority >= -127 && priority <= 128)
                                {
                                    struct PCB* newPCB = SetupPCB(name, priority, classType);
                                    InsertPCB(newPCB, &readyQueue, &blockedQueue);
                                    goto OUTER;
                                }
                                else
                                {
                                    cout << "*ERROR: The input priority value was invalid. Try entering an integer between -127 and +128*" << endl;
                                }
                            }
                        }
                        else
                        {
                            cout << "*ERROR: The input class type was invalid. Try entering in either '1' for app or '2' for sys type*" << endl;
                        }
                    }


                }
                else
                {
                    cout << "*ERROR: The input name was not unique, try entering a new name*" << endl;
                }
            }
            OUTER:
            cout << "PCB Successfully created" << endl;
        }
        else if (in.compare("deletepcb") == 0)
        {
            //check to see if the user enters in an existing name for the PCB
            string name = GetPCBname();
            struct PCB* removed = FindPCB(name, readyQueue, blockedQueue);
            if (removed != NULL)
            {
                RemovePCB(removed, &readyQueue, &blockedQueue);
                FreePCB(removed);
                cout << "PCB Successfully deleted" << endl;
            }
            else
            {
                cout << "*ERROR: No PCB found with that name*" << endl;
            }
        }
        else if (in.compare("block") == 0)
        {
            //check to see if the user enters in an existing name for the PCB
            string name = GetPCBname();
            struct PCB* removed = FindPCB(name, readyQueue, blockedQueue);
            if (removed != NULL)
            {
                RemovePCB(removed, &readyQueue, &blockedQueue);
                removed->stateOne = BLOCKED;
                InsertPCB(removed, &readyQueue, &blockedQueue);
                cout << "Successfully changed the PCB to the blocked status" << endl;
            }
            else
            {
                cout << "*ERROR: No PCB found with that name*" << endl;
            }
        }
        else if (in.compare("unblock") == 0)
        {
            //check to see if the user enters in an existing name for the PCB
            string name = GetPCBname();
            struct PCB* removed = FindPCB(name, readyQueue, blockedQueue);
            if (removed != NULL)
            {
                RemovePCB(removed, &readyQueue, &blockedQueue);
                removed->stateOne = READY;
                InsertPCB(removed, &readyQueue, &blockedQueue);
                cout << "Successfully changed the PCB to the unblocked status" << endl;
            }
            else
            {
                cout << "*ERROR: No PCB found with that name*" << endl;
            }
        }
        else if (in.compare("suspend") == 0)
        {
            //check to see if the user enters in an existing name for the PCB
            string name = GetPCBname();
            struct PCB* change = FindPCB(name, readyQueue, blockedQueue);
            if (change != NULL)
            {
                change->stateTwo = SUSPENDED;
                cout << "Successfully suspended the PCB" << endl;
            }
            else
            {
                cout << "*ERROR: No PCB found with that name*" << endl;
            }
        }
        else if (in.compare("resume") == 0)
        {
            //check to see if the user enters in an existing name for the PCB
            string name = GetPCBname();
            struct PCB* change = FindPCB(name, readyQueue, blockedQueue);
            if (change != NULL)
            {
                change->stateTwo = NOT_SUSPENDED;
                cout << "Successfully resumed the PCB" << endl;
            }
            else
            {
                cout << "*ERROR: No PCB found with that name*" << endl;
            }
        }
        else if (in.compare("setpriority") == 0)
        {
            //check to see if the user enters in an existing name for the PCB
            string name = GetPCBname();
            struct PCB* change = FindPCB(name, readyQueue, blockedQueue);
            if (change != NULL)
            {
                int newPriority;
                cout << "\nEnter the new priority level for this PCB (must be between -127 to +128): ";
                cin >> newPriority;
                if (newPriority >= -127 && newPriority <= 128)
                {
                    change->priority = newPriority;
                    cout << "Successfully changed the PCB's priority" << endl;
                }
                else
                {
                    cout << "*ERROR: The input priority value is invalid*" << endl;
                }
            }
            else
            {
                cout << "*ERROR: No PCB found with that name*" << endl;
            }
        }
        else if (in.compare("showpcb") == 0)
        {
            string name = GetPCBname();
            struct PCB* toShow = FindPCB(name, readyQueue, blockedQueue);
            if (toShow != NULL)
            {
                cout << "  ******* PCB INFORMATION *******" << endl;
                cout << "    Name ---------- " << toShow->processName << endl;
                if (toShow->processClass == APP)
                {
                    cout << "    Class --------- Application" << endl;
                }
                else
                {
                    cout << "    Class --------- System" << endl;
                }
                cout << "    Priority ------ " << toShow->priority << endl;
                if (toShow->stateOne == RUNNING)
                {
                    cout << "    State One ----- Running" << endl;
                }
                else if (toShow->stateOne == READY)
                {
                    cout << "    State One ----- Ready" << endl;
                }
                else
                {
                    cout << "    State One ----- Blocked" << endl;
                }
                if (toShow->stateTwo == SUSPENDED)
                {
                    cout << "    State Two ----- Suspended" << endl;
                }
                else
                {
                    cout << "    State Two ----- Not Suspended" << endl;
                }
                cout << "    Memory -------- " << toShow->memory << endl;
                cout << "  ***** END PCB INFORMATION *****" << endl;
            }
            else
            {
                cout << "*ERROR: No PCB found with that name*" << endl;
            }
        }
        else if (in.compare("showall") == 0)
        {
            cout << "  ******* PCB INFORMATION *******" << endl;
            if (readyQueue.size() == 0 && blockedQueue.size() == 0)
            {
                cout << "*ERROR: There are currently no PCBs to show information*" << endl;
            }
            if (readyQueue.size() != 0)
            {
                struct PCB* toShow = readyQueue.front();
                for (unsigned int i = 0; i < readyQueue.size(); i++)
                {
                    cout << "    Name -- " << toShow->processName << " || ";
                    cout << "    Priority -- " << toShow->priority << " || ";
                    if (toShow->stateOne == RUNNING)
                    {
                        cout << "    State One -- Running" << " || ";
                    }
                    else if (toShow->stateOne == READY)
                    {
                        cout << "    State One -- Ready" << " || ";
                    }
                    else
                    {
                        cout << "    State One -- Blocked" << " || ";
                    }
                    if (toShow->stateTwo == SUSPENDED)
                    {
                        cout << "    State Two -- Suspended" << endl;
                    }
                    else
                    {
                        cout << "    State Two -- Not Suspended" << endl;
                    }
                    toShow = toShow->next;
                }
            }
            if (blockedQueue.size() != 0)
            {
                struct PCB* toShow = blockedQueue.front();
                for (unsigned int i = 0; i < blockedQueue.size(); i++)
                {
                    cout << "    Name -- " << toShow->processName << " || ";
                    cout << "    Priority -- " << toShow->priority << " || ";
                    if (toShow->stateOne == RUNNING)
                    {
                        cout << "    State One -- Running" << " || ";
                    }
                    else if (toShow->stateOne == READY)
                    {
                        cout << "    State One -- Ready" << " || ";
                    }
                    else
                    {
                        cout << "    State One -- Blocked" << " || ";
                    }
                    if (toShow->stateTwo == SUSPENDED)
                    {
                        cout << "    State Two -- Suspended" << endl;
                    }
                    else
                    {
                        cout << "    State Two -- Not Suspended" << endl;
                    }
                    toShow = toShow->next;
                }
            }
            cout << "  ***** END PCB INFORMATION *****" << endl;
        }
        else if (in.compare("showready") == 0)
        {
            if (readyQueue.size() != 0)
            {
                struct PCB* toShow = readyQueue.front();
                for (unsigned int i = 0; i < readyQueue.size(); i++)
                {
                    cout << "    Name -- " << toShow->processName << " || ";
                    cout << "    Priority -- " << toShow->priority << " || ";
                    cout << "    State One -- Ready" << " || ";
                    if (toShow->stateTwo == SUSPENDED)
                    {
                        cout << "    State Two -- Suspended" << endl;
                    }
                    else
                    {
                        cout << "    State Two -- Not Suspended" << endl;
                    }
                    toShow = toShow->next;
                }
            }
            else
            {
                cout << "*ERROR: There are no PCBs in the ready state*" << endl;
            }
        }
        else if (in.compare("showblocked") == 0)
        {
            if (blockedQueue.size() != 0)
            {
                struct PCB* toShow = blockedQueue.front();
                for (unsigned int i = 0; i < blockedQueue.size(); i++)
                {
                    cout << "    Name -- " << toShow->processName << " || ";
                    cout << "    Priority -- " << toShow->priority << " || ";
                    cout << "    State One -- Blocked" << " || ";
                    if (toShow->stateTwo == SUSPENDED)
                    {
                        cout << "    State Two -- Suspended" << endl;
                    }
                    else
                    {
                        cout << "    State Two -- Not Suspended" << endl;
                    }
                    toShow = toShow->next;
                }
            }
            else
            {
                cout << "*ERROR: There are no PCBs in the blocked state*" << endl;
            }
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
    if ((dir = opendir ("./")) != NULL)
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
    cout << "\n--potatOS Version 0.2.0--\n\n";
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
    cout << "*note-- commands are NOT case sensitive *\n\n";
    cout << "version -------- Displays the current version of the OS.\n";
    cout << "displaydate ---- Displays the date.\n";
    cout << "setdate -------- Allows user to set the date.\n";
    cout << "restoredate ---- Sets the date to the current date.\n";
    cout << "directoryfiles - Displays a list of all of the files in the OS directory.\n";
    cout << "createpcb ------ Allows the user to create a PCB.\n";
    cout << "deletepcb ------ Deletes a PCB with a user specified name.\n";
    cout << "block ---------- Changes a user specified PCB's state to blocked.\n";
    cout << "unblock -------- Changes a user specified PCB's state to ready.\n";
    cout << "suspend -------- Changes a user specified PCB's state to suspended.\n";
    cout << "resume --------- Changes a user specified PCB's state to not suspended.\n";
    cout << "setpriority ---- Changes a user specified PCB's priority\n";
    cout << "showpcb -------- Displays all information about a specified PCB\n";
    cout << "showall -------- Displays some information about all PCBs\n";
    cout << "showready ------ Displays some information about all PCBs in the ready state\n";
    cout << "showblocked ---- Displays some information about all PCBs in the blocked state\n";
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

/**************************************************************************/
/**************************PART 2 FUNCTIONS********************************/
/**************************************************************************/

//if malloc doesn't return 0, return malloc, else return NULL
struct PCB* AllocatePCB(){
    struct PCB* temp = new PCB;
    if (temp != 0)
    {
        return temp;
    }
    else
    {
        return NULL;
    }
}

//if the pcb is null, return error, else free its memory
void FreePCB(struct PCB* inPCB){
    if (inPCB == NULL)
    {
        cout << "ERROR: Something went wrong when attempting to free memory." << endl;
    }
    else
    {
        free(inPCB);
    }
}

//initializes a PCB's content(name, priority,class)
struct PCB* SetupPCB(string name, int priority, bool classType)
{
    struct PCB* inPCB = AllocatePCB();
    inPCB->processName = name;
    inPCB->priority = priority;
    inPCB->processClass = classType;
    inPCB->stateOne = READY;
    inPCB->stateTwo = NOT_SUSPENDED;
    return inPCB;
}

//Search for a PCB with a given name, return a pointer to it if found or NULL if not found
struct PCB* FindPCB(string name, queue<struct PCB*> ready, queue<struct PCB*> blocked)
{
    //if both queues are empty return null
    if (ready.size() == 0 && blocked.size() == 0)
    {
        return NULL;
    }
    else
    {
        //if ready queue isn't empty, go through it and search for PCB with the given name
        if (ready.size() != 0)
        {
            struct PCB* temp = ready.front();
            for (unsigned int i = 0; i < ready.size(); i++)
            {
                if (temp->processName == name)
                {
                    return temp;
                }
                temp = temp->next;
            }
        }
        //if blocked queue isn't empty, go through it and search for PCB with the given name
        if (blocked.size() != 0)
        {
            struct PCB* temp = blocked.front();
            for (unsigned int i = 0; i < blocked.size(); i++)
            {
                if (temp->processName == name)
                {
                    return temp;
                }
                temp = temp->next;
            }
        }
    }
    //if the PCB isn't found in either queue, return NULL
    return NULL;
}

//Insert a PCB into its appropriate queue
void InsertPCB(struct PCB* inPCB, queue<struct PCB*> *ready, queue<struct PCB*> *blocked)
{
    //see if the PCB belongs in the ready queue
    if (inPCB->stateOne == READY)
    {
        //if the ready queue already has members, change the tail member to point to the new PCB and make the new PCB the tail
        if (ready->size() != 0)
        {
            struct PCB* temp = ready->back();
            temp->next = inPCB;
            inPCB->previous = ready->back();
            inPCB->next = NULL;
            ready->push(inPCB);
        }
        //if there are no members in the queue, point the new PCB to nothing and put it in the queue
        else
        {
            inPCB->previous = NULL;
            inPCB->next = NULL;
            ready->push(inPCB);
        }
    }
    //check to see if the PCB belongs in the blocked queue
    else if (inPCB->stateOne == BLOCKED)
    {
        //if the blocked queue already has members, change the tail member to point to the new PCB and make the new PCB the tail
        if (blocked->size() != 0)
        {
            struct PCB* temp = blocked->back();
            temp->next = inPCB;
            inPCB->previous = blocked->back();
            inPCB->next = NULL;
            blocked->push(inPCB);
        }
        //if there are no members in the queue, point the new PCB to nothing and put it in the queue
        else
        {
            inPCB->previous = NULL;
            inPCB->next = NULL;
            blocked->push(inPCB);
        }
    }
    //show an error since the PCB has an invalid state
    else
    {
        cout << "ERROR: PROCESS IS EITHER ALREADY RUNNING OR HAS AN INVALID STATE STATUS" << endl;
    }
}

//Remove a PCB from its queue
void RemovePCB(struct PCB* inPCB, queue<struct PCB*> *ready, queue<struct PCB*> *blocked)
{
    //check to see if the PCB exists
    if (FindPCB(inPCB->processName, *ready, *blocked) == NULL)
    {
        cout << "ERROR: COULD NOT FIND PCB" << endl;
    }
    else
    {
        //if ready queue isn't empty, go through it and search for the PCB to remove it
        if (ready->size() != 0)
        {
            struct PCB* temp = ready->front();
            for (unsigned int i = 0; i < ready->size(); i++)
            {
                //if temp's name = inPCB's name, then the PCB is in this queue
                if (temp->processName == inPCB->processName)
                {
                    //since the PCB is in this queue, go through and make a copy of the queue except for the PCB which we are removing
                    queue<struct PCB*> copy;
                    temp = ready->front();
                    for (unsigned int i = 0; i < ready->size(); i++)
                    {
                        //makes sure not to copy the PCB that is to be removed by comparing names
                        if (temp->processName != inPCB->processName)
                        {
                            copy.push(temp);
                        }
                        temp = temp->next;
                    }
                    //once the queue has been copied, empty the original queue
                    while (!ready->empty())
                    {
                        ready->pop();
                    }
                    //now empty the copy queue into the original queue
                    while (!copy.empty())
                    {
                        ready->push(copy.front());
                        copy.pop();
                    }
                    //since we didn't copy the PCB to be removed into the copy queue, we have now removed it from the original
                    return;
                }
                temp = temp->next;
            }
        }

        //if blocked queue isn't empty, go through it and search for the PCB to remove it
        if (blocked->size() != 0)
        {
            struct PCB* temp = blocked->front();
            for (unsigned int i = 0; i < blocked->size(); i++)
            {
                if (temp->processName == inPCB->processName)
                {
                    //since the PCB is in this queue, go through and make a copy of the queue except for the PCB which we are removing
                    queue<struct PCB*> copy;
                    temp = blocked->front();
                    for (unsigned int i = 0; i < blocked->size(); i++)
                    {
                        if (temp->processName != inPCB->processName)
                        {
                            copy.push(temp);
                        }
                        temp = temp->next;
                    }
                    //once the queue has been copied, empty the original queue
                    while (!blocked->empty())
                    {
                        blocked->pop();
                    }
                    //now empty the copy queue into the original queue
                    while (!copy.empty())
                    {
                        blocked->push(copy.front());
                        copy.pop();
                    }
                    //since we didn't copy the PCB to be removed into the copy queue, we have now removed it from the original
                    return;
                }
                temp = temp->next;
            }
        }
    }
}

string GetPCBname()
{
    string name;
    cout << "\nEnter the name of the PCB: ";
    cin >> name;
    transform(name.begin(), name.end(), name.begin(), ::tolower);

    return name;
}




































