// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-04-03.11.38.54
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      simplexe via salome evenementiel
// 
//___________________________________________________________________

#include "sasimpl.hxx"

#include "saconst.h"

#include "solution.hxx"

SalomeTest::SalomeTest(Superviseur &t)
{
    super = &t;
    // distribution
    dst = (SalomeEventLoop *) NULL;
    dec = (LinearDecoder *) NULL;
    mtr = (Maestro *) NULL;
    // swarm
    solv = (Simplex *) NULL;

}

SalomeTest::~SalomeTest(void)
{
    // distribution
    delete dst;
    delete dec;
    delete mtr;
    // swarm
    delete solv;
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
    // distribution
    dst = new SalomeEventLoop(*super);
    dec = new LinearDecoder(dom);
    mtr = new Maestro((Decoder &) *dec, (Critere *) NULL, (Distrib &) *dst);
    // swarm
    solv = new Simplex(NBNODE, NBGENE, *mtr);
    
    solv->setStop(NBEVAL);
}

void SalomeTest::start(void)
{
    solv->start();
}

void SalomeTest::next(void)
{
    int     rien;
    rien = solv->next();
}

void SalomeTest::finish(void)
{
    Solution        *res;

    solv->finish();
    res = solv->solution();
    dec->echo(*res);
}

