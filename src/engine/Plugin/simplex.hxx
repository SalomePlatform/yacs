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

#ifndef __SIMPLEX__
#define __SIMPLEX__

#include <vector>

#include "solution.hxx"
#include "point.hxx"
#include "maestro.hxx"

class Simplex {
    protected:
        long    size, nbin, maxe, nbeval;
        std::vector<Solution *>     simplx;
        std::vector<Point *>        work;
        Maestro     *calc;

        Solution *add(Solution *);
        std::vector<double> *minimum(void);
        std::vector<double> *barycentre(void);

    public :
        Simplex(long, long, Maestro &);
        ~Simplex(void);
        void setStop(long);
        void start(void);
        int next(void);
        void finish(void);
        void solve(void);
        Solution *solution(void);
};

#endif
