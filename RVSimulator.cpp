/*
    Created By: Hector Soto
*/

#include "RVSimulator.h"

using namespace std;

/*
    CONSTRUCTOR
*/
RVSimulator::RVSimulator()
{
    Register[REG_PC] = 0;
    Register[REG_ZERO] = 0;
    Register[REG_RA] = 0;
    Register[REG_SP] = 0;
}

/*
    DESTRUCTOR
*/
RVSimulator::~RVSimulator()
{

}

void RVSimulator::Simulate(const char *MemoryFile)
{
    ifstream File; //Filed being loaded.
    ofstream OutputFile; //Output file.
    uint32_t Address = 0, Instruction = 0; //To hold address and instruction loaded from file.
    string InputData; //Hold input data as string.
    bool Reason = false; //Set to true for specific read fail reason.
    bool DataType = true;
    if(!MemoryFile) //Need memory file to simulate.
    {
        cout << "Error: No memory file given." << endl;
        return;
    }
    File.open(MemoryFile); //Open the selected file for simulation.
    if(!File.is_open()) //Need to be able to open file.
    {
        cout << "Error: Unable to open '" << MemoryFile << "'." << endl;
    }
    cout << hex << setfill('0');
    if(DebugMode)
    {
        cout << "---| Data Read from File |---" << endl;
    }
    while(File.peek() != EOF) //Loads data until EOF.
    {
        File >> InputData;
        InputData = "0x" + InputData;
        if(DataType) //Get Address
        {
            InputData.pop_back(); //Remove last charater ':' from address.
            try
            {
                Address = std::stoul(InputData, nullptr, 16);
            }
            catch(invalid_argument &)
            {
                cout << "Failed to get address. Was given '" << InputData << "'" << endl;
                Reason = true; goto TRACE_FAIL;
            }
        }
        else //Get instruction data.
        {
            try
            {
                Instruction = std::stoul(InputData, nullptr, 16);
                if(DebugMode)
                {
                    cout << "Address 0x" << setw(8) << Address << " | Data: 0x" << setw(8) << Instruction << endl;
                }
            }
            catch(invalid_argument &)
            {
                cout << "Failed to get instruction. Was given '" << InputData << "'" << endl;
                Reason = true; goto TRACE_FAIL;
            }
        }
        DataType = !DataType;
    }
DATA_LOADED:
    cout << dec << setfill(' ');
    return;
TRACE_FAIL: //Failed to read a request.
    cout << dec << setfill(' ');
    if(Reason) //If a reason was given.
    {
        cout << "Failed to simulate file, reason is above." << endl;
    }
    return;
}
