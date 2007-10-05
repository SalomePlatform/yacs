// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-04-03.11.39.15
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      simplexe via salome evenementiel
// 
//___________________________________________________________________


#ifndef __SASIMPL__
#define __SASIMPL__

#include <string>

#include "decode.hxx"
#include "salomevent.hxx"
#include "maestro.hxx"
#include "simplex.hxx"

#include "salomesup.hxx"

class SalomeTest {
    protected :
        Superviseur    *super;
        // distribution
        SalomeEventLoop  *dst;
        LinearDecoder  *dec;
        Maestro  *mtr;
        // swarm
        Simplex  *solv;
        
    public :
        SalomeTest(Superviseur &);
        ~SalomeTest(void);
        void readFromFile(std::string);
        void start(void);
        void next(void);
        void finish(void);
};

#endif

