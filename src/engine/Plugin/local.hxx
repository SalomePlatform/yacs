// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-02-26.03.40.22
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      realisation des calculs en local
// 
//___________________________________________________________________


#ifndef __LOCAL__
#define __LOCAL__

#include <vector>
#include <utility>
#include <queue>

#include "distrib.hxx"
#include "fonction.hxx"


class Local : Distrib {
    protected :
        UserFun     *fun;
        std::queue<std::pair<long, std::vector<double> *> >     q;
    public :
        Local(UserFun &);
        ~Local(void);
        virtual void put(long, std::vector<double> &);
        virtual std::vector<double> *get(long *);
};

#endif
