/*
    Created by: Hector Soto
    Description: Main for RISC-V Simulator
    Handles the terminal menus.
*/

#include <iostream> //For IO.
#include <string> //For strings.

#include "General.h" //For general stuff.
#include "DirectoryList.h" //For handling the directory.
#include "RVSimulator.h" //For RISC-V Simulator.

#define IGNORE_LEN 1000 //Ignore length for incorrect input.

using namespace std; //So we don't have to reference each time. E.g. std::cout or std::endl

namespace MainData //Holds important data for main.cpp
{
    bool Simulate = false; //If true, program will immediately simulate and close.
    string FileName; //Name of file to simulate.
    string Suffix; //Suffix to check for loadable files.
    DirectoryList Directory; //The directory.
    RVSimulator Simulator; //Simulator
}

void DisplayTitle() //Displays the title.
{
    cout << "---| RISC-V Simulator V0.03 ( ";
    if(MainData::Simulator.DebugMode)
    {
        cout << "Debug ";
    }
    if(MainData::Simulator.TraceFile)
    {
        cout << "TraceFile ";
    }
    if(MainData::Simulator.MemoryTraceFile)
    {
        cout << "MemTraceFile ";
    }
    if(!MainData::FileName.empty()) //If we've loaded a file.
    {
        cout << ") [Filed Selected: " << MainData::FileName << "] |---" << endl;
    }
    else
    {
        cout << ") [No Filed Selected] |---" << endl;
    }
    cout << "Behavior Flags: ";
    if(MainData::Simulator.ProtectInstructions)
    {
        cout << "ProtectInstructions ";
    }
    cout << endl;
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
        if(ArgHolder.compare("-d") == 0) //Enable debug mode.
        {
            MainData::Simulator.DebugMode = true;
        }
        else if(ArgHolder.compare("-tf") == 0) //Enable trace file.
        {
            MainData::Simulator.TraceFile = true;
        }
        else if(ArgHolder.compare("-mtf") == 0) //Enable memory trace file.
        {
            MainData::Simulator.MemoryTraceFile = true;
        }
        else if(ArgHolder.compare("-pi") == 0) //Enable memory trace file.
        {
            MainData::Simulator.ProtectInstructions = true;
        }
        else if(ArgHolder.compare("-pc") == 0)
        {
            if((i+1) < argc) //If the next argument exists, take it as register value.
            {
                ++i; ArgHolder = argv[i];
                try
                {
                    MainData::Simulator.Register[REG_PC] = std::stoi(ArgHolder, nullptr, 16);
                    cout << "PC set to: 0x" << hex << setw(8) << setfill('0') << std::stoi(ArgHolder, nullptr, 16) << endl; cout << dec  << setfill(' ');
                }
                catch(invalid_argument &)
                {
                    cout << "Failed to set PC. Was given '" << ArgHolder << "'" << endl;
                }
            }
        }
        else if(ArgHolder.compare("-sp") == 0)
        {
            if((i+1) < argc) //If the next argument exists, take it as register value.
            {
                ++i; ArgHolder = argv[i];
                try
                {
                    MainData::Simulator.Register[REG_SP] = std::stoi(ArgHolder, nullptr, 16);
                    cout << "SP set to: 0x" << hex << setw(8) << setfill('0') << std::stoi(ArgHolder, nullptr, 16) << endl; cout << dec  << setfill(' ');
                }
                catch(invalid_argument &)
                {
                    cout << "Failed to set SP. Was given '" << ArgHolder << "'" << endl;
                }
            }
        }
        else if(ArgHolder.compare("-sfx") == 0) //Set suffix
        {
            if((i+1) < argc) //If the next argument exists, take it as a suffix.
            {
                ++i; ArgHolder = argv[i];
                MainData::Suffix = argv[i];
            }
        }
        else if(ArgHolder.compare("-f") == 0) //Set file name.
        {
            if((i+1) < argc) //If the next argument exists, take it as a file name.
            {
                ++i; ArgHolder = argv[i];
                MainData::FileName = argv[i];
            }
        }
        else if(ArgHolder.compare("-s") == 0) //Simulate
        {
            MainData::Simulate = true;
        }
    }
    if(MainData::Simulate)
    {
        MainData::Simulator.Simulate(MainData::FileName.c_str());
        goto END_SIM;
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
                    MainData::Simulator.Simulate(MainData::FileName.c_str());
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
END_SIM:
    cout << "Simulator ended." << endl;
    return 0;
}
