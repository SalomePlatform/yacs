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

#include "saclass.hxx"

#include <utility>

#include "topologie.hxx"

#include "saconst.h"



SalomeTest::SalomeTest(Superviseur &t)
{
    super = &t;
    rnd1 = (SpherePositif *) NULL;
    st1 = (Traditionnel *) NULL;
    rnd2 = (Sphere *) NULL;
    st2 = (Pivot *) NULL;
    // distribution
    dst = (SalomeEventLoop *) NULL;
    dec = (LinearDecoder *) NULL;
    mtr = (Maestro *) NULL;
    // swarm
    swrm = (MonoSwarm *) NULL;

}

SalomeTest::~SalomeTest(void)
{
    delete rnd1;
    delete rnd2;
    delete st1;
    delete st2;
    // distribution
    delete dst;
    delete dec;
    delete mtr;
    // swarm
    delete swrm;
}

void SalomeTest::readFromFile(std::string rien)
{
    std::vector<std::pair<double, double> > dom(NBGENE);
    long    i;

    // domaine de recherche
    for (i=0; i<NBGENE; i++) {
        dom[i].first = BORNEMIN;
        dom[i].second = BORNEMAX;
    }
    // topologie
    Plan  top(COTE,COTE);
    // operateur stochastique
    rnd1 = new SpherePositif(NBGENE);
    st1 = new Traditionnel(NBGENE, *rnd1);
    rnd2 = new Sphere(NBGENE);
    st2 = new Pivot(NBGENE, *rnd2);
    // distribution
    dst = new SalomeEventLoop(*super);
    dec = new LinearDecoder(dom);
    mtr = new Maestro((Decoder &) *dec, (Critere *) NULL, (Distrib &) *dst);
    // swarm
    swrm = new MonoSwarm(PLAN, NBGENE, (Topologie &) top, (Movement &) *st1, (Movement &) *st1, *mtr);
    
    swrm->setStop(NBEVAL);
}

void SalomeTest::start(void)
{
    swrm->start();
}

void SalomeTest::next(void)
{
    int     rien;
    rien = swrm->next();
}

void SalomeTest::finish(void)
{
    Solution        *res;

    swrm->finish();
    res = swrm->solution();
    dec->echo(*res);
}

