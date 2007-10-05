// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-02-26.03.40.35
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


#include "local.hxx"

Local::Local(UserFun &f)
{
    fun = &f;
}

Local::~Local(void)
{
    std::pair<long,std::vector<double> *>    pa;
    
    while (! q.empty()) {
        pa = q.front();
        q.pop();
        delete pa.second;
    }
}

void Local::put(long id, std::vector<double> &cal)
{
    std::pair<long, std::vector<double> *> *tmp;

    tmp = new std::pair<long, std::vector<double> *>(id, &cal);
    q.push(*tmp);
    delete tmp;
}

std::vector<double> *Local::get(long *id)
{
    std::vector<double>     *cal, *res;
    std::pair<long,std::vector<double> *>    pa;
    
    pa = q.front(); q.pop();
    *id = pa.first; cal = pa.second;
    res = fun->eval(*cal);
    delete cal;
    return res;
}

