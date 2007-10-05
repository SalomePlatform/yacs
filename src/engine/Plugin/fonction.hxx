// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-02-26.04.24.28
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      prototype de la fonction d evaluation
// 
//___________________________________________________________________


#ifndef __FONCTION__
#define __FONCTION__

#include <vector>

class UserFun {
    public :
        virtual std::vector<double> *eval(std::vector<double> &) = 0;
};

#endif
