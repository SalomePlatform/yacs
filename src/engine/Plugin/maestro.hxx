// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-02-26.04.25.36
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      gestionnaire des calculs
// 
//___________________________________________________________________

#ifndef __MAESTRO__
#define __MAESTRO__

#include <vector>

#include "decode.hxx"
#include "distrib.hxx"
#include "critere.hxx"

class Maestro {
    private :
        Decoder     *dom;
        Distrib     *dist;
        Critere     *crit;

    public :
        Maestro(Decoder &, Critere *, Distrib &);
        void put(long, std::vector<double> &);
        std::vector<double> *get(long *);
};

#endif

