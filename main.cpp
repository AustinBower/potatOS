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
#include <fstream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstddef>
#include <time.h>
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
struct PCB* FindPCB(string name, deque<struct PCB*> ready, deque<struct PCB*> blocked);
void InsertPCB(struct PCB* inPCB, deque<struct PCB*> *ready, deque<struct PCB*> *blocked);
void RemovePCB(struct PCB* inPCB, deque<struct PCB*> *ready, deque<struct PCB*> *blocked);
string GetPCBname();
void ShowPCB(deque<struct PCB*> ready, deque<struct PCB*> blocked);
void ShowAll(deque<struct PCB*> readyQueue, deque<struct PCB*> blockedQueue);
void ShowReady(deque<struct PCB *> ready);
void ShowBlocked(deque<struct PCB *> blocked);
//PART 3 FUNCTIONS
void ShortestJobFirst(deque<struct PCB*> *ready);
void FirstInFirstOut(deque<struct PCB*> *ready);
void ShortestTimeToCompletion(deque<struct PCB*> *ready);
void FixedPriority(deque<struct PCB*> *ready);
void RoundRobin(deque<struct PCB*> *ready);
void Multilevel(deque<struct PCB*> *ready);
void Lottery(deque<struct PCB*> *ready);

struct PCB{
    string processName;
    bool processClass;//application(true) or system(false) type
    int priority;//int from -127 to +128
    int stateOne;//0=running, 1=ready, 2=blocked
    int stateTwo;//0=not suspended, 1=suspended
    int memory;
    int timeRemaining;
    int timeOfArrival;
    int percentCPU;
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
    deque<struct PCB*> readyQueue;
    deque<struct PCB*> blockedQueue;

    //display a welcome message and the version number as well as set up the date variables and initialize them to the current date retrieved from the OS
    cout << "Welcome to potatOS!\n";
    DisplayVersion();
    int year;
    int month;
    int day;
    RestoreCurrentDate(year, month, day);

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
        /*USER COMMAND FOR ASSIGNMENT 2
         *
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
        */
        /*USER COMMAND FOR ASSIGNMENT 2
         *
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
        */
        /*USER COMMAND FOR ASSIGNMENT 2
         *
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
        */
        /*USER COMMAND FOR ASSIGNMENT 2
         *
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
        */
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
            ShowPCB(readyQueue, blockedQueue);
        }
        else if (in.compare("showall") == 0)
        {
            ShowAll(readyQueue, blockedQueue);
        }
        else if (in.compare("showready") == 0)
        {
            ShowReady(readyQueue);
        }
        else if (in.compare("showblocked") == 0)
        {
            ShowBlocked(blockedQueue);
        }
        else if (in.compare("sjf") == 0)
        {
            ShortestJobFirst(&readyQueue);
        }
        else if (in.compare("fifo") == 0)
        {
            FirstInFirstOut(&readyQueue);
        }
        else if (in.compare("stcf") == 0)
        {
            ShortestTimeToCompletion(&readyQueue);
        }
        else if (in.compare("fpps") == 0)
        {
            FixedPriority(&readyQueue);
        }
        else if (in.compare("rr") == 0)
        {
            RoundRobin(&readyQueue);
        }
        else if (in.compare("mlfq") == 0)
        {
            Multilevel(&readyQueue);
        }
        else if (in.compare("ls") == 0)
        {
            Lottery(&readyQueue);
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
//for this function to work properly you may need to go to the projects tab, then the build & run tab, then the run tab, and then set the working directory to the proper directory
//I am unsure as to whether or not the working directory is set automatically when first running this on a new machine
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
    cout << "\n--potatOS Version 0.3.0--\n\n";
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
    //cout << "createpcb ------ Allows the user to create a PCB.\n";
    //cout << "deletepcb ------ Deletes a PCB with a user specified name.\n";
    //cout << "block ---------- Changes a user specified PCB's state to blocked.\n";
    //cout << "unblock -------- Changes a user specified PCB's state to ready.\n";
    cout << "suspend -------- Changes a user specified PCB's state to suspended.\n";
    cout << "resume --------- Changes a user specified PCB's state to not suspended.\n";
    cout << "setpriority ---- Changes a user specified PCB's priority\n";
    cout << "showpcb -------- Displays all information about a specified PCB\n";
    cout << "showall -------- Displays some information about all PCBs\n";
    cout << "showready ------ Displays some information about all PCBs in the ready state\n";
    cout << "showblocked ---- Displays some information about all PCBs in the blocked state\n";
    cout << "help ----------- Displays a list of all valid user commands as well as their function.\n";
    cout << "sjf ------------ Performs the Shortest Job First scheduler.\n";
    cout << "fifo ----------- Performs the First In First Out scheduler.\n";
    cout << "stcf ----------- Performs the Shortest Time to Completion First scheduler.\n";
    cout << "fpps ----------- Performs the Fixed Priority Pre-Emptive Scheduling scheduler.\n";
    cout << "rr ------------- Performs the Round-Robin scheduler.\n";
    cout << "mlfq ----------- Performs the Multilevel Feecback Queue scheduler.\n";
    cout << "ls ------------- Performs the Lottery Scheduling scheduler.\n";
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
        delete inPCB;
    }
}

//initializes a PCB's content(name, priority,class)
struct PCB* SetupPCB(string name, int priority, bool classType, int memory, int timeRemaining, int timeArrival, int percentCPU)
{
    struct PCB* inPCB = AllocatePCB();
    inPCB->processName = name;
    inPCB->priority = priority;
    inPCB->processClass = classType;
    inPCB->memory = memory;
    inPCB->timeRemaining = timeRemaining;
    inPCB->timeOfArrival = timeArrival;
    inPCB->percentCPU = percentCPU;
    inPCB->stateOne = READY;
    inPCB->stateTwo = NOT_SUSPENDED;
    return inPCB;
}

//Search for a PCB with a given name, return a pointer to it if found or NULL if not found
struct PCB* FindPCB(string name, deque<struct PCB*> ready, deque<struct PCB*> blocked)
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
            for (unsigned int i = 0; i < ready.size(); i++)
            {
                struct PCB* temp = ready.at(i);
                if (temp->processName == name)
                {
                    return temp;
                }
            }
        }
        //if blocked queue isn't empty, go through it and search for PCB with the given name
        if (blocked.size() != 0)
        {
            for (unsigned int i = 0; i < blocked.size(); i++)
            {
                struct PCB* temp = blocked.at(i);
                if (temp->processName == name)
                {
                    return temp;
                }
            }
        }
    }
    //if the PCB isn't found in either queue, return NULL
    return NULL;
}

//Insert a PCB into its appropriate queue
void InsertPCB(struct PCB* inPCB, deque<struct PCB*> *ready, deque<struct PCB*> *blocked)
{
    //see if the PCB belongs in the ready queue
    if (inPCB->stateOne == READY)
    {
        ready->push_back(inPCB);
    }
    //check to see if the PCB belongs in the blocked queue
    else if (inPCB->stateOne == BLOCKED)
    {
        blocked->push_back(inPCB);
    }
    //show an error since the PCB has an invalid state
    else
    {
        cout << "ERROR: PROCESS IS EITHER ALREADY RUNNING OR HAS AN INVALID STATE STATUS" << endl;
    }
}

//Remove a PCB from its queue
void RemovePCB(struct PCB* inPCB, deque<struct PCB*> *ready, deque<struct PCB*> *blocked)
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
            for (unsigned int i = 0; i < ready->size(); i++)
            {
                //if temp's name = inPCB's name, then the PCB is in this queue
                struct PCB* temp = ready->at(i);
                if (temp->processName == inPCB->processName)
                {
                    ready->erase(ready->begin() + i);
                    return;
                }
            }
        }

        //if blocked queue isn't empty, go through it and search for the PCB to remove it
        if (blocked->size() != 0)
        {
            for (unsigned int i = 0; i < blocked->size(); i++)
            {
                struct PCB* temp = blocked->at(i);
                if (temp->processName == inPCB->processName)
                {
                    blocked->erase(ready->begin() + i);
                    return;
                }
            }
        }
    }
}

//get a string name for a pcb from the user
string GetPCBname()
{
    string name;
    cout << "\nEnter the name of the PCB: ";
    cin >> name;
    transform(name.begin(), name.end(), name.begin(), ::tolower);

    return name;
}

//show info for a pcb
void ShowPCB(deque<struct PCB*> ready, deque<struct PCB*> blocked)
{
    string name = GetPCBname();
    struct PCB* toShow = FindPCB(name, ready, blocked);
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

//show info from all pcbs
void ShowAll(deque<struct PCB*> readyQueue, deque<struct PCB*> blockedQueue)
{
    cout << "  ******* PCB INFORMATION *******" << endl;
    if (readyQueue.size() == 0 && blockedQueue.size() == 0)
    {
        cout << "*ERROR: There are currently no PCBs to show information*" << endl;
    }
    if (readyQueue.size() != 0)
    {
        for (unsigned int i = 0; i < readyQueue.size(); i++)
        {
            struct PCB* toShow = readyQueue.at(i);
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
        }
    }
    if (blockedQueue.size() != 0)
    {
        for (unsigned int i = 0; i < blockedQueue.size(); i++)
        {
            struct PCB* toShow = blockedQueue.at(i);
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
        }
    }
    cout << "  ***** END PCB INFORMATION *****" << endl;
}

//show all pcbs in readyqueue
void ShowReady(deque<struct PCB*> ready)
{
    cout <<"    *****READY QUEUE*****" << endl;
    if (ready.size() != 0)
    {
        for (unsigned int i = 0; i < ready.size(); i++)
        {
            struct PCB* toShow = ready.at(i);
            cout << "    Name -- " << toShow->processName << " ||";
            cout << "  Priority -- " << toShow->priority << " ||";
            cout << "  State One -- Ready" << " ||";
            if (toShow->stateTwo == SUSPENDED)
            {
                cout << "  State Two -- Suspended" << " ||";
            }
            else
            {
                cout << "  State Two -- Not Suspended" << " ||";
            }
            cout << "  Time Remaining -- " << toShow->timeRemaining << endl;
        }
        cout << endl;
    }
    else
    {
        cout << "*ERROR: There are no PCBs in the ready state*" << endl;
    }
}

//show all pcbs in blockedqueue
void ShowBlocked(deque<struct PCB *> blocked)
{
    cout <<"    *****BLOCKED QUEUE*****" << endl;
    if (blocked.size() != 0)
    {
        for (unsigned int i = 0; i < blocked.size(); i++)
        {
            struct PCB* toShow = blocked.at(i);
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
        }
        cout << endl;
    }
    else
    {
        cout << "*ERROR: There are no PCBs in the blocked state*" << endl;
    }
}

//
//ASSIGNMENT 3 FUNCTIONS
//

//perform the shortest job first scheduler
void ShortestJobFirst(deque<struct PCB*> *ready)
{
    ifstream inFile;
    string line = "";

    string fileName;
    cout << "Enter the data file name : ";
    cin >> fileName;

    //check to see if a file is open and if not, then oopen it and read in the info
    if (!inFile.is_open())
    {
        inFile.open(fileName.c_str());
        if (inFile.is_open())
        {
            //read in file line by line
            while (getline(inFile, line))
            {
                int startPos=0;
                int endPos=0;
                vector<string> words;
                //go through the line read in and store each word dilimeted by spaces into a string vecor
                for (unsigned int i = 0; i < line.size(); i++)
                {
                    char at = line.at(i);
                    if (at == ' ')
                    {
                        int size = endPos - startPos;
                        string word = line.substr(startPos, size);
                        words.push_back(word);
                        endPos++;
                        startPos = endPos;
                    }
                    else
                    {
                        endPos++;
                    }
                }
                //store the last value into the string vector
                int size = endPos - startPos;
                string word = line.substr(startPos, size);
                words.push_back(word);

                //change string values to proper types and setup the pcb
                string temp = words.at(1);
                bool classType;
                if (temp == "A")
                {
                    classType = APP;
                }
                else
                {
                    classType = SYS;
                }
                temp = words.at(2);
                int priority = atoi(temp.c_str());
                temp = words.at(3);
                int memory = atoi(temp.c_str());
                temp = words.at(4);
                int timeRemaining = atoi(temp.c_str());
                temp = words.at(5);
                int timeArrival = atoi(temp.c_str());
                temp = words.at(6);
                int percentCPU = atoi(temp.c_str());
                struct PCB* newPCB = SetupPCB(words.at(0), priority, classType, memory, timeRemaining, timeArrival, percentCPU);

                //check to see where the new PCB should be in the deque
                if (ready->size() == 0)
                {
                    ready->push_back(newPCB);
                }
                else
                {
                    bool inserted = false;
                    for (unsigned int i = 0; i < ready->size(); i++)
                    {
                        struct PCB* previous = ready->at(i);
                        if (newPCB->timeRemaining < previous->timeRemaining)
                        {
                            ready->insert(ready->begin() + i, newPCB);
                            inserted = true;
                            break;
                        }
                    }
                    if (inserted == false)
                    {
                        ready->push_back(newPCB);
                    }
                }
            }
            inFile.close();
        }
        else
        {
            cout << "*ERROR: Unable to open file*\n";
            return;
        }
    }
    else
    {
        cout << "*ERROR: File is already open*\n";
        return;
    }
    //show ready queue to display the scheduler is ordered properly and then execute processes
    ShowReady(*ready);
    deque<struct PCB*> completedProcesses;
    int completion = 0; //total time to completion
    float average = 0; //average turnaround time
    int numJobs = 0; //total number of jobs
    if(ready->size() != 0)
    {
        while(ready->size() != 0)
        {
            struct PCB* running = ready->front();
            ready->pop_front();
            completion = completion + running->timeRemaining;
            numJobs++;
            Sleep(running->timeRemaining * 1000); //parameter time is in milliseconds so i am multiplying by 1000 to wait for temp.timeremaining seconds
            completedProcesses.push_back(running);
            cout << "  ** " << running->processName << " completed **" << endl;
        }
    }
    //display order of execution, total time to completion, and avg turnaround
    cout << "\n  *****Process Execution Complete*****" << endl;
    cout << "  Order of Execution:" << endl;
    for(unsigned int i = 0; i < completedProcesses.size(); i++)
    {
        struct PCB* temp = completedProcesses.at(i);
        cout << "    " << temp->processName << endl;
    }
    average = (float)completion / (float)numJobs;
    cout << "  Total Time to Completion -- " << completion << " seconds" << endl;
    cout << "  Average Turnaround Time --- " << average << " seconds" << endl;
}

//fifo scheduler
void FirstInFirstOut(deque<struct PCB*> *ready)
{
    //open the input and output files
    ifstream inFile;
    string line = "";
    ofstream writeFile("C:/Users/Sam/Desktop/Github/potatOS/fifo.txt");

    string fileName;
    cout << "Enter the data file name : ";
    cin >> fileName;

    //check to see if a file is open and if not, then oopen it and read in the info
    if (!inFile.is_open())
    {
        inFile.open(fileName.c_str());
        if (inFile.is_open())
        {
            //read in file line by line
            while (getline(inFile, line))
            {
                int startPos=0;
                int endPos=0;
                vector<string> words;
                //go through the line read in and store each word dilimeted by spaces into a string vecor
                for (unsigned int i = 0; i < line.size(); i++)
                {
                    char at = line.at(i);
                    if (at == ' ')
                    {
                        int size = endPos - startPos;
                        string word = line.substr(startPos, size);
                        words.push_back(word);
                        endPos++;
                        startPos = endPos;
                    }
                    else
                    {
                        endPos++;
                    }
                }
                //store the last value into the string vector
                int size = endPos - startPos;
                string word = line.substr(startPos, size);
                words.push_back(word);

                //change string values to proper types and setup the pcb
                string temp = words.at(1);
                bool classType;
                if (temp == "A")
                {
                    classType = APP;
                }
                else
                {
                    classType = SYS;
                }
                temp = words.at(2);
                int priority = atoi(temp.c_str());
                temp = words.at(3);
                int memory = atoi(temp.c_str());
                temp = words.at(4);
                int timeRemaining = atoi(temp.c_str());
                temp = words.at(5);
                int timeArrival = atoi(temp.c_str());
                temp = words.at(6);
                int percentCPU = atoi(temp.c_str());
                struct PCB* newPCB = SetupPCB(words.at(0), priority, classType, memory, timeRemaining, timeArrival, percentCPU);
                writeFile << newPCB << " has entered into the system" << endl;
                //check to see where the new PCB should be in the deque
                if (ready->size() == 0)
                {
                    ready->push_back(newPCB);
                    writeFile << newPCB->processName << " entered into the ready queue" << endl;
                }
                else
                {
                    bool inserted = false;
                    for (unsigned int i = 0; i < ready->size(); i++)
                    {
                        struct PCB* previous = ready->at(i);
                        if (newPCB->timeOfArrival < previous->timeOfArrival)
                        {
                            ready->insert(ready->begin() + i, newPCB);
                            writeFile << newPCB->processName << " entered into the ready queue" << endl;
                            inserted = true;
                            break;
                        }
                    }
                    if (inserted == false)
                    {
                        ready->push_back(newPCB);
                        writeFile << newPCB->processName << " entered into the ready queue" << endl;
                    }
                }
            }
            //show the ready queue as processes are added to it
            //ShowReady(*ready);
            inFile.close();
        }
        else
        {
            cout << "*ERROR: Unable to open file*\n";
            return;
        }
    }
    else
    {
        cout << "*ERROR: File is already open*\n";
        return;
    }
    //show ready queue to display the scheduler is ordered properly and then execute processes
    ShowReady(*ready);
    deque<struct PCB*> completedProcesses;
    int completion = 0; //total time to completion
    float average = 0; //average turnaround time
    int numJobs = 0; //total number of jobs
    if(ready->size() != 0)
    {
        while(ready->size() != 0)
        {
            struct PCB* running = ready->front();
            writeFile << running->processName << " is now running" << endl;
            ready->pop_front();
            completion = completion + running->timeRemaining;
            numJobs++;
            Sleep(running->timeRemaining * 1000); //parameter time is in milliseconds so i am multiplying by 1000 to wait for temp.timeremaining seconds
            completedProcesses.push_back(running);
            cout << "  ** " << running->processName << " completed **" << endl;
            writeFile << running->processName << " completed execution" << endl;
        }
    }
    //display order of execution, total time to completion, and avg turnaround
    cout << "\n  *****Process Execution Complete*****" << endl;
    cout << "  Order of Execution:" << endl;
    for(unsigned int i = 0; i < completedProcesses.size(); i++)
    {
        struct PCB* temp = completedProcesses.at(i);
        cout << "    " << temp->processName << endl;
    }
    average = (float)completion / (float)numJobs;
    cout << "  Total Time to Completion -- " << completion << " seconds" << endl;
    cout << "  Average Turnaround Time --- " << average << " seconds" << endl;
    writeFile << "\n**EXECUTION COMPLETE**\n";
    writeFile.close();
}

//stcf scheduler
void ShortestTimeToCompletion(deque<struct PCB*> *ready)
{
    //open the input and output files
    ifstream inFile;
    string line = "";
    ofstream writeFile("C:/Users/Sam/Desktop/Github/potatOS/stcf.txt");
    struct PCB* running;
    struct PCB* previous;//previously inserted PCB, used to tell time of arrival differential
    //used for displaying/calculating execution information
    deque<struct PCB*> completedProcesses;
    int completion = 0; //total time to completion
    float average = 0; //average turnaround time
    int numJobs = 0; //total number of jobs

    string fileName;
    cout << "Enter the data file name : ";
    cin >> fileName;

    //check to see if a file is open and if not, then oopen it and read in the info
    if (!inFile.is_open())
    {
        inFile.open(fileName.c_str());
        if (inFile.is_open())
        {
            //read in file line by line
            while (getline(inFile, line))
            {
                int startPos=0;
                int endPos=0;
                vector<string> words;
                //go through the line read in and store each word dilimeted by spaces into a string vecor
                for (unsigned int i = 0; i < line.size(); i++)
                {
                    char at = line.at(i);
                    if (at == ' ')
                    {
                        int size = endPos - startPos;
                        string word = line.substr(startPos, size);
                        words.push_back(word);
                        endPos++;
                        startPos = endPos;
                    }
                    else
                    {
                        endPos++;
                    }
                }
                //store the last value into the string vector
                int size = endPos - startPos;
                string word = line.substr(startPos, size);
                words.push_back(word);

                //change string values to proper types and setup the pcb
                string temp = words.at(1);
                bool classType;
                if (temp == "A")
                {
                    classType = APP;
                }
                else
                {
                    classType = SYS;
                }
                temp = words.at(2);
                int priority = atoi(temp.c_str());
                temp = words.at(3);
                int memory = atoi(temp.c_str());
                temp = words.at(4);
                int timeRemaining = atoi(temp.c_str());
                temp = words.at(5);
                int timeArrival = atoi(temp.c_str());
                temp = words.at(6);
                int percentCPU = atoi(temp.c_str());
                struct PCB* newPCB = SetupPCB(words.at(0), priority, classType, memory, timeRemaining, timeArrival, percentCPU);
                writeFile << newPCB << " has entered into the system" << endl;
                //check to see where the new PCB should be in the deque
                if (ready->size() == 0)
                {
                    ready->push_back(newPCB);
                    writeFile << newPCB->processName << " entered into the ready queue" << endl;
                }
                else
                {
                    //set up variable to calculate wait time (i.e. time between arrival of newest process and the previous process) and wait that time, remove that time from the remaining time of the running process
                    int difference = newPCB->timeOfArrival - previous->timeOfArrival; //used to tell how long a process should run before it should be checked if it needs to be running
                    running = ready->at(0);
                    ready->pop_front();
                    writeFile << "A new process has entered the system -- " << running->processName << " is running" << endl;
                    Sleep(difference * 1000);
                    completion = completion + difference;
                    running->timeRemaining = running->timeRemaining - difference;
                    //if the running proccess completed while waiting for new processess, complete execution of it
                    if (running->timeRemaining <= 0)
                    {
                        writeFile << running->processName << " has finished executing" << endl;
                        completedProcesses.push_back(running);
                        numJobs++;
                    }
                    //if the new process has less time remaining than the running process, interrupt running
                    if (newPCB->timeRemaining < running->timeRemaining)
                    {
                        //if running process still has time remaining, determine where it should be inserted
                        bool inserted = false;
                        for (unsigned int i = 0; i < ready->size(); i++)
                        {
                            struct PCB* position = ready->at(i);
                            if (running->timeRemaining < position->timeRemaining)
                            {
                                ready->insert(ready->begin() + i, running);
                                writeFile << running->processName << " entered into the ready queue" << endl;
                                inserted = true;
                                break;
                            }
                        }
                        if (inserted == false)
                        {
                            ready->push_back(running);
                            writeFile << running->processName << " entered into the ready queue" << endl;
                        }
                        ready->insert(ready->begin(), newPCB);
                    }
                    //since the new process shouldn't be running yet, place it properly in the queue
                    else
                    {
                        ready->insert(ready->begin(), running);
                        bool inserted = false;
                        for (unsigned int i = 0; i < ready->size(); i++)
                        {
                            struct PCB* position = ready->at(i);
                            if (newPCB->timeRemaining < position->timeRemaining)
                            {
                                ready->insert(ready->begin() + i, newPCB);
                                writeFile << running->processName << " entered into the ready queue" << endl;
                                inserted = true;
                                break;
                            }
                        }
                        if (inserted == false)
                        {
                            ready->push_back(newPCB);
                            writeFile << newPCB->processName << " entered into the ready queue" << endl;
                        }
                    }
                }
                //show the ready queue as processes are added to it
                //ShowReady(*ready);
                previous = newPCB;
                //The following function call can be uncommented to show the ready queue as it builds with new processess
                //*note* this function replaces the running process back into the readyqueue at the end of the loop and removes it at the beginning so the show ready call will show the running process as the first ready process
                //ShowReady(*ready);
            }
            inFile.close();
        }
        else
        {
            cout << "*ERROR: Unable to open file*\n";
            return;
        }
    }
    else
    {
        cout << "*ERROR: File is already open*\n";
        return;
    }
    //as long as we still have processes ready to execute, go through with execution
    if(ready->size() != 0)
    {
        //output information and update metrics calcuation variables and remove executed process from ready queue
        while(ready->size() != 0)
        {
            struct PCB* running = ready->front();
            writeFile << running->processName << " is running" << endl;
            ready->pop_front();
            completion = completion + running->timeRemaining;
            numJobs++;
            Sleep(running->timeRemaining * 1000); //parameter time is in milliseconds so i am multiplying by 1000 to wait for temp.timeremaining seconds
            completedProcesses.push_back(running);
            cout << "  ** " << running->processName << " completed **" << endl;
            writeFile << running->processName << " completed execution" << endl;
        }
    }
    //display order of execution, total time to completion, and avg turnaround
    cout << "\n  *****Process Execution Complete*****" << endl;
    cout << "  Order of Execution:" << endl;
    for(unsigned int i = 0; i < completedProcesses.size(); i++)
    {
        struct PCB* temp = completedProcesses.at(i);
        cout << "    " << temp->processName << endl;
    }
    average = (float)completion / (float)numJobs;
    cout << "  Total Time to Completion -- " << completion << " seconds" << endl;
    cout << "  Average Turnaround Time --- " << average << " seconds" << endl;
    writeFile << "\n**EXECUTION COMPLETE**\n";
    writeFile.close();
}

//fixed priority pre-emptive scheduling
void FixedPriority(deque<struct PCB*> *ready)
{
    //open the input and output files
    ifstream inFile;
    string line = "";
    ofstream writeFile("C:/Users/Sam/Desktop/Github/potatOS/fpps.txt");
    struct PCB* running;
    struct PCB* previous;//previously inserted PCB, used to tell time of arrival differential
    //used for displaying/calculating execution information
    deque<struct PCB*> completedProcesses;
    int completion = 0; //total time to completion
    float average = 0; //average turnaround time
    int numJobs = 0; //total number of jobs

    string fileName;
    cout << "Enter the data file name : ";
    cin >> fileName;

    //check to see if a file is open and if not, then oopen it and read in the info
    if (!inFile.is_open())
    {
        inFile.open(fileName.c_str());
        if (inFile.is_open())
        {
            //read in file line by line
            while (getline(inFile, line))
            {
                int startPos=0;
                int endPos=0;
                vector<string> words;
                //go through the line read in and store each word dilimeted by spaces into a string vecor
                for (unsigned int i = 0; i < line.size(); i++)
                {
                    char at = line.at(i);
                    if (at == ' ')
                    {
                        int size = endPos - startPos;
                        string word = line.substr(startPos, size);
                        words.push_back(word);
                        endPos++;
                        startPos = endPos;
                    }
                    else
                    {
                        endPos++;
                    }
                }
                //store the last value into the string vector
                int size = endPos - startPos;
                string word = line.substr(startPos, size);
                words.push_back(word);

                //change string values to proper types and setup the pcb
                string temp = words.at(1);
                bool classType;
                if (temp == "A")
                {
                    classType = APP;
                }
                else
                {
                    classType = SYS;
                }
                temp = words.at(2);
                int priority = atoi(temp.c_str());
                temp = words.at(3);
                int memory = atoi(temp.c_str());
                temp = words.at(4);
                int timeRemaining = atoi(temp.c_str());
                temp = words.at(5);
                int timeArrival = atoi(temp.c_str());
                temp = words.at(6);
                int percentCPU = atoi(temp.c_str());
                struct PCB* newPCB = SetupPCB(words.at(0), priority, classType, memory, timeRemaining, timeArrival, percentCPU);
                writeFile << newPCB << " has entered into the system" << endl;
                //check to see where the new PCB should be in the deque
                if (ready->size() == 0)
                {
                    ready->push_back(newPCB);
                    writeFile << newPCB->processName << " entered into the ready queue" << endl;
                }
                else
                {
                    //set up variable to calculate wait time (i.e. time between arrival of newest process and the previous process) and wait that time, remove that time from the remaining time of the running process
                    int difference = newPCB->timeOfArrival - previous->timeOfArrival; //used to tell how long a process should run before it should be checked if it needs to be running
                    running = ready->at(0);
                    ready->pop_front();
                    writeFile << "A new process has entered the system -- " << running->processName << " is running" << endl;
                    Sleep(difference * 1000);
                    completion = completion + difference;
                    running->timeRemaining = running->timeRemaining - difference;
                    //if the running proccess completed while waiting for new processess, complete execution of it
                    if (running->timeRemaining <= 0)
                    {
                        writeFile << running->processName << " has finished executing" << endl;
                        completedProcesses.push_back(running);
                        numJobs++;
                    }
                    //if the new process has higher priority than the running process, interrupt running
                    if (newPCB->priority > running->priority)
                    {
                        //if running process still has time remaining, determine where it should be inserted
                        bool inserted = false;
                        for (unsigned int i = 0; i < ready->size(); i++)
                        {
                            struct PCB* position = ready->at(i);
                            if (running->priority > position->priority)
                            {
                                ready->insert(ready->begin() + i, running);
                                writeFile << running->processName << " entered into the ready queue" << endl;
                                inserted = true;
                                break;
                            }
                        }
                        if (inserted == false)
                        {
                            ready->push_back(running);
                            writeFile << running->processName << " entered into the ready queue" << endl;
                        }
                        ready->insert(ready->begin(), newPCB);
                    }
                    //since the new process shouldn't be running yet, place it properly in the queue
                    else
                    {
                        ready->insert(ready->begin(), running);
                        bool inserted = false;
                        for (unsigned int i = 0; i < ready->size(); i++)
                        {
                            struct PCB* position = ready->at(i);
                            if (newPCB->priority > position->priority)
                            {
                                ready->insert(ready->begin() + i, newPCB);
                                writeFile << running->processName << " entered into the ready queue" << endl;
                                inserted = true;
                                break;
                            }
                        }
                        if (inserted == false)
                        {
                            ready->push_back(newPCB);
                            writeFile << newPCB->processName << " entered into the ready queue" << endl;
                        }
                    }
                }
                //show the ready queue as processes are added to it
                //ShowReady(*ready);
                previous = newPCB;
                //The following function call can be uncommented to show the ready queue as it builds with new processess
                //*note* this function replaces the running process back into the readyqueue at the end of the loop and removes it at the beginning so the show ready call will show the running process as the first ready process
                //ShowReady(*ready);
            }
            inFile.close();
        }
        else
        {
            cout << "*ERROR: Unable to open file*\n";
            return;
        }
    }
    else
    {
        cout << "*ERROR: File is already open*\n";
        return;
    }
    //as long as we still have processes ready to execute, go through with execution
    if(ready->size() != 0)
    {
        //output information and update metrics calcuation variables and remove executed process from ready queue
        while(ready->size() != 0)
        {
            struct PCB* running = ready->front();
            writeFile << running->processName << " is running" << endl;
            ready->pop_front();
            completion = completion + running->timeRemaining;
            numJobs++;
            Sleep(running->timeRemaining * 1000); //parameter time is in milliseconds so i am multiplying by 1000 to wait for temp.timeremaining seconds
            completedProcesses.push_back(running);
            cout << "  ** " << running->processName << " completed **" << endl;
            writeFile << running->processName << " completed execution" << endl;
        }
    }
    //display order of execution, total time to completion, and avg turnaround
    cout << "\n  *****Process Execution Complete*****" << endl;
    cout << "  Order of Execution:" << endl;
    for(unsigned int i = 0; i < completedProcesses.size(); i++)
    {
        struct PCB* temp = completedProcesses.at(i);
        cout << "    " << temp->processName << endl;
    }
    average = (float)completion / (float)numJobs;
    cout << "  Total Time to Completion -- " << completion << " seconds" << endl;
    cout << "  Average Turnaround Time --- " << average << " seconds" << endl;
    writeFile << "\n**EXECUTION COMPLETE**\n";
    writeFile.close();
}

//round robin scheduler
void RoundRobin(deque<struct PCB*> *ready)
{
    //open the input and output files
    ifstream inFile;
    string line = "";
    ofstream writeFile("C:/Users/Sam/Desktop/Github/potatOS/robin.txt");
    struct PCB* running;
    struct PCB* previous;//previously inserted PCB, used to tell time of arrival differential
    //used for displaying/calculating execution information
    deque<struct PCB*> completedProcesses;
    int completion = 0; //total time to completion
    float average = 0; //average turnaround time
    int numJobs = 0; //total number of jobs
    int timeSlice = 0;//length of the time slice
    int currentSlice = 0;//time remaining in currentSlice

    string fileName;
    cout << "Enter the data file name : ";
    cin >> fileName;

    //check to see if a file is open and if not, then oopen it and read in the info
    if (!inFile.is_open())
    {
        inFile.open(fileName.c_str());
        if (inFile.is_open())
        {
            //infinite loop to get an integer number for the time quantum
            while (1)
            {
                //for the sake of not allowing the user to enter some ridiculous time quantum and for simplicity's sake this program will only accept an
                //integer from 1 to 10 as the time slice, however this could easily be tweaked to accept bigger numbers
                cout << "Enter an integer number for the Round Robin time quantum/slice \nThe number entered will be the duration, in seconds, of the slice (number must be in the range of 1 to 10): ";
                cin >> timeSlice;
                if (timeSlice < 1 || timeSlice > 10)
                {
                    cout << "*ERROR: Input number invalid, try again" << endl;
                }
                else
                {
                    cout << "Time slice set to " << timeSlice << " seconds" << endl;
                    currentSlice = timeSlice;
                    break;
                }
            }
            //read in file line by line
            while (getline(inFile, line))
            {
                int startPos=0;
                int endPos=0;
                vector<string> words;
                //go through the line read in and store each word dilimeted by spaces into a string vecor
                for (unsigned int i = 0; i < line.size(); i++)
                {
                    char at = line.at(i);
                    if (at == ' ')
                    {
                        int size = endPos - startPos;
                        string word = line.substr(startPos, size);
                        words.push_back(word);
                        endPos++;
                        startPos = endPos;
                    }
                    else
                    {
                        endPos++;
                    }
                }
                //store the last value into the string vector
                int size = endPos - startPos;
                string word = line.substr(startPos, size);
                words.push_back(word);

                //change string values to proper types and setup the pcb
                string temp = words.at(1);
                bool classType;
                if (temp == "A")
                {
                    classType = APP;
                }
                else
                {
                    classType = SYS;
                }
                temp = words.at(2);
                int priority = atoi(temp.c_str());
                temp = words.at(3);
                int memory = atoi(temp.c_str());
                temp = words.at(4);
                int timeRemaining = atoi(temp.c_str());
                temp = words.at(5);
                int timeArrival = atoi(temp.c_str());
                temp = words.at(6);
                int percentCPU = atoi(temp.c_str());
                struct PCB* newPCB = SetupPCB(words.at(0), priority, classType, memory, timeRemaining, timeArrival, percentCPU);
                writeFile << newPCB << " has entered into the system" << endl;
                //check to see where the new PCB should be in the deque
                if (ready->size() == 0)
                {
                    ready->push_back(newPCB);
                    writeFile << newPCB->processName << " entered into the ready queue" << endl;
                }
                else
                {
                    //set up variable to calculate wait time (i.e. time between arrival of newest process and the previous process) and wait that time, remove that time from the remaining time of the running process
                    int difference = newPCB->timeOfArrival - previous->timeOfArrival; //used to tell how long a process should run before it should be checked if it needs to be running
                    running = ready->front();
                    ready->pop_front();
                    writeFile << "A new process has entered the system -- " << running->processName << " is running" << endl;
                    //if there has been a time slice complete since the new process entered the system, calculate accordingly
                    if (currentSlice - difference < 0)
                    {
                        //sleep for the remainder of the current slice and adjust variables
                        Sleep(currentSlice * 1000);
                        completion = completion + currentSlice;
                        running->timeRemaining = running->timeRemaining - timeSlice;
                        //check to see if the running process will complete or not
                        if (running->timeRemaining <= 0)
                        {
                            writeFile << running->processName << " has finished executing" << endl;
                            completedProcesses.push_back(running);
                            numJobs++;
                        }
                        else
                        {
                            ready->push_back(running);
                        }
                        int slicesElapsed = (difference - currentSlice) / timeSlice;//see how many time slices have passed
                        //if there are more than one slices to account for, do so
                        if (slicesElapsed > 1)
                        {
                            for (unsigned int i = 1; i < slicesElapsed; i++)
                            {
                                running = ready->front();
                                ready->pop_front();
                                Sleep(timeSlice * 1000);
                                completion = completion + timeSlice;
                                running->timeRemaining = running->timeRemaining - timeSlice;
                                if (running->timeRemaining <= 0)
                                {
                                    writeFile << running->processName << " has finished executing" << endl;
                                    completedProcesses.push_back(running);
                                    numJobs++;
                                }
                                else
                                {
                                    ready->push_back(running);
                                }
                            }
                        }
                        //account for the extra time slice
                        else
                        {
                            running = ready->front();
                            ready->pop_front();
                            Sleep(timeSlice * 1000);
                            completion = completion + timeSlice;
                            running->timeRemaining = running->timeRemaining - timeSlice;
                            if (running->timeRemaining <= 0)
                            {
                                writeFile << running->processName << " has finished executing" << endl;
                                completedProcesses.push_back(running);
                                numJobs++;
                            }
                            else
                            {
                                ready->push_back(running);
                            }
                        }
                        currentSlice = (difference - currentSlice) % timeSlice;
                    }
                    //else a time quantum has not passed since new process entered system
                    else
                    {
                        currentSlice = currentSlice - difference;
                        Sleep(difference * 1000);
                        completion = completion + difference;
                        running->timeRemaining = running->timeRemaining - difference;
                        //if the running proccess completed while waiting for new processess, complete execution of it
                        if (running->timeRemaining <= 0)
                        {
                            writeFile << running->processName << " has finished executing" << endl;
                            completedProcesses.push_back(running);
                            numJobs++;
                        }
                        else
                        {
                            ready->push_front(running);
                        }
                    }
                    ready->push_back(newPCB);
                }
                //show the ready queue as processes are added to it
                //ShowReady(*ready);
                previous = newPCB;
                //*note* this function replaces the running process back into the readyqueue at the end of the loop and removes it at the beginning so the show ready call will show the running process as the first ready process
            }
            inFile.close();
        }
        else
        {
            cout << "*ERROR: Unable to open file*\n";
            return;
        }
    }
    else
    {
        cout << "*ERROR: File is already open*\n";
        return;
    }
    //as long as we still have processes ready to execute, go through with execution
    if(ready->size() != 0)
    {
        //complete the currentSlice if it still has time
        if (currentSlice > 0)
        {
            struct PCB* running = ready->front();
            writeFile << running->processName << " is running" << endl;
            ready->pop_front();
            Sleep(currentSlice * 1000);
            running->timeRemaining = running->timeRemaining - currentSlice;
            completion = completion + currentSlice;
            //check to see if the running process has completed, if so increment numJobs variable and display completion info, if not push back into deque
            if (running->timeRemaining <= 0 )
            {
                numJobs++;
                cout << "  ** " << running->processName << " completed **" << endl;
                writeFile << running->processName << " completed execution" << endl;
            }
            else
            {
                ready->push_back(running);
            }
        }
        //finish executing the rest of the processes
        while(ready->size() != 0)
        {
            struct PCB* running = ready->front();
            writeFile << running->processName << " is running" << endl;
            ready->pop_front();
            Sleep(timeSlice * 1000);
            running->timeRemaining = running->timeRemaining - timeSlice;
            completion = completion + timeSlice;
            //check to see if the running process has completed, if so increment numJobs variable and display completion info, if not push back into deque
            if (running->timeRemaining <= 0 )
            {
                numJobs++;
                cout << "  ** " << running->processName << " completed **" << endl;
                writeFile << running->processName << " completed execution" << endl;
            }
            else
            {
                ready->push_back(running);
            }
        }
    }
    //display order of execution, total time to completion, and avg turnaround
    cout << "\n  *****Process Execution Complete*****" << endl;
    cout << "  Order of Execution:" << endl;
    for(unsigned int i = 0; i < completedProcesses.size(); i++)
    {
        struct PCB* temp = completedProcesses.at(i);
        cout << "    " << temp->processName << endl;
    }
    average = (float)completion / (float)numJobs;
    cout << "  Total Time to Completion -- " << completion << " seconds" << endl;
    cout << "  Average Turnaround Time --- " << average << " seconds" << endl;
    writeFile << "\n**EXECUTION COMPLETE**\n";
    writeFile.close();
}

//multilevel feeback queue shcheduler
//*NOTE* I couldn't seem to get this scheduler to work properly while processes were being read in so I simply read in all processes at once (i.e. each process will arrive with negligible time of arrival difference)
void Multilevel(deque<struct PCB*> *ready)
{
    //open the input and output files
    ifstream inFile;
    string line = "";
    ofstream writeFile("C:/Users/Sam/Desktop/Github/potatOS/mlfq.txt");
    struct PCB* running;
    struct PCB* previous;//previously inserted PCB, used to tell time of arrival differential
    //used for displaying/calculating execution information
    deque<struct PCB*> completedProcesses;
    int completion = 0; //total time to completion
    float average = 0; //average turnaround time
    int numJobs = 0; //total number of jobs
    vector<int> timeSlice;
    int numQueues = 0;
    int numCycles = 0;
    int cyclesPassed = 0;

    string fileName;
    cout << "Enter the data file name : ";
    cin >> fileName;

    //check to see if a file is open and if not, then oopen it and read in the info
    if (!inFile.is_open())
    {
        inFile.open(fileName.c_str());
        if (inFile.is_open())
        {
            //get the number of 'queues' the user wants to use
            while (1)
            {
                //for the sake of not allowing the user to enter some ridiculous amount of queues and for simplicity's sake this program will only accept an
                //integer from 1 to 10 as the number of queues, however this could easily be tweaked to accept bigger numbers
                cout << "**Enter an integer number for the number of priority levels/queues \nThe number entered will be the number of priority levels (number must be in the range of 1 to 10): ";
                cin >> numQueues;
                if (numQueues < 1 || numQueues > 10)
                {
                    cout << "*ERROR: Input number invalid, try again" << endl;
                }
                else
                {
                    cout << "  Number of queues set to " << numQueues << "\n\n";
                    break;
                }
            }
            //infinite loop to get an integer number for the time quantum
            while (1)
            {
                for (unsigned int i = 0; i < numQueues; i++)
                {
                    //for the sake of not allowing the user to enter some ridiculous time quantum and for simplicity's sake this program will only accept an
                    //integer from 1 to 10 as the time slice, however this could easily be tweaked to accept bigger numbers
                    int currentNum = 0;
                    cout << "**Enter an integer number for queue " << i + 1 << "\nThe number entered will be the duration, in seconds, of the slice (number must be in the range of 1 to 10): ";
                    cin >> currentNum;
                    if (currentNum < 1 || currentNum > 10)
                    {
                        cout << "*ERROR: Input number invalid, try again" << endl;
                    }
                    else
                    {
                        cout << "  Time slice set to " << currentNum << " seconds" << "\n\n";
                        timeSlice.push_back(currentNum);
                    }
                }
                break;
            }
            //get the number of cycles to pass before restoring all processes to highest priority
            while (1)
            {
                //for the sake of not allowing the user to enter some ridiculous amount of cycles and for simplicity's sake this program will only accept an
                //integer from 2 to 20 as the number of cycles, however this could easily be tweaked to accept bigger numbers
                cout << "**Enter an integer number for the number of cycles to pass \nThe number entered will be the number of cycles passed before restoring all processes to the highest priority (number must be in the range of 2 to 10): ";
                cin >> numCycles;
                if (numCycles < 2 || numCycles > 20)
                {
                    cout << "*ERROR: Input number invalid, try again" << endl;
                }
                else
                {
                    cout << "  Number of cycles set to " << numCycles << "\n\n";
                    break;
                }
            }

            //read in file line by line
            while (getline(inFile, line))
            {
                int startPos=0;
                int endPos=0;
                vector<string> words;
                //go through the line read in and store each word dilimeted by spaces into a string vecor
                for (unsigned int i = 0; i < line.size(); i++)
                {
                    char at = line.at(i);
                    if (at == ' ')
                    {
                        int size = endPos - startPos;
                        string word = line.substr(startPos, size);
                        words.push_back(word);
                        endPos++;
                        startPos = endPos;
                    }
                    else
                    {
                        endPos++;
                    }
                }
                //store the last value into the string vector
                int size = endPos - startPos;
                string word = line.substr(startPos, size);
                words.push_back(word);

                //change string values to proper types and setup the pcb
                string temp = words.at(1);
                bool classType;
                if (temp == "A")
                {
                    classType = APP;
                }
                else
                {
                    classType = SYS;
                }
                int priority = 0;//this sets each new process to the highest priority level (0 being highest priority with priority decreasing as a process's priority value approaches the numQueues given)
                temp = words.at(3);
                int memory = atoi(temp.c_str());
                temp = words.at(4);
                int timeRemaining = atoi(temp.c_str());
                temp = words.at(5);
                int timeArrival = atoi(temp.c_str());
                temp = words.at(6);
                int percentCPU = atoi(temp.c_str());
                struct PCB* newPCB = SetupPCB(words.at(0), priority, classType, memory, timeRemaining, timeArrival, percentCPU);
                writeFile << newPCB->processName << " has entered into the system" << endl;
                ready->push_back(newPCB);
            }
            inFile.close();
        }
        else
        {
            cout << "*ERROR: Unable to open file*\n";
            return;
        }
    }
    else
    {
        cout << "*ERROR: File is already open*\n";
        return;
    }
    //as long as we still have processes ready to execute, go through with execution
    if(ready->size() != 0)
    {
        //finish executing the rest of the processes
        while(ready->size() != 0)
        {
            struct PCB* running = ready->front();
            writeFile << running->processName << " is running with a current priority of " << running->priority + 1 << endl;
            int currentSlice = timeSlice.at(running->priority);
            ready->pop_front();
            Sleep(currentSlice * 1000);
            running->timeRemaining = running->timeRemaining - currentSlice;
            if (running->priority < numQueues - 1)
            {
                running->priority = running->priority + 1;
            }
            completion = completion + currentSlice;
            //check to see if the running process has completed, if so increment numJobs variable and display completion info, if not push back into deque
            if (running->timeRemaining <= 0 )
            {
                numJobs++;
                cout << "  ** " << running->processName << " completed **" << endl;
                writeFile << running->processName << " completed execution" << endl;
                completedProcesses.push_back(running);
            }
            //since it still needs to run, find out where it should be placed in the ready queue based on priority
            else
            {
                bool inserted = false;
                for (unsigned int i = 0; i < ready->size(); i++)
                {
                    struct PCB* currentReady = ready->at(i);
                    //if running's priority is less than (which means it has a higher priority by my rules) the currently accessed process's priority
                    //then insert the running priority back into the queue there
                    if (running->priority < currentReady->priority)
                    {
                        ready->insert(ready->begin() + i, running);
                        inserted = true;
                    }
                }
                //if the previous loop didn't find a process with worse priority than the running and it wasn't inserted, then put it at the end of the queue
                if (inserted == false)
                {
                    ready->push_back(running);
                }
            }
            cyclesPassed++;
            if (cyclesPassed == numCycles)
            {
                cyclesPassed = 0;
                writeFile << "Priorities for all processes reset to highest value" << endl;
                for (unsigned int i = 0; i < ready->size(); i++)
                {
                    struct PCB* currentReady = ready->at(i);
                    currentReady->priority = 0;
                }
            }
            //can be used to see the queue as it is altered and processes are run
            //ShowReady(*ready);
        }
    }
    //display order of execution, total time to completion, and avg turnaround
    cout << "\n  *****Process Execution Complete*****" << endl;
    cout << "  Order of Execution:" << endl;
    for(unsigned int i = 0; i < completedProcesses.size(); i++)
    {
        struct PCB* temp = completedProcesses.at(i);
        cout << "    " << temp->processName << endl;
    }
    average = (float)completion / (float)numJobs;
    cout << "  Total Time to Completion -- " << completion << " seconds" << endl;
    cout << "  Average Turnaround Time --- " << average << " seconds" << endl;
    writeFile << "\n**EXECUTION COMPLETE**\n";
    writeFile.close();
}

//lottery shcheduler
//*NOTE* I couldn't seem to get this scheduler to work properly while processes were being read in so I simply read in all processes at once (i.e. each process will arrive with negligible time of arrival difference)
void Lottery(deque<struct PCB*> *ready)
{
    //open the input and output files
    srand(time(NULL));
    ifstream inFile;
    string line = "";
    ofstream writeFile("C:/Users/Sam/Desktop/Github/potatOS/lottery.txt");
    struct PCB* running;
    struct PCB* previous;//previously inserted PCB, used to tell time of arrival differential
    //used for displaying/calculating execution information
    deque<struct PCB*> completedProcesses;
    int completion = 0; //total time to completion
    float average = 0; //average turnaround time
    int numJobs = 0; //total number of jobs
    int timeSlice = 0; //time for each process to run
    int tickets = 0;

    string fileName;
    cout << "Enter the data file name : ";
    cin >> fileName;

    //check to see if a file is open and if not, then oopen it and read in the info
    if (!inFile.is_open())
    {
        inFile.open(fileName.c_str());
        if (inFile.is_open())
        {
            //infinite loop to get an integer number for the time quantum
            //borrowed from http://stackoverflow.com/questions/5131647/why-would-we-call-cin-clear-and-cin-ignore-after-reading-input
            do
            {
                //for the sake of not allowing the user to enter some ridiculous time quantum and for simplicity's sake this program will only accept an
                //integer from 1 to 10 as the time slice, however this could easily be tweaked to accept bigger numbers
                cout << "Enter an integer number for the time slice (number must be greater than 0 and less than 11): ";
                if (!(cin >> timeSlice))
                {
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
                else if (timeSlice > 0 && timeSlice < 11)
                {
                    break;
                }
                cout << "*ERROR: Input number invalid, try again" << endl;
            }while(1);
            do
            {
                //only accepting numbers of 100 or greater in order to prevent the user from having less tickets than processes, however this can be easily altered
                cout << "Enter an integer number for the amount of lottery tickets in the system (number must be greater than 99): ";
                if (!(cin >> tickets))
                {
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
                else if (tickets > 99)
                {
                    break;
                }
                cout << "*ERROR: Input number invalid, try again" << endl;
            }while(1);

            //read in file line by line
            while (getline(inFile, line))
            {
                int startPos=0;
                int endPos=0;
                vector<string> words;
                //go through the line read in and store each word dilimeted by spaces into a string vecor
                for (unsigned int i = 0; i < line.size(); i++)
                {
                    char at = line.at(i);
                    if (at == ' ')
                    {
                        int size = endPos - startPos;
                        string word = line.substr(startPos, size);
                        words.push_back(word);
                        endPos++;
                        startPos = endPos;
                    }
                    else
                    {
                        endPos++;
                    }
                }
                //store the last value into the string vector
                int size = endPos - startPos;
                string word = line.substr(startPos, size);
                words.push_back(word);

                //change string values to proper types and setup the pcb
                string temp = words.at(1);
                bool classType;
                if (temp == "A")
                {
                    classType = APP;
                }
                else
                {
                    classType = SYS;
                }
                temp = words.at(2);
                int priority = atoi(temp.c_str());//this sets each new process to the highest priority level (0 being highest priority with priority decreasing as a process's priority value approaches the numQueues given)
                temp = words.at(3);
                int memory = atoi(temp.c_str());
                temp = words.at(4);
                int timeRemaining = atoi(temp.c_str());
                temp = words.at(5);
                int timeArrival = atoi(temp.c_str());
                temp = words.at(6);
                int percentCPU = atoi(temp.c_str());
                struct PCB* newPCB = SetupPCB(words.at(0), priority, classType, memory, timeRemaining, timeArrival, percentCPU);
                writeFile << newPCB->processName << " has entered into the system" << endl;
                ready->push_back(newPCB);
            }
            inFile.close();
        }
        else
        {
            cout << "*ERROR: Unable to open file*\n";
            return;
        }
    }
    else
    {
        cout << "*ERROR: File is already open*\n";
        return;
    }
    //as long as we still have processes ready to execute, go through with execution
    if(ready->size() != 0)
    {
        //finish executing the rest of the processes
        while(ready->size() != 0)
        {
            int randomTicket = rand() % tickets + 1;
            int min = 0;
            for (unsigned int i = 0; i < ready->size(); i++)
            {
                struct PCB* running = ready->at(i);
                int max = (((float)running->percentCPU / 100) * tickets) + min;
                if (randomTicket >= min && randomTicket <= max)
                {
                    //can be used to see the queue as processes are run
                    //ShowReady(*ready);
                    writeFile << running->processName << " is running" << endl;
                    ready->erase(ready->begin() + i);
                    Sleep(timeSlice * 1000);
                    completion = completion + timeSlice;
                    running->timeRemaining = running->timeRemaining - timeSlice;
                    //check to see if the running process has completed, if so increment numJobs variable and display completion info, if not push back into deque
                    if (running->timeRemaining <= 0 )
                    {
                        numJobs++;
                        cout << "  ** " << running->processName << " completed **" << endl;
                        writeFile << running->processName << " completed execution" << endl;
                        completedProcesses.push_back(running);
                    }
                    else
                    {
                        ready->insert(ready->begin() + i, running);
                    }
                    break;
                }
                min = max;
            }
        }
    }
    //display order of execution, total time to completion, and avg turnaround
    cout << "\n  *****Process Execution Complete*****" << endl;
    cout << "  Order of Execution:" << endl;
    for(unsigned int i = 0; i < completedProcesses.size(); i++)
    {
        struct PCB* temp = completedProcesses.at(i);
        cout << "    " << temp->processName << endl;
    }
    average = (float)completion / (float)numJobs;
    cout << "  Total Time to Completion -- " << completion << " seconds" << endl;
    cout << "  Average Turnaround Time --- " << average << " seconds" << endl;
    writeFile << "\n**EXECUTION COMPLETE**\n";
    writeFile.close();
}















