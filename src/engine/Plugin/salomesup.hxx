// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-03-23.03.16.40
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      interface du superviseur salome
// 
//___________________________________________________________________



#ifndef __SALOMESUP__
#define __SALOMESUP__

class Superviseur {
    public :
        virtual void destroyAll(void) = 0;
        virtual void push(long id, std::vector<double> &cal) = 0;
        virtual long getCurrentId(void) = 0;
        virtual std::vector<double> *getCurrentOut(void) = 0;
};

#endif

