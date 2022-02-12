/*
    Created by: Hector Soto
    Description: Main for RISC-V Simulator
    Handles the terminal menus.
*/

#include <iostream> //For IO.
#include <string> //For strings.

#include "General.h" //For general stuff.
#include "DirectoryList.h" //For handling the directory.
#include "SimMemory.h" //For simulation memory.

#define IGNORE_LEN 1000 //Ignore length for incorrect input.

using namespace std; //So we don't have to reference each time. E.g. std::cout or std::endl

namespace MainData //Holds important data for main.cpp
{
    bool DebugMode = 0; //In debug mode if true.
    string FileName; //Name of file to simulate.
    string Suffix; //Suffix to check for loadable files.
    DirectoryList Directory; //The directory.
}

void DisplayTitle() //Displays the title.
{
    cout << "---| RISC-V Simulator V0.00 ( ";
    if(MainData::DebugMode)
    {
        cout << "Debug ";
    }
    if(!MainData::FileName.empty()) //If we've loaded a file.
    {
        cout << ") [Filed Selected: " << MainData::FileName << "] |---" << endl;
    }
    else
    {
        cout << ") [No Filed Selected] |---" << endl;
    }
    return;
}

void SelectFileMenu() //User selects a file.
{
    int Choice = 0;
    MainData::Directory.LoadDirectory(MainData::Suffix.c_str()); //Load names in current directory.
    CLEAR_SCREEN;
    while(true)
    {
        DisplayTitle();
        cout << "Select a file." << endl;
        MainData::Directory.Display();
        cout << "Choice: ";
        cin >> Choice;
        if(MainData::Directory.GetSize() == 0)
        {
            cout << "No loadable files in current directory." << endl;
            return;
        }
        if((Choice <= MainData::Directory.GetSize()) && (Choice >= 1))
        {// If the choice was a possible number in the list.
            MainData::Directory.GetString(Choice, MainData::FileName); //Get selected file name.
            CLEAR_SCREEN;
            break; //Exit the loop.
        }
        else
        {
            CLEAR_SCREEN;
            cout << "Please choose a valid file." << endl;
        }
    }
    return;
}

int main(int argc, char *argv[])
{
    int Choice = 0;
    string ArgHolder; //To hold arguments.
    CLEAR_SCREEN; //Clear the screen.
    //Load command arguments
    for(int i = 1; i < argc; ++i)
    {
        ArgHolder = argv[i];
        if(ArgHolder.compare("-d") == 0) //If the user wants type checking for loading files.
        {
            MainData::DebugMode = true;
            cout << "Debug mode is enabled." << endl;
        }
        else if(ArgHolder.compare("-s") == 0) //Set suffix
        {
            if((i+1) < argc) //If the next argument exists, take it as a file name.
            {
                ++i;
                MainData::Suffix = argv[i];
            }
        }
    }
    while(Choice != 4) //While we haven't selected to quit.
    {
        DisplayTitle(); //Display title.
        cout << "1. Select file" << endl;
        cout << "2. Simulate" << endl;
        cout << "3. Settings" << endl;
        cout << "4. Quit" << endl;
        cout << "Choice: ";
        cin >> Choice; //Get input for choice.
        if(std::cin) //If we got an integer.
        {
            cin.clear();
            cin.ignore(IGNORE_LEN, '\n'); //Clear extra input.
            switch(Choice)
            {
                case 1: //Select file.
                    SelectFileMenu();
                    CLEAR_SCREEN;
                    break;
                case 2: //Simulate file.
                    CLEAR_SCREEN;
                    break;
                case 3: //Simulate file.
                    CLEAR_SCREEN;
                    break;
                case 4: //Quit the program.
                    CLEAR_SCREEN;
                    break;
                default: //Invalid input given.
                    CLEAR_SCREEN;
                    cout << "Please select a valid option." << endl;
                    break;
            }
        }
        else
        {
            cin.clear(); //Reset error flags.
            cin.ignore(IGNORE_LEN, '\n'); //Clear the invalid input.
            CLEAR_SCREEN;
            cout << "Please enter an integer." << endl;
        }
    }
    cout << "Simulator ended." << endl;
    return 0;
}
