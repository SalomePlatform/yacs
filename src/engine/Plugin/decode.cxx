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

#include "decode.hxx"

#include <iostream>

// Decoder
void Decoder::echo(Solution &sol)
{
    std::vector<double>     *tmp;
    std::vector<double>::iterator   j;
    
    tmp = decode(*sol.param);
    for (j=tmp->begin(); j!= tmp->end(); j++)
        std::cout << *j << "  " ;
    std::cout << std::endl << "->";
    delete tmp;
    tmp = sol.obj;
    for (j=tmp->begin(); j!= tmp->end(); j++)
        std::cout << "  " << *j ;
    std::cout << std::endl ;
    
    return;
}

// LinearDecoder
LinearDecoder::LinearDecoder(std::vector<std::pair<double,double> > &borne)
{
    int     i;

    size = borne.size();
    a.resize(size);
    b.resize(size);
    for (i=0; i < size; i++) {
        a[i] = borne[i].second - borne[i].first;
        b[i] = borne[i].first;
    }
}

// representation interne -> representation physique
std::vector<double> *LinearDecoder::decode(std::vector<double> &in)
{
    std::vector<double>     *ret;
    long            i;

    ret = new std::vector<double>(size);
    for (i=0; i<size; i++)
        (*ret)[i] = a[i]*in[i] + b[i];
    return ret;
}

// representation physique -> representation interne
std::vector<double> *LinearDecoder::code(std::vector<double> &in)
{
    std::vector<double>     *ret;
    long            i;

    ret = new std::vector<double>(size);
    for (i=0; i<size; i++)
        (*ret)[i] = (in[i] - b[i]) / a[i];
    return ret;
}

