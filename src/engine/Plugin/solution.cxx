// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-02-22.15.08.41
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      individu solution
// 
//___________________________________________________________________

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

