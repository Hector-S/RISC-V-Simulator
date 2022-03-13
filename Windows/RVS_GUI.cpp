/*
    Created By: Hector Soto
    Uses SDL2, SDL2_image and SDL_ttf.
*/

#include "RVS_GUI.h"
#include "RVSimulator.h"

#define ADDR_ROWS 20
#define REGS_ROWS 16

using namespace std;

int ResW = GUI_WIDTH; //Resolution width.
int ResH = GUI_HEIGHT; //Resolution height.
SDL_Window *Window = nullptr; //The window.
SDL_Renderer* Renderer = nullptr; //The renderer.
SDL_Event Event; //To capture events.

//Initiates SDL2. Returns non-zero on failure.
int Init()
{
    //Initialize SDL.
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }
    //Initialize SDL_image.
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) //Enabled PNG loading. Returns bitmask of enabled image formats.
    {
        SDL_Log("Unable to initialize SDL_Image: %s\n", IMG_GetError());
        SDL_Quit();
        return -2;
    }
    //Initialize SDL_ttf.
    if(TTF_Init())
    {
        SDL_Log("Unable to initialize SDL_ttf: %s\n", TTF_GetError());
        IMG_Quit(); SDL_Quit();
        return -3;
    }
    //Create window.
    Window = SDL_CreateWindow("RISC-V Simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ResW, ResH, SDL_WINDOW_RESIZABLE);
    if(!Window)
    {
        SDL_Log("Unable to create window: %s\n", SDL_GetError());
        TTF_Quit(); IMG_Quit(); SDL_Quit();
        return -4;
    }
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    //Create renderer.
    if(!Renderer)
    {
        SDL_Log("Unable to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(Window); TTF_Quit(); IMG_Quit(); SDL_Quit();
        return -5;
    }
    //Initiate settings.
    SDL_Surface *Icon = IMG_Load("Media/RVS_Logo.png");
    if(Icon)
    {
        SDL_SetWindowIcon(Window, Icon);
        SDL_FreeSurface(Icon);
    }
    SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND); //Activate color blending.
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ); //Enable anti-aliasing.
    return 0; //Success.
}

//Quits SDL. Returns non-zero on failure.
int Exit()
{
    //Destroy window and renderer.
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    Renderer = nullptr;
    Window = nullptr;

    //Quit SDL.
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

/*
    Generates string value.
*/
string ValtoStr(int Value)
{
    std::stringstream ReturnString;
    ReturnString << "0x" << hex << setw(8) << setfill('0') << Value;
    return ReturnString.str();
}

/*
    Runs the GUI.
*/
void RVSimulator::RVS_GUI()
{
    int quit = 0; //1 = End program.
    int MouseX = 0, MouseY = 0; //Mouse position.
    stringstream temp; //For simpler string manipulation.
    bool AddressChange = false; //If true, need to update address + instruction data info.
    bool OffsetLock = false; //If true, will lock address view to current address offset.
    bool MemorySwap = false; //If true, memory locations will be shown instead of the data at the memory location.
    bool MSBHold = false; //If true. the tab button is being held down.
    bool GettingInput = false; //If true, user is typing into GUI.
    string InputString; //Holds string input.

    //Sim variables
    int AddressOffset = -40;
    int MemoryPosition = 0; //What memory address we're looking at.
    int AddressView = Register[REG_PC] + AddressOffset; //What address we're viewing.
    uint32_t InstructionLoaded = 0;
    History SimHistory; //Holds states of the registers.
    int *TempRegArray = nullptr;
    int LabelOffset = 0; //Offset for label height.
    int TempInt = 0;

    if(Init() != 0) //If initialization failed, end GUI.
    {
        return;
    }
    SDL_RaiseWindow(Window); //Put focus on the window.
    TTF_Font *Font = TTF_OpenFont(GUI_FONT, GUI_FONT_SIZE); //Font used in gui.
    if(!Font) //If we failed to load the font.
    {
        cout << "GUI Error: Failed to load font'" << GUI_FONT << "'." << endl;
        Exit();
        return;
    }
    UI_BOX Address[ADDR_ROWS + 3];
    UI_BOX Instruction[ADDR_ROWS + 1];
    UI_BOX Decoded[ADDR_ROWS + 1];
    UI_BOX RegisterData1[REGS_ROWS + 1]; //Registers 0-15.
    UI_BOX RegisterData2[REGS_ROWS]; //Registers 16-32.
    UI_BOX MemoryData[ADDR_ROWS + 3];
    UI_BOX OffLeftArrow; //Offset arrow button
    UI_BOX OffRightArrow;
    UI_BOX PCLock; //Button to toggle locking onto the PC or not.
    UI_BOX Rewind; //Button to rewind sim.
    UI_BOX HistoryCount; //Shows how many times we can rewind.
    UI_BOX Forward; //Button to move sim forward.
    UI_BOX Find; //Button to find REG_PC location.
    UI_BOX MemPosition; //Memory position.
    UI_BOX MemLeft; //Increment memory backwards.
    UI_BOX MemRight; //Increment memory forwards.
    UI_BOX MemSwap; //Toggles swapping looking at memory address vs memory data.
    //Set up UI.
    LabelOffset = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    for(int i = 0; i < ADDR_ROWS + 3; ++i)
    {
        Address[i].h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
        Address[i].w = GUI_WIDTH / 2.0 / 3.0;
        Address[i].y = (GUI_HEIGHT / (ADDR_ROWS + 3))*i;
        Address[i].Font = &Font;
        MemoryData[i].h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
        MemoryData[i].w = GUI_WIDTH / 2.0 / 3.0;
        MemoryData[i].x = MemoryData[i].w * 5;
        MemoryData[i].y = (GUI_HEIGHT / (ADDR_ROWS + 3))*i;
        MemoryData[i].Font = &Font;
        if(i < ADDR_ROWS + 1)
        {
            Instruction[i].h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
            Instruction[i].w = GUI_WIDTH / 2.0 / 3.0;
            Instruction[i].x = Instruction[i].w;
            Instruction[i].y = (GUI_HEIGHT / (ADDR_ROWS + 3))*i;
            Instruction[i].Font = &Font;
            Decoded[i].h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
            Decoded[i].w = GUI_WIDTH / 2.0 / 3.0;
            Decoded[i].x = Decoded[i].w * 2;
            Decoded[i].y = (GUI_HEIGHT / (ADDR_ROWS + 3))*i;
            Decoded[i].Font = &Font;
            if(i < REGS_ROWS + 1)
            {
                RegisterData1[i].h = (float)GUI_HEIGHT / (REGS_ROWS + 1);
                RegisterData1[i].w = GUI_WIDTH / 2.0 / 3.0;
                RegisterData1[i].x = RegisterData1[i].w * 3;
                RegisterData1[i].y = (GUI_HEIGHT / (REGS_ROWS + 1))*(i - 1) + LabelOffset;
                RegisterData1[i].Font = &Font;
            }
            if(i < REGS_ROWS)
            {
                RegisterData2[i].h = (float)GUI_HEIGHT / (REGS_ROWS + 1);
                RegisterData2[i].w = GUI_WIDTH / 2.0 / 3.0;
                RegisterData2[i].x = RegisterData2[i].w * 4;
                RegisterData2[i].y = (GUI_HEIGHT / (REGS_ROWS + 1))*i + LabelOffset;
                RegisterData2[i].Font = &Font;
            }
        }
    }
    RegisterData1[0].h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    RegisterData1[0].w = GUI_WIDTH / 3.0;
    RegisterData1[0].y = 0;
    OffLeftArrow.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    OffLeftArrow.w = GUI_WIDTH / 2.0 / 3.0 / 4.0;
    OffLeftArrow.x = Instruction[ADDR_ROWS].w;
    OffLeftArrow.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3));
    OffLeftArrow.Font = &Font;
    OffRightArrow.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    OffRightArrow.w = GUI_WIDTH / 2.0 / 3.0 / 4.0;
    OffRightArrow.x = Instruction[ADDR_ROWS].w + OffRightArrow.w;
    OffRightArrow.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3));
    OffRightArrow.Font = &Font;
    PCLock.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    PCLock.w = GUI_WIDTH / 2.0 / 3.0 / 2.0;
    PCLock.x = Instruction[ADDR_ROWS].w + OffRightArrow.w * 2;
    PCLock.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3));
    PCLock.Font = &Font;
    Rewind.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    Rewind.w = GUI_WIDTH / 2.0 / 3.0 / 4.0;
    Rewind.x = Instruction[ADDR_ROWS].w;
    Rewind.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3))*2;
    Rewind.Font = &Font;
    HistoryCount.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    HistoryCount.w = GUI_WIDTH / 2.0 / 3.0 / 4.0;
    HistoryCount.x = Rewind.x + Rewind.w;
    HistoryCount.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3))*2;
    HistoryCount.Font = &Font;
    Forward.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    Forward.w = GUI_WIDTH / 2.0 / 3.0 / 4.0;
    Forward.x = HistoryCount.x + HistoryCount.w;
    Forward.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3))*2;
    Forward.Font = &Font;
    Find.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    Find.w = GUI_WIDTH / 2.0 / 3.0 / 2.0;
    Find.x = Forward.x + Forward.w;
    Find.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3))*2;
    Find.Font = &Font;
    MemPosition.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    MemPosition.w = GUI_WIDTH / 2.0 / 3.0;
    MemPosition.x = Instruction[ADDR_ROWS].w * 2;
    MemPosition.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3));
    MemPosition.Font = &Font;
    MemLeft.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    MemLeft.w = GUI_WIDTH / 2.0 / 3.0 / 4.0;
    MemLeft.x = Instruction[ADDR_ROWS].w * 2 + MemLeft.w;
    MemLeft.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3))*2;
    MemLeft.Font = &Font;
    MemRight.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    MemRight.w = GUI_WIDTH / 2.0 / 3.0 / 4.0;
    MemRight.x = MemLeft.x + MemLeft.w;
    MemRight.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3))*2;
    MemRight.Font = &Font;
    MemSwap.h = (float)GUI_HEIGHT / (ADDR_ROWS + 3);
    MemSwap.w = GUI_WIDTH / 2.0 / 3.0 / 4.0;
    MemSwap.x = MemRight.x + MemRight.w;
    MemSwap.y = Instruction[ADDR_ROWS].y + (GUI_HEIGHT / (ADDR_ROWS + 3))*2;
    MemSwap.Font = &Font;

    Address[0].SetText("Address");
    Instruction[0].SetText("Instruction");
    Decoded[0].SetText("Decoded");
    RegisterData1[0].SetText("[x0-x15] Regs [x16-x31]");
    OffLeftArrow.SetText("<"); OffRightArrow.SetText(">"); PCLock.SetText("Lock");
    Rewind.SetText("<"); Forward.SetText(">"); Find.SetText("Find"); HistoryCount.SetText(to_string(SimHistory.Size));
    MemLeft.SetText("<"); MemRight.SetText(">");
    if(MemorySwap)
    {
        MemSwap.SetText("ADDR"); MemoryData[0].SetText("Memory Address");
    }
    else
    {
        MemSwap.SetText("DATA"); MemoryData[0].SetText("Memory Data");
    }
    temp << "0x" << hex << setfill('0') << setw(8) << AddressOffset; //Current address offset.
    Address[ADDR_ROWS + 1].SetText(temp.str());
    temp.str(std::string()); //Clear temp stream.
    temp << "0x" << hex << setfill('0') << setw(8) << Register[REG_PC]; //PC Register address.
    Address[ADDR_ROWS + 2].SetText(temp.str());
    temp.str(std::string()); //Clear temp stream.
    if(!GettingInput)
    {
        temp << "0x" << hex << setfill('0') << setw(8) << MemoryPosition; //Memory position address.
        MemPosition.SetText(temp.str());
        temp.str(std::string()); //Clear temp stream.
    }
    else
    {
        MemPosition.SetText(InputString);
    }
    for(int i = 1; i < ADDR_ROWS + 1; ++i) //Update gui text.
    {
        temp << "0x" << hex << setfill('0') << setw(8) << (AddressView + 4*(i-1));
        Address[i].SetText(temp.str());
        temp.str(std::string()); //Clear temp stream.
        if(ProtectInstructions)
        {
            InstructionLoaded = InstructionMemory.Load((int) AddressView + 4*(i-1), LS_WORD);
        }
        else
        {
            InstructionLoaded = Memory.Load((int) AddressView + 4*(i-1), LS_WORD);
        }
        temp << "0x" << hex << setfill('0') << setw(8) << InstructionLoaded;
        Instruction[i].SetText(temp.str());
        Decoded[i].SetText(DecodeInstruction(InstructionLoaded));
        if(i < REGS_ROWS + 1)
        {
            RegisterData1[i].SetText(RegValtoStr(i - 1));
            RegisterData2[i - 1].SetText(RegValtoStr(i + 15));
        }
        temp.str(std::string()); //Clear temp stream.
    }
    for(int i = 1; i < ADDR_ROWS + 3; ++i)
    {
        if(MemorySwap)
        {
            MemoryData[i].SetText(ValtoStr(MemoryPosition + 4*(i - 1)));
        }
        else
        {
            MemoryData[i].SetText(ValtoStr(Memory.Load(MemoryPosition + 4*(i - 1), LS_WORD)));
        }
    }

    while(!quit)
    {
        //Update data
        if(AddressChange)
        {
            //Resize font if needed.
            TTF_CloseFont(Font); //Free font.
            Font = TTF_OpenFont(GUI_FONT, GUI_FONT_SIZE*(float)ResH/GUI_HEIGHT); //Font used in gui.
            if(!Font) //If we failed to load the font.
            {
                cout << "GUI Error: Failed to load font'" << GUI_FONT << "'." << endl;
                return;
            }
            if(OffsetLock)
            {
                AddressView = AddressOffset;
            }
            else
            {
                AddressView = Register[REG_PC] + AddressOffset;
            }
            Address[0].SetText("Address"); //Update text resolution.
            Instruction[0].SetText("Instruction");
            Decoded[0].SetText("Decoded");
            RegisterData1[0].SetText("[x0-x15] Regs [x16-x31]");
            OffLeftArrow.SetText("<"); OffRightArrow.SetText(">"); PCLock.SetText("Lock");
            Rewind.SetText("<"); Forward.SetText(">"); Find.SetText("Find"); HistoryCount.SetText(to_string(SimHistory.Size));
            MemLeft.SetText("<"); MemRight.SetText(">");
            if(MemorySwap)
            {
                MemSwap.SetText("ADDR"); MemoryData[0].SetText("Memory Address");
            }
            else
            {
                MemSwap.SetText("DATA"); MemoryData[0].SetText("Memory Data");
            }
            temp << "0x" << hex << setfill('0') << setw(8) << AddressOffset; //Current address offset.
            Address[ADDR_ROWS + 1].SetText(temp.str());
            temp.str(std::string()); //Clear temp stream.
            temp << "0x" << hex << setfill('0') << setw(8) << Register[REG_PC]; //PC Register address.
            Address[ADDR_ROWS + 2].SetText(temp.str());
            temp.str(std::string()); //Clear temp stream.
            if(!GettingInput)
            {
                temp << "0x" << hex << setfill('0') << setw(8) << MemoryPosition; //Memory position address.
                MemPosition.SetText(temp.str());
                temp.str(std::string()); //Clear temp stream.
            }
            else
            {
                MemPosition.SetText(InputString);
            }
            for(int i = 1; i < ADDR_ROWS + 1; ++i) //Update gui text.
            {
                temp << "0x" << hex << setfill('0') << setw(8) << (AddressView + 4*(i-1));
                Address[i].SetText(temp.str());
                temp.str(std::string()); //Clear temp stream.
                if(ProtectInstructions)
                {
                    InstructionLoaded = InstructionMemory.Load((int) AddressView + 4*(i-1), LS_WORD);
                }
                else
                {
                    InstructionLoaded = Memory.Load((int) AddressView + 4*(i-1), LS_WORD);
                }
                temp << "0x" << hex << setfill('0') << setw(8) << InstructionLoaded;
                Instruction[i].SetText(temp.str());
                Decoded[i].SetText(DecodeInstruction(InstructionLoaded));
                if(i < REGS_ROWS + 1)
                {
                    RegisterData1[i].SetText(RegValtoStr(i - 1));
                    RegisterData2[i - 1].SetText(RegValtoStr(i + 15));
                }
                temp.str(std::string()); //Clear temp stream.
            }
            for(int i = 1; i < ADDR_ROWS + 3; ++i)
            {
                if(MemorySwap)
                {
                    MemoryData[i].SetText(ValtoStr(MemoryPosition + 4*(i - 1)));
                }
                else
                {
                    MemoryData[i].SetText(ValtoStr(Memory.Load(MemoryPosition + 4*(i - 1), LS_WORD)));
                }
            }
            AddressChange = false;
        }

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255); //Set screen to black.
        SDL_RenderClear(Renderer); //Clear renderer with current draw color.
        SDL_Delay(20); //Don't burn out the CPU/GPU.
        //Render UI
        SDL_SetRenderDrawColor(Renderer, 150, 150, 255, 255); //Set labels to light blue.
        Address[0].Render(Renderer);
        Instruction[0].Render(Renderer);
        Decoded[0].Render(Renderer);
        RegisterData1[0].Render(Renderer);
        MemoryData[0].Render(Renderer);
        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255); //Set address boxes to white.
        for(int i = 1; i < ADDR_ROWS + 3; ++i)
        {
            if(i == -((AddressView - Register[REG_PC]) / 4) + 1)
            {
                SDL_SetRenderDrawColor(Renderer, 90, 165, 90, 255); //Set box at REG_PC to dark green.
            }
            if(i == -((AddressView - Register[REG_PC]) / 4))
            {
                SDL_SetRenderDrawColor(Renderer, 115, 215, 115, 255); //Set box before REG_PC to green.
            }
            if((i >= 21) && ( i <= 22))
            {
                SDL_SetRenderDrawColor(Renderer, 150, 150, 150, 255); //Set offset and REG_PC box to grey.
            }
            Address[i].Render(Renderer);
            if(i < ADDR_ROWS + 1)
            {
                Instruction[i].Render(Renderer);
                if(Decoded[i].tw == 0) //No decoded instruction in box.
                {
                    SDL_SetRenderDrawColor(Renderer, 75, 75, 75, 255); //Set empty boxes to dark grey.
                }
                Decoded[i].Render(Renderer);
            }
            if(MemorySwap)
            {
                SDL_SetRenderDrawColor(Renderer, 245, 245, 155, 255); //Set to yellow.
            }
            else
            {
                SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255); //Set boxes to white.
            }
            MemoryData[i].Render(Renderer);
            SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255); //Set boxes to white.
            if(i < REGS_ROWS + 1)
            {
                RegisterData1[i].Render(Renderer);
            }
            if(i < REGS_ROWS + 1)
            {
                RegisterData2[i - 1].Render(Renderer);
            }
        }
        SDL_SetRenderDrawColor(Renderer, 150, 150, 150, 255); //Grey.
        HistoryCount.Render(Renderer);
        if(GettingInput)
        {
            SDL_SetRenderDrawColor(Renderer, 255, 90, 90, 255); //Light red.
        }
        else
        {
            SDL_SetRenderDrawColor(Renderer, 200, 150, 70, 255); //Light orange.
        }
        MemPosition.Render(Renderer);
        SDL_SetRenderDrawColor(Renderer, 200, 150, 70, 255); //Set buttons to light orange.
        OffLeftArrow.Render(Renderer); OffRightArrow.Render(Renderer);
        Rewind.Render(Renderer); Forward.Render(Renderer); Find.Render(Renderer);
        MemLeft.Render(Renderer); MemRight.Render(Renderer);
        if(OffsetLock)
        {
            SDL_SetRenderDrawColor(Renderer, 255, 90, 90, 255); //Set lock button to light red.
        }
        PCLock.Render(Renderer);
        if(MemorySwap)
        {
            SDL_SetRenderDrawColor(Renderer, 245, 245, 155, 255); //Set to yellow.
        }
        else
        {
            SDL_SetRenderDrawColor(Renderer, 200, 150, 70, 255); //Set to orange/
        }
        MemSwap.Render(Renderer);

        SDL_RenderPresent(Renderer); //Update the screen.
        SDL_GetMouseState(&MouseX, &MouseY);
        while(SDL_PollEvent(&Event)) //Clear event queue each frame.
        {
            //Mouse.UpdateButtons(&Event);
            switch(Event.type)
            {
                case SDL_QUIT: //User wants to quit.
                    quit = 1;
                    break;
                case SDL_WINDOWEVENT:
                    switch(Event.window.event)
                    {
                        case SDL_WINDOWEVENT_RESIZED: //If window was resized, update resolution.
                            SDL_GetWindowSize(Window, &ResW, &ResH);
                            break;
                    }
                    break;
                case SDL_KEYDOWN: //A button was pressed.
                    switch(Event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE: //User wants to quit.
                            quit = 1;
                            break;
                        case SDLK_RIGHT: //Right Arrow = time forward.
                            if(SimHistory.Size == HISTORY_MAX) //If the history is full.
                            { //Clear oldest history state.
                                TempRegArray = SimHistory.PopEnd();
                                if(TempRegArray)
                                {
                                    delete TempRegArray;
                                    TempRegArray = nullptr;
                                }
                            }
                            TempRegArray = new int[36]; //Assume new doesn't fail.
                            for(int i = 0; i < 33; ++i) //Copy current register states.
                            {
                                TempRegArray[i] = Register[i];
                            }
                            InstructionLoop(); //Do instruction.
                            TempRegArray[33] = StoreType; //Save memory data info before instruction loop.
                            TempRegArray[34] = LastAddress;
                            TempRegArray[35] = LAOldData;
                            SimHistory.Push(TempRegArray);
                            TempRegArray = nullptr;
                            StoreType = -1; //Invalidate StoreType.
                            Register[REG_PC] += 4; //Increment PC.
                            AddressChange = true;
                            break;
                        case SDLK_LEFT: //Time backward.
                            if(SimHistory.Size > 0)
                            {
                                TempRegArray = SimHistory.Pop();
                                if(TempRegArray)
                                {
                                    if(TempRegArray[33] != -1) //If a store had occurred, undo it.
                                    {
                                        Memory.Store(TempRegArray[34], TempRegArray[35], TempRegArray[33]);
                                    }
                                    for(int i = 0; i < 33; ++i)
                                    {
                                        Register[i] = TempRegArray[i];
                                    }
                                    delete TempRegArray; TempRegArray = nullptr;
                                    AddressChange = true;
                                }
                            }
                            break;
                        case SDLK_EQUALS: //Increase offset.
                            AddressOffset += 4; AddressChange = true;
                            break;
                        case SDLK_MINUS: //Decrease offset.
                            AddressOffset -= 4; AddressChange = true;
                            break;
                        case SDLK_COMMA: //Decrease memory position.
                            MemoryPosition -= 4; AddressChange = true;
                            break;
                        case SDLK_PERIOD: //Increase memory position.
                            MemoryPosition += 4; AddressChange = true;
                            break;
                        case SDLK_TAB: //Temporarily swap memory data/address display.
                            if(!MSBHold)
                            {
                                MSBHold = true;
                                MemorySwap = !MemorySwap; AddressChange = true;
                            }
                            break;
                        case SDLK_RETURN:
                            if(GettingInput)
                            {
                                GettingInput = false;
                                try{TempInt = std::stoll(InputString.c_str(), nullptr, 16);}
                                catch(invalid_argument &){TempInt = MemoryPosition;}
                                catch(out_of_range &){TempInt = MemoryPosition;}
                                InputString.clear();
                                MemoryPosition = TempInt;
                                SDL_StopTextInput(); AddressChange = true;
                            }
                            break;
                        case SDLK_BACKSPACE:
                            if(!InputString.empty() && GettingInput)
                            {
                                InputString.pop_back(); //Remove last character.
                                AddressChange = true;
                            }
                            break;
                        default:

                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(Event.key.keysym.sym)
                    {
                        case SDLK_TAB:
                            MemorySwap = !MemorySwap; AddressChange = true;
                            MSBHold = false;
                            break;
                        default:

                            break;
                    }
                    break;
                case SDL_MOUSEWHEEL: //If mouse wheel moved.
                    if(Event.wheel.y > 0) //If the mouse moved up.
                    {

                    }
                    else
                    {

                    }
                    break;
                case SDL_MOUSEBUTTONUP: //Lifting mouse button after click.
                    switch(Event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            if(Forward.Inside(MouseX, MouseY))
                            {
                                if(SimHistory.Size == HISTORY_MAX) //If the history is full.
                                { //Clear oldest history state.
                                    TempRegArray = SimHistory.PopEnd();
                                    if(TempRegArray)
                                    {
                                        delete TempRegArray;
                                        TempRegArray = nullptr;
                                    }
                                }
                                TempRegArray = new int[36]; //Assume new doesn't fail.
                                for(int i = 0; i < 33; ++i) //Copy current register states.
                                {
                                    TempRegArray[i] = Register[i];
                                }
                                InstructionLoop(); //Do instruction.
                                TempRegArray[33] = StoreType; //Save memory data info before instruction loop.
                                TempRegArray[34] = LastAddress;
                                TempRegArray[35] = LAOldData;
                                SimHistory.Push(TempRegArray);
                                TempRegArray = nullptr;
                                StoreType = -1; //Invalidate StoreType.
                                Register[REG_PC] += 4; //Increment PC.
                                AddressChange = true;
                            }
                            else if(Rewind.Inside(MouseX, MouseY))
                            {
                                if(SimHistory.Size > 0)
                                {
                                    TempRegArray = SimHistory.Pop();
                                    if(TempRegArray)
                                    {
                                        if(TempRegArray[33] != -1) //If a store had occurred, undo it.
                                        {
                                            Memory.Store(TempRegArray[34], TempRegArray[35], TempRegArray[33]);
                                        }
                                        for(int i = 0; i < 33; ++i)
                                        {
                                            Register[i] = TempRegArray[i];
                                        }
                                        delete TempRegArray; TempRegArray = nullptr;
                                        AddressChange = true;
                                    }
                                }
                            }
                            else if(MemPosition.Inside(MouseX, MouseY)) //Clicked memory address box.
                            {
                                GettingInput = true;
                                InputString = ValtoStr(MemoryPosition);
                                SDL_StartTextInput(); AddressChange = true;
                            }
                            else if(PCLock.Inside(MouseX, MouseY)) //If we clicked the PC-Lock button
                            {
                                if(!OffsetLock)
                                {
                                    AddressOffset = AddressView; //Lock offset to current location.
                                }
                                else
                                {
                                    AddressOffset = AddressView - Register[REG_PC];
                                }
                                OffsetLock = !OffsetLock; AddressChange = true;
                            }
                            else if(OffLeftArrow.Inside(MouseX, MouseY))
                            {
                                AddressOffset -= 4; AddressChange = true;
                            }
                            else if(OffRightArrow.Inside(MouseX, MouseY))
                            {
                                AddressOffset += 4; AddressChange = true;
                            }
                            else if(Find.Inside(MouseX, MouseY))
                            {
                                OffsetLock = false; //We want to track the REG_PC address.
                                AddressOffset = -40; //Show it at center of displayed addresses.
                                AddressChange = true;
                            }
                            else if(MemLeft.Inside(MouseX, MouseY))
                            {
                                MemoryPosition -= 4; AddressChange = true;
                            }
                            else if(MemRight.Inside(MouseX, MouseY))
                            {
                                MemoryPosition += 4; AddressChange = true;
                            }
                            else if(MemSwap.Inside(MouseX, MouseY))
                            {
                                MemorySwap = !MemorySwap; AddressChange = true;
                            }
                            break;
                        case SDL_BUTTON_MIDDLE:

                            break;
                        case SDL_BUTTON_RIGHT:

                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN: //Pushing mouse down.
                    switch(Event.button.button)
                    {
                        case SDL_BUTTON_LEFT:

                            break;
                        case SDL_BUTTON_MIDDLE:

                            break;
                        case SDL_BUTTON_RIGHT:

                            break;
                    }
                    break;
                case SDL_TEXTINPUT:
                    InputString += Event.text.text;
                    AddressChange = true;
                    break;
                default:

                    break;
            }
        }
    }
    //Free history data
    while(SimHistory.Size != 0)
    {
        TempRegArray = SimHistory.Pop();
        if(TempRegArray)
        {
            delete TempRegArray;
        }
    }
    if(Font)
    {
        TTF_CloseFont(Font); //Free font.
    }
    Exit();
    return;
}

/*
    CONSTRUCTOR
*/
UI_BOX::UI_BOX()
{

}

/*
    DESTRUCTOR
*/
UI_BOX::~UI_BOX()
{

}

/*
    Render box.
    WARNING: Assumes Renderer is not nullptr. Assumes Font is not nullptr twice.
*/
void UI_BOX::Render(SDL_Renderer *Renderer)
{
    uint8_t r = 0, g = 0, b = 0, a = 0;
    int cx = x*(float)ResW/GUI_WIDTH; //Calculate render x.
    int cy = y*(float)ResH/GUI_HEIGHT;
    int cw = w*(float)ResW/GUI_WIDTH;
    int ch = h*(float)ResH/GUI_HEIGHT;
    int rtw = tw; //Calculate render width of tw.
    int rth = th;
    float Ratio = 0;
    if(((ch*0.8) - th) < ((cw*0.9) - tw))
    {
        Ratio = (ch*0.8) / th; //Get ratio
    }
    else
    {
        Ratio = (cw*0.9) / tw; //Get ratio
    }
    rtw = tw * Ratio;
    rth = th * Ratio;
    if(rtw > cw)
    {
        Ratio = (float)cw / rtw;
        rtw = tw * Ratio;
        rth = th * Ratio;
    }

    SDL_GetRenderDrawColor(Renderer, &r, &g, &b, &a); //Save render draw color.
    SDL_Rect Rect = {cx, cy, cw, ch};
    SDL_RenderFillRect(Renderer, &Rect);
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255); //Set color to black.
    SDL_RenderDrawRect(Renderer, &Rect); //Outline will be black.
    if(Text) //Render text if it's not empty.
    {
        Rect = {(int)(cx + ((cw-rtw)/2.0)), (int)(cy + ((ch-rth)/2.0)), rtw, rth};
        SDL_RenderCopy(Renderer, Text, NULL, &Rect);
    }
    SDL_SetRenderDrawColor(Renderer, r, g, b, a); //Restore render draw color.
    return;
}

/*
    Creates texture from given text.
    Text is always black. Assumes *Font is not nullptr.
*/
void UI_BOX::SetText(string NewText)
{
    tw = 0; th = 0; //Make sure text size always 0 if text creation fails.
    if(Text) //Free text texture.
    {
        SDL_DestroyTexture(Text);
        Text = nullptr;
    }
    if(!NewText.empty() && Font)
    {
        SDL_Color TextColor = {0, 0, 0}; //Text will be black.
        SDL_Surface *TextSurface = TTF_RenderText_Blended(*Font, NewText.c_str(), TextColor);
        if(TextSurface)
        {
            Text = SDL_CreateTextureFromSurface(Renderer, TextSurface);
            TTF_SizeText(*Font, NewText.c_str(), &tw, &th); //Get text width and height.
            tw /= GUI_FONT_SCALE; th /= GUI_FONT_SCALE;
            SDL_FreeSurface(TextSurface);
        }
    }
    return;
}

/*
    Returns true if X and Y are inside the box.
*/
bool UI_BOX::Inside(int X, int Y)
{
    X /= (float)ResW/GUI_WIDTH;
    Y /= (float)ResH/GUI_HEIGHT;
    if((X >= x) && (X <= (x + w)) && (Y >= y) && (Y <= (y + h)))
    {
        return true;
    }
    return false;
}

/*
    Decode I-type instructions.
*/
string DII(uint32_t Instruction)
{
    std::stringstream ReturnString;
    uint8_t opcode = Instruction & 0x7F; //First 7 bits.
    uint8_t rd = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint8_t funct3 = (Instruction >> 12) & 0x07; //Next 3 bits.
    uint8_t rs1 = (Instruction >> 15) & 0x1F; //Next 5 bits.
    uint16_t imm = (Instruction >> 20) & 0x0FFF; //Last 12 bits.
    int temp = 0; //Hold signed data.

    if((opcode == OP_JALR) && (funct3 == FUNC_JALR))
    {
        temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
        ReturnString << "JALR " << RegtoStr(rd) << ',' << dec << temp << '(' << RegtoStr(rs1) << ')';
    }
    else if(Instruction == OP_IMMEDIATE) //Catch NOP instruction.
    {
        ReturnString << "NOP";
    }
    else if(opcode == OP_LOAD) //If we have a load instruction.
    {
        temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
        switch(funct3)
        {
            case FUNC_LW: //Load a word.
                ReturnString << "LW ";
                break;
            case FUNC_LH: //Load 2 bytes.
                ReturnString << "LH ";
                break;
            case FUNC_LB: //Load 1 byte.
                ReturnString << "LB ";
                break;
            case FUNC_LHU: //Load 2 bytes unsigned.
                ReturnString << "LHU ";
                break;
            case FUNC_LBU: //Load 1 byte unsigned.
                ReturnString << "LBU ";
                break;
        }
        ReturnString << RegtoStr(rd) << ',' << dec << temp << '(' << RegtoStr(rs1) << ')';
    }
    else
    {
        switch(funct3)
        {
            case FUNC_ADDI: //Add immediate.
                temp = imm << 20; temp = temp >> 20;
                ReturnString << "ADDI " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp;
                break;
            case FUNC_SLTI: //Set less than immediate.
                temp = imm << 20; temp = temp >> 20;
                ReturnString << "SLTI " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp;
                break;
            case FUNC_SLTIU: //Set less than immediate unsigned.
                temp = imm << 20; temp = temp >> 20;
                ReturnString << "SLTIU " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp;
                break;
            case FUNC_XORI: //XOR immediate.
                temp = imm << 20; temp = temp >> 20;
                ReturnString << "XORI " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp;
                break;
            case FUNC_ORI: //OR immediate.
                temp = imm << 20; temp = temp >> 20;
                ReturnString << "ORI " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp;
                break;
            case FUNC_ANDI: //AND immediate.
                temp = imm << 20; temp = temp >> 20;
                ReturnString << "ANDI " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp;
                break;
            case FUNC_SLLI: //Logical shift left.
                ReturnString << "SLLI " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp;
                break;
            case FUNC_SRLI: //Logical shift right.
                if((imm >> 5) == 0x00) //If unsigned, fill with zeros.
                {
                    ReturnString << "SRLI " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp;
                }
                else if((imm >> 5) == 0x20) //SRAI. If signed.
                {
                    ReturnString << "SRAI " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp;
                }
                break;
        }
    }
    return ReturnString.str();
}

/*
    Decode JAL instruction.
*/
string DJALI(uint32_t Instruction)
{
    std::stringstream ReturnString;
    uint8_t rd = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint32_t temp = (Instruction >> 12) & 0xFFFFF; //Next 20 bits.
    int32_t imm = 0; //The signed immediate.
    imm = temp >> 9; // First 10 bits.
    imm |= ((temp >> 8) & 0x1) << 10; //Get bit 11.
    imm |= (temp & 0x0FF) << 11; //Get next 8 bits
    imm |= temp & 0x80000; //Get bit 20.
    imm *= 2; //Get multiples of 2 bytes.
    ReturnString << "JAL " << RegtoStr(rd) << ",0x" << hex << imm;

    return ReturnString.str();
}

/*
    Decode S-type instructions.
*/
string DSI(uint32_t Instruction)
{
    std::stringstream ReturnString;
    uint8_t imm0 = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint8_t funct3 = (Instruction >> 12) & 0x07; //Next 3 bits.
    uint8_t rs1 = (Instruction >> 15) & 0x1F; //Next 5 bits.
    uint8_t rs2 = (Instruction >> 20) & 0x1F; //Next 5 bits.
    uint16_t imm1 = (Instruction >> 25) & 0x7F; //Next 7 bits.
    int32_t temp = 0; //Hold signed data.
    imm1 = (imm1 << 5) | imm0; //Combine immediate value.
    temp = imm1 << 20; temp = temp >> 20; //Get sign-extended immediate.
    switch(funct3)
    {
        case FUNC_SW:
            ReturnString << "SW " << RegtoStr(rs2) << ',' << dec << temp << '(' << RegtoStr(rs1) << ')';
            break;
        case FUNC_SH:
            ReturnString << "SH " << RegtoStr(rs2) << ',' << dec << temp << '(' << RegtoStr(rs1) << ')';
            break;
        case FUNC_SB:
            ReturnString << "SB " << RegtoStr(rs2) << ',' << dec << temp << '(' << RegtoStr(rs1) << ')';
            break;
    }
    return ReturnString.str();
}

/*
    Decode R-type instructions.
*/
string DRI(uint32_t Instruction)
{
    std::stringstream ReturnString;
    uint8_t rd = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint8_t funct3 = (Instruction >> 12) & 0x07; //Next 3 bits.
    uint8_t rs1 = (Instruction >> 15) & 0x1F; //Next 5 bits.
    uint8_t rs2 = (Instruction >> 20) & 0x1F; //Next 5 bits.
    uint8_t funct7 = (Instruction >> 25) & 0x7F; //Next 7 bits.
    switch(funct3)
    {
        case FUNC_ADD: //ADD or SUB
            if(funct7 == 0x00) //ADD
            {
                ReturnString << "ADD " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            else if(funct7 == 0x20) //SUB
            {
                ReturnString << "SUB " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            break;
        case FUNC_SLL:
            if(funct7 == 0x00)
            {
                ReturnString << "SLL " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            break;
        case FUNC_SLT:
            if(funct7 == 0x00)
            {
                ReturnString << "SLT " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            break;
        case FUNC_SLTU:
            if(funct7 == 0x00)
            {
                ReturnString << "SLTU " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            break;
        case FUNC_XOR:
            if(funct7 == 0x00)
            {
                ReturnString << "XOR " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            break;
        case FUNC_SRL:
            if(funct7 == 0x00) //SRL
            {
                ReturnString << "SRL " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            else if(funct7 == 0x20) //SRA
            {
                ReturnString << "SRA " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            break;
        case FUNC_OR:
            if(funct7 == 0x00)
            {
                ReturnString << "OR " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            break;
        case FUNC_AND:
            if(funct7 == 0x00)
            {
                ReturnString << "AND " << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2);
            }
            break;
    }
    return ReturnString.str();
}

/*
    Decode B-type instructions.
*/
string DBI(uint32_t Instruction)
{
    std::stringstream ReturnString;
    uint8_t imm0 = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint8_t funct3 = (Instruction >> 12) & 0x07; //Next 3 bits.
    uint8_t rs1 = (Instruction >> 15) & 0x1F; //Next 5 bits.
    uint8_t rs2 = (Instruction >> 20) & 0x1F; //Next 5 bits.
    uint16_t imm1 = (Instruction >> 25) & 0x7F; //Next 7 bits.
    int32_t temp = 0; //Hold signed data.
    //Get signed immediate.
    temp = imm0 >> 1; //Insert first 4 bits.
    temp |= imm1 << 4; //Insert next 6 bits.
    temp |= (imm1 & 0x40) << 10; //Insert last bit.
    temp = temp << 20; temp = temp >> 20; //Sign extend temp.
    temp *= 2; //Branch instruction uses 2-byte interval.
    switch(funct3)
    {
        case FUNC_BEQ:
            ReturnString << "BEQ ";
            break;
        case FUNC_BNE:
            ReturnString << "BNE ";
            break;
        case FUNC_BLT:
            ReturnString << "BLT ";
            break;
        case FUNC_BGE:
            ReturnString << "BGE ";
            break;
        case FUNC_BLTU:
            ReturnString << "BLTU ";
            break;
        case FUNC_BGEU:
            ReturnString << "BGEU ";
            break;
    }
    ReturnString << RegtoStr(rs1) << ',' << RegtoStr(rs2) << ',' << dec << temp;
    return ReturnString.str();
}

/*
    Decode U-type instructions.
*/
string DUI(uint32_t Instruction)
{
    std::stringstream ReturnString;
    uint8_t opcode = Instruction & 0x7F; //First 7 bits.
    uint8_t rd = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint32_t imm = (Instruction >> 12) & 0xFFFFF; //Next 20 bits.
    switch(opcode)
    {
        case OP_LUI: //Load Upper Immediate.
            ReturnString << "LUI ";
            break;
        case OP_AUIPC: //Add Upper Immediate to PC.
            ReturnString << "AUIPC ";
            break;
    }

    ReturnString << RegtoStr(rd) << ",0x" << hex << imm;
    return ReturnString.str();
}

/*
    Returns string of decoded instruction
*/
string DecodeInstruction(uint32_t Instruction)
{
    uint32_t Opcode = Instruction & 0x0000007F;
    string ReturnString;
    switch(Opcode)
    {
        case OP_IMMEDIATE: //I-type instructions.
            ReturnString = DII(Instruction);
            break;
        case OP_JALR:
            ReturnString = DII(Instruction);
            break;
        case OP_JAL: //JAL Instruction.
            ReturnString = DJALI(Instruction);
            break;
        case OP_LOAD:
            ReturnString = DII(Instruction);
            break;
        case OP_STORE:  //S-type instructions.
            ReturnString = DSI(Instruction);
            break;
        case OP_REGISTER: //R-type instructions.
            ReturnString = DRI(Instruction);
            break;
        case OP_BRANCH: //B-type instructions.
            ReturnString = DBI(Instruction);
            break;
        case OP_LUI: //Load upper immediate.
            ReturnString = DUI(Instruction);
            break;
        case OP_AUIPC: //Load upper immediate.
            ReturnString = DUI(Instruction);
            break;
    }
    return ReturnString;
}

// ---| IMPLEMENT History |---

/*
    CONSTRUCTOR
*/
History::History()
{

}

/*
    DESTRUCTOR
    Assumes all int pointers are stored elsewhere.
    Those int pointers will not be freed.
*/
History::~History()
{
    HistoryNode *temp = nullptr;
    while(Head)
    {
        temp = Head->Next;
        delete Head;
        Head = temp;
    }
}

/*
    Push register data into history.
*/
void History::Push(int *NewRegisterData)
{
    HistoryNode *temp = nullptr;
    if(NewRegisterData)
    {
        if(Head)
        {
            temp = new HistoryNode;
            temp->RegisterData = NewRegisterData;
            temp->Next = Head;
            Head->Previous = temp;
            Head = temp;
        }
        else
        {
            Head = new HistoryNode;
            Head->RegisterData = NewRegisterData;
            Tail = Head;
        }
        ++Size;
    }
    return;
}

/*
    Returned array should be freed.
*/
int *History::Pop()
{
    int *ReturnValue = nullptr;
    HistoryNode *temp = nullptr;
    if(Head)
    {
        ReturnValue = Head->RegisterData;
        temp = Head->Next;
        delete Head;
        Head = temp;
        if(!Head)
        {
            Tail = Head;
        }
        else
        {
            Head->Previous = nullptr;
            if(!Head->Next)
            {
                Tail = Head;
            }
        }
        --Size;
    }
    return ReturnValue;
}

/*
    Returned array should be freed.
*/
int *History::PopEnd()
{
    int *ReturnValue = nullptr;
    HistoryNode *temp = nullptr;
    if(Tail)
    {
        ReturnValue = Tail->RegisterData;
        temp = Tail->Previous;
        delete Tail;
        Tail = temp;
        Tail->Next = nullptr;
        if(!Tail)
        {
            Head = Tail;
        }
        --Size;
    }
    return ReturnValue;
}
