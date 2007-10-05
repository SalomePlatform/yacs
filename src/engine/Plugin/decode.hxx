// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-02-21.13.38.13
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      decodage du genotype
// 
//___________________________________________________________________

#ifndef __DECODE__
#define __DECODE__

#include <vector>
#include <utility>

#include "solution.hxx"

class Decoder {
    protected:
        long        size;
    public:
        virtual std::vector<double> *code(std::vector<double> &) = 0;
        virtual std::vector<double> *decode(std::vector<double> &) = 0;
        void echo(Solution &);
};

class LinearDecoder : public Decoder {
    private :
        std::vector<double>     a,b;
    public :
        LinearDecoder(std::vector<std::pair<double, double> > &);
        virtual std::vector<double> *code(std::vector<double> &);
        virtual std::vector<double> *decode(std::vector<double> &);
};

#endif
