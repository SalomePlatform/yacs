// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef __POINT__
#define __POINT__

#include <vector>

#include "aleas.hxx"
#include "solution.hxx"

class Point {
    private:
        int     size, etat;
        double      epsil;
        Cube    *rnd;
        Solution    *start;
        std::vector<double>     *courant, *baryc, *minim;

        std::vector<double> *symetrique(std::vector<double> &, std::vector<double> &);
        std::vector<double> *milieu(std::vector<double> &, std::vector<double> &);

    public:
        Point(long);
        ~Point(void);
        void reinit(void);
        void mute(Solution &, std::vector<double> &, std::vector<double> &);
        Solution *inform(std::vector<double> &);
        std::vector<double> *next(void);
};

#endif

