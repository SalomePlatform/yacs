// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-02-21.09.46.36
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      différent tirage aleatoire
// 
//___________________________________________________________________

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
