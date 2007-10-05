// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-02-22.15.08.27
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


#ifndef __SOLUTION__
#define __SOLUTION__

#include <vector>

class Solution {
    public :
        std::vector<double>     *param, *obj;

        Solution(std::vector<double> &, std::vector<double> &);
        ~Solution(void);
        void echo(void);
};

#endif
