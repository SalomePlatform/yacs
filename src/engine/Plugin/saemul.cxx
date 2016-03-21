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

#include "saemul.hxx"

#include "saconst.h"

#include "math.h"

Emulator::Emulator(void)
{
    solver = new SalomeTest((Superviseur &) *this);
}

Emulator::~Emulator(void)
{
    delete solver;
    destroyAll();
}

void Emulator::destroyAll(void)
{
    std::pair<long,std::vector<double> *>    pa;
    
    while (! q.empty()) {
        pa = q.front();
        q.pop();
        delete pa.second;
    }
}

void Emulator::push(long id, std::vector<double> &cal)
{
    std::pair<long, std::vector<double> *> *tmp;

    tmp = new std::pair<long, std::vector<double> *>(id, &cal);
    q.push(*tmp);
    delete tmp;
}

long Emulator::getCurrentId(void)
{   
    std::vector<double>     *cal;
    std::pair<long,std::vector<double> *>    pa;
    
    pa = q.front(); q.pop();
    cal = pa.second;
    out = eval(*cal);
    delete cal;
    return pa.first;
}

std::vector<double> *Emulator::getCurrentOut(void)
{
    return out;
}

std::vector<double> *Emulator::eval(std::vector<double> &x)
{   
    std::vector<double>     *res;
    res = new std::vector<double>(1);
#ifdef TRIPOD
    //Tripod
    double      x1, x2;
    
    x1 = x[0]; x2 = x[1];

    if (x2 < 0) 
        (*res)[0] = fabs(x1) + fabs(x2+50);
    else {
        (*res)[0] = fabs(x2-50);
        if (x1 < 0)
            (*res)[0] += 1 + fabs(x1+50);
        else
            (*res)[0] += 2 + fabs(x1-50);
    }
#endif
#ifdef ROSENBROCK
    // rosenbrock
    double      x1, x2;

    x1 = x[0] ; x2 = x1*x1 - x[1]; x1 -= 1;
    (*res)[0] = x1*x1 + 100*x2*x2 ;
#endif
#ifdef GRIEWANK
    double      x1, x2, y;
    int     i;

    x1 = 0.0; x2 = 1.0;
    for (i=0; i<NBGENE; i++) {
        y = 3*x[i] - 100;
        x1 += y*y;
        x2 *= cos(y/sqrt(i+1));
    }
    (*res)[0] = x1 / 4000 - x2 + 1;
#endif
    return res;
}

void Emulator::run(void)
{
    solver->readFromFile("caca");
    solver->start();
    while (! q.empty())
        solver->next();
    solver->finish();
}

