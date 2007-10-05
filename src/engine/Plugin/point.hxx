// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-03-30.16.58.50
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      particules du simplex
// 
//___________________________________________________________________


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

