// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-03-23.02.09.11
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      class emulateur superviseur salome
// 
//___________________________________________________________________

#ifndef __SASUPER__
#define __SASUPER__

#include <vector>
#include <queue>
#include <utility>

#include "salomesup.hxx"

#include "saclass.hxx"

class Emulator : public Superviseur {
    protected :
        SalomeTest      *solver;
        std::queue<std::pair<long, std::vector<double> *> >     q;
        std::vector<double> *out;
        
        std::vector<double> *eval(std::vector<double> &x);
    public :
        Emulator(void);
        ~Emulator(void);
        void destroyAll(void);
        void push(long id, std::vector<double> &cal);
        long getCurrentId(void);
        std::vector<double> *getCurrentOut(void);
        void run(void);
};

#endif

