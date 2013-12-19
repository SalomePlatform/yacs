#include <cstring>
#include <iostream>
#include <fstream>

#include "tools.hxx"

using namespace std ;

/** Test if two files are identical. 
 * \param fileName Name of the first file
 * \param otherFileName Name of the other file
 * \return 0 if files are equal.
 */
int areFilesEqual(const std::string & fileName, std::string otherFileName)
{
    // Get content of the files 
    string str1("");
    const char* f1 = fileName.c_str();
    std::ifstream ifs1 (f1, ios::in);
    if (ifs1) 
    {
        string ligne;
        while(getline(ifs1, ligne))
        {
            str1 +=  ligne ;
            str1 += "'\n'";
        }
        ifs1.close();
    } 
    //
    string str2("");
    const char* f2 = otherFileName.c_str();
    std::ifstream ifs2 (f2);
    if (ifs2) 
    {
        string ligne;
        while(getline(ifs2, ligne))
        {
            str2 +=  ligne ;
            str2 += "'\n'";
        }    
        ifs2.close();
    }  
    // Compare
    int cmp = str1.compare(str2);
    return cmp; 
}