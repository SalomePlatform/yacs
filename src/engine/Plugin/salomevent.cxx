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

