// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-03-30.13.44.14
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      version distribue du simplex de nelder mead
// 
//___________________________________________________________________


#include "simplex.hxx"

#include <iostream>

#include "aleas.hxx"

Simplex::Simplex(long sz, long nbgen, Maestro &maest)
{
    size = sz;
    nbin = nbgen;
    calc = &maest;
    maxe = 40000;
    nbeval = 0;
}

Simplex::~Simplex(void)
{
    int     i;
    
    if (work.size() == size) {
        for (i=0; i<size; i++)
            delete work[i];
        for (i=0; i<nbin; i++)
            delete simplx[i];
    }
}

void Simplex::setStop(long max)
{
    maxe = max;
}

void Simplex::start(void)
{
    long    i;

    work.resize(size);
    for (i=0; i<size; i++) {
        work[i] = new Point(nbin);
        calc->put(i, *(work[i]->next()));
    }
    nbeval = maxe;
}


int Simplex::next(void)
{
    long        id;
    Solution    *pt, *pnt;
    std::vector<double>     *next, *res;
    
    res = calc->get(&id);
    nbeval--;
    pt = work[id]->inform(*res);
    if (pt) {
        pnt = add(pt);
        if (pnt)
            work[id]->mute(*pnt, *barycentre(), *minimum());
        else
            work[id]->reinit();
    }
    next = work[id]->next();
    if (nbeval > size)
        if (next) 
            calc->put(id, *next);
        //else
        //    nbeval = size - 1;
    return (nbeval > 0);
}

void Simplex::finish(void)
{
    std::cout << maxe - nbeval << std::endl;
    return;
}

Solution *Simplex::solution(void)
{
    return simplx[0];
}

Solution *Simplex::add(Solution *sol)
{
    int     i;
    Solution    *ret, *swp;
    
    if (simplx.size() < nbin) {
        ret = (Solution *) NULL;
        i = simplx.size();
        simplx.push_back(sol);
        while (i && simplx[i]->obj[0] < simplx[i-1]->obj[0]) {
            // TODO swap interne
            swp = simplx[i];
            simplx[i] = simplx[i-1];
            simplx[i-1] = swp;
            i--;
        }
    }
    else if (sol->obj[0] > simplx[nbin-1]->obj[0])
        ret = sol;
    else {
        i = nbin -1;
        ret = simplx[i];
        simplx[i] = sol;
        while (i && simplx[i]->obj[0] < simplx[i-1]->obj[0]) {
            swp = simplx[i];
            simplx[i] = simplx[i-1];
            simplx[i-1] = swp;
            i--;
        }
    }
    return ret;
}

std::vector<double> *Simplex::minimum(void)
{
    return new std::vector<double>(*simplx[0]->param);
}

std::vector<double> *Simplex::barycentre(void)
{
    int     i,j;
    std::vector<double>     *ret;

    ret = new std::vector<double>(nbin);
    for (i=0; i<nbin; i++) {
        (*ret)[i] = 0.0;
        for (j=0; j<nbin; j++)
            (*ret)[i] += (*simplx[j]->param)[i];
        (*ret)[i] /= nbin;
    }
    return ret;
}
