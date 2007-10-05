// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-03-22.23.17.58
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


#include "salomevent.hxx"

#include "Any.hxx"

using namespace YACS::ENGINE;

SalomeEventLoop::SalomeEventLoop(Pool *sup)
{
    super = sup;
}

void SalomeEventLoop::put(long i, std::vector<double> &cal)
{
    SequenceAny *tmp = SequenceAny::New(cal);
    super->pushInSample(i, (Any *)tmp);
}

std::vector<double> *SalomeEventLoop::get(long *id)
{
    SequenceAny     *tmp;
    std::vector<double>     *ret;
    unsigned int     nb, i;
    
    *id = super->getCurrentId();
    tmp = (SequenceAny *) super->getCurrentOutSample();
    nb = tmp->size();
    ret = new std::vector<double>(nb);
    for (i=0; i<nb; i++)
        (*ret)[i] = (*tmp)[i]->getDoubleValue();
    tmp->decrRef();
    
    return ret;
}

