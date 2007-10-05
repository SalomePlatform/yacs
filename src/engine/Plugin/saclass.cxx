// -*- C++ -*-
// -*- coding: latin_1 -*-
//
//    File
//      creation : 2007-03-22.23.34.31
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      class pour test salome
// 
//___________________________________________________________________


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

