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

