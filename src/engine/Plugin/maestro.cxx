// Copyright (C) 2006-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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


