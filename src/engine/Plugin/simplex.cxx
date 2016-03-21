// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
