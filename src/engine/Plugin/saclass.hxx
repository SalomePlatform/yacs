// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-03-22.23.34.41
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      class pour test salome
// 
//___________________________________________________________________

#ifndef __SACLASS__
#define __SACLASS__

#include <string>

#include "aleas.hxx"
#include "decode.hxx"
#include "salomevent.hxx"
#include "topologie.hxx"
#include "movement.hxx"
#include "maestro.hxx"
#include "mono.hxx"
#include "solution.hxx"

#include "salomesup.hxx"

class SalomeTest {
    protected :
        Superviseur    *super;
        SpherePositif  *rnd1;
        Sphere      *rnd2;
        Traditionnel  *st1;
        Pivot  *st2;
        // distribution
        SalomeEventLoop  *dst;
        LinearDecoder  *dec;
        Maestro  *mtr;
        // swarm
        MonoSwarm  *swrm;
        
    public :
        SalomeTest(Superviseur &);
        ~SalomeTest(void);
        void readFromFile(std::string);
        void start(void);
        void next(void);
        void finish(void);
};

#endif

