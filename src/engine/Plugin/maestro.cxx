// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-02-26.04.25.45
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

#include "maestro.hxx"

Maestro::Maestro(Decoder &dm, Critere *crt, Distrib &dst)
{
    dom = &dm;
    dist = &dst;
    crit = crt;
}

void Maestro::put(long id, std::vector<double> &inp)
{
    std::vector<double>     *tmp;
    
    tmp = dom->decode(inp);
    dist->put(id, *tmp);
    return;
}

std::vector<double> *Maestro::get(long *id)
{
    std::vector<double>     *res, *crt;
    
    res = dist->get(id);
    if (crit) {
        crt = crit->eval(*res);
        delete res;
    }
    else
        crt = res;
    return crt;
}


