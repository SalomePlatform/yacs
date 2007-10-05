// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-03-30.13.46.22
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      version distribue du simplex de nelder mead
// 
//___________________________________________________________________

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
