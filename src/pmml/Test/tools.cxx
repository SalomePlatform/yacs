// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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