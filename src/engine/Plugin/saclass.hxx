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

#ifndef __SACLASS__
#define __SACLASS__

#include <string>

#include "aleas.hxx"
#include "decode.hxx"
#include "salomevent.hxx"
#include "topologie.hxx"
#include "movement.hxx"
#include "maestro.hxx"
#include "mono.hxx"
#include "solution.hxx"

#include "salomesup.hxx"

class SalomeTest {
    protected :
        Superviseur    *super;
        SpherePositif  *rnd1;
        Sphere      *rnd2;
        Traditionnel  *st1;
        Pivot  *st2;
        // distribution
        SalomeEventLoop  *dst;
        LinearDecoder  *dec;
        Maestro  *mtr;
        // swarm
        MonoSwarm  *swrm;
        
    public :
        SalomeTest(Superviseur &);
        ~SalomeTest(void);
        void readFromFile(std::string);
        void start(void);
        void next(void);
        void finish(void);
};

#endif

