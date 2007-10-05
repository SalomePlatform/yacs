// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-02-26.03.14.22
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      distribution des calculs
// 
//___________________________________________________________________


#ifndef __DISTRIB__
#define __DISTRIB__

#include <vector>
#include <utility>


class Distrib {
    public :
        virtual void put(long, std::vector<double> &) = 0;
        virtual std::vector<double> *get(long *) = 0;
};

#endif

