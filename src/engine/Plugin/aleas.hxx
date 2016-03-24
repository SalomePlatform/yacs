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

#ifndef __ALEAS__
#define __ALEAS__

#include <vector>


class Aleatoire {
    private :
    protected :
        long size;
    public :
        Aleatoire(long);
        virtual std::vector<double> *gen(void);
        virtual void fill(std::vector<double> &);
        virtual double tire(void) = 0;
};

class Cube : public Aleatoire {
    public :
        Cube(long s) : Aleatoire(s) {};
        virtual double tire(void);
};

class Normale : public Aleatoire {
    public :
        Normale(long s) : Aleatoire(s) {};
        virtual double tire(void);
};

class NormalePositif : public Aleatoire {
    public :
        NormalePositif(long s) : Aleatoire(s) {};
        virtual double tire(void);
};

class Sphere : public Normale {
    public:
        Sphere(long s) : Normale(s) {};
        virtual void fill(std::vector<double> &);
};

class SpherePositif : public Sphere {
    public:
        SpherePositif(long s) : Sphere(s) {};
        virtual void fill(std::vector<double> &);
};

#endif
