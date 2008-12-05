//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "solution.hxx"

#include <iostream>

Solution::Solution(std::vector<double> &par, std::vector<double> &crt)
{
    param = &par;
    obj = &crt;
}

Solution::~Solution(void)
{
    delete param;
    delete obj;
}

void Solution::echo(void)
{
    int     i;

    for (i=0; i<param->size(); i++)
        std::cout << (*param)[i] << "  ";
    std::cout << std::endl << "->" ;
    for (i=0; i<obj->size(); i++)
        std::cout << "  " << (*obj)[i];
    std::cout << std::endl;
}

