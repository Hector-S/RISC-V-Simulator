/*
    Created By: Hector Soto
    Uses SDL2, SDL2_image and SDL_ttf.
*/

#ifndef RVS_GUI_H_INCLUDED
#define RVS_GUI_H_INCLUDED

#include <string> //For strings.

#include <SDL.h> //Basic SDL
#include <SDL_ttf.h> //For text.
#include <SDL_image.h> //For image loading.

#define GUI_FONT "Media/consola.ttf" //Font used in gui.
#define GUI_FONT_SIZE 116
#define GUI_FONT_SCALE 4 //Will effective divide size of font by 4 while keeping texture detail.
#define GUI_WIDTH 960 //Default window width.
#define GUI_HEIGHT 960 //Default window height.
#define HISTORY_MAX 999 //Maximum number of previous register states that can be saved.

std::string DecodeInstruction(uint32_t Instruction);

class UI_BOX
{
    public:
        UI_BOX(); //CONSTRUCTOR
        ~UI_BOX(); //DESTRUCTOR
        void Render(SDL_Renderer *Renderer); //Will render the UI box.
        void SetText(std::string NewText);
        bool Inside(int X, int Y); //Returns true if X and Y are inside the box.

        int x = 0, y = 0, w = 0, h = 0; //Position coordinates, width, and height.
        int tw = 0, th = 0; //Text width and height.
        TTF_Font **Font; //Pointer to a Font pointer.
        SDL_Texture *Text = nullptr;
    private:
};

/*
    To hold data before an instruction was evoked.
*/
struct HistoryNode
{
    int *RegisterData = nullptr; //Pointer to 33-element array of ints.
    HistoryNode *Next = nullptr;
    HistoryNode *Previous = nullptr; //Pointer to previous node.
};

/*
    Holds history of past register states so we can reverse the simulation;
    This is really a linked list of integer pointers.
    Assumes no errors from allocating memory via new.
*/
class History
{
    public:
        History(); //CONSTRUCTOR
        ~History(); //DESTRUCTOR
        void Push(int *NewRegisterData); //Push register data into history.
        int *Pop(); //Returned array should be freed.
        int *PopEnd(); //Remove the current tail from the list.
        int Size = 0; //Size of the history.
    private:
        HistoryNode *Head = nullptr;
        HistoryNode *Tail = nullptr;
};

#endif // RVS_GUI_H_INCLUDED
