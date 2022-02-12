/*
    Created by: Hector Soto
    Description: Implementation of DirectoryHandler.h.
*/

#include "DirectoryList.h"
#include "General.h"
#include <cstring>

using namespace std;

/*
    CONSTRUCTOR
*/
DirectoryList::DirectoryList()
{

}

/*
    DESTRUCTOR
*/
DirectoryList::~DirectoryList()
{
    Clear(); //Deletes entire list.
}

/*
    Displays the list of strings.
*/
void DirectoryList::Display()
{
    int StrNum = 1;
    StringNode *current = Head;
    while(current)
    {
        cout << StrNum << ". " << current->data << endl;
        ++StrNum;
        current = current->next;
    }
    return;
}

/*
    Recursive function for Clear();
*/
void DirectoryList::ClearRecursive(StringNode *current)
{
    if(current)
    {
        ClearRecursive(current->next);
        delete current;
    }
}

/*
    Clears the list.
*/
void DirectoryList::Clear()
{
    if(Head)
    {
        ClearRecursive(Head);
        Head = nullptr;
    }
    return;
}

/*
    Fills list with names of files in current directory.
    Will clear the list first.
    If Suffix is not an empty string, file must end in Suffix
    to be added to list.
*/
void DirectoryList::LoadDirectory(const char *Suffix)
{
    Clear(); //Clear the list.
    DIR *dir;
    struct dirent *ent;
    string FileName;
    if((dir = opendir (".")) != NULL)
    {
        while((ent = readdir (dir)) != NULL)
        {
            FileName = ent->d_name; //Current file name.
            if(FileName.length() >= 4) //If the file is at least 4 characters long.
            {
                if(Suffix) //If Suffix exists.
                {
                    if(FileName.find(Suffix, FileName.length() - strnlen(Suffix, MAX_STRLEN)) == (FileName.length() - strnlen(Suffix, MAX_STRLEN))) //If the file ends with Suffix.
                    {
                        Add(FileName); //Adds file name to list.
                    }
                }
                else
                {
                    Add(FileName);
                }
            }
        }
        closedir (dir);
    }
    else
    {
        cout << "Error: Failed to open directory!" << endl;
    }
    return;
}

/*
    Recursive function for Add().
*/
void DirectoryList::AddRecursive(StringNode *current, string &Name)
{
    if(current->next) //If next node exists
    {
        AddRecursive(current->next, Name);
    }
    else
    {
        current->next = new StringNode;
        current->next->data = Name;
        ++Size;
    }
    return;
}

/*
    Adds a string to the list. Returns true on success.
*/
bool DirectoryList::Add(string &Name)
{
    if(!Name.empty()) //If we were given a non-empty string.
    {
        if(Head)
        {
            AddRecursive(Head, Name);
        }
        else
        { //
            Head = new StringNode;
            Head->data = Name;
            ++Size;
        }
        return true;
    }
    return false;
}

/*
    Obtains nth (number) string. Returns true on success.
    String reference will hold new string.
*/
bool DirectoryList::GetString(int number, string &Buffer)
{
    StringNode *current = Head;
    int StrNum = 1;
    while(current)
    {
        if(StrNum == number) //We got to nth string.
        {
            Buffer = current->data;
            return true; //Success.
        }
        ++StrNum;
        if(StrNum > number) //The list isn't that long.
        {
            break; //Exit while loop.
        }
        current = current->next;
    }
    return false; //Failure: Failed to obtain nth string.
}

int DirectoryList::GetSize()
{
    return Size;
}
