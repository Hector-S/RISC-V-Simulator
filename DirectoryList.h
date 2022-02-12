#ifndef FILEHANDLER_H_INCLUDED
#define FILEHANDLER_H_INCLUDED

/*
    Created by: Hector Soto
    Description: For loading files from the directory or making a
    String list of files from the directory.
*/

#include <fstream> //For files.
#include <string> //For strings.
#include <iostream> //For IO.
#include <dirent.h> //For accessing the directory.

#define MAX_STRLEN 100

class DirectoryList
{
    public:
        DirectoryList(); //CONSTRUCTOR
        ~DirectoryList(); //DESTRUCTOR
        void Clear(); //Clears the list.
        void Display(); //Displays strings from last loaded directory in list format.
        void LoadDirectory(const char *Suffix); //Fills list with names of files in current directory.
        bool GetString(int number, std::string &Buffer); //String reference will hold nth string in list.
        int GetSize(); //Returns size of list.
    private:
        struct StringNode
        {
            std::string data;
            StringNode *next = nullptr;
        };
        StringNode *Head = nullptr; //List starts empty.
        int Size = 0; //Total amount of strings in the list.
        void AddRecursive(StringNode *current, std::string &Name);
        void ClearRecursive(StringNode *current);
        bool Add(std::string &Name); //Adds a string to the list. Given string will be unaltered.
};

#endif // FILEHANDLER_H_INCLUDED
