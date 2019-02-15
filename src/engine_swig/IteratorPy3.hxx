// Copyright (C) 2006-2019  CEA/DEN, EDF R&D
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
// Author: Nicolas GEIMER (EDF)

#ifndef __ITERATORPY3_HXX__
#define __ITERATORPY3_HXX__

template <typename U> class StopIteratorPy3 {};

template <typename U> class IteratorPy3 {
    public:
        IteratorPy3( typename std::map<std::string,U*>::iterator _cur, typename std::map<std::string,U*>::iterator _end) : cur(_cur), end(_end) {}
        ~IteratorPy3() {}
        IteratorPy3* __iter__()
        {
            return this;
        }
        typename std::map<std::string,U*>::iterator cur;
        typename std::map<std::string,U*>::iterator end;
};

#endif /* __ITERATORPY3_HXX__ */

