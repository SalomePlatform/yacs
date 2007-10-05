// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-03-22.23.18.15
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      distribution via superviseur salome
// 
//___________________________________________________________________


#ifndef __SALOMEVENT__
#define __SALOMEVENT__

#include "Pool.hxx"

#include "distrib.hxx"

class SalomeEventLoop : Distrib {
    protected :
        YACS::ENGINE::Pool        *super;
    public :
        SalomeEventLoop(YACS::ENGINE::Pool *);
        virtual void put(long, std::vector<double> &);
        virtual std::vector<double> *get(long *);
};


#endif        
    
