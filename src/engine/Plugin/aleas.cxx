// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-02-21.09.50.46
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      tirage aleatoire
// 
//___________________________________________________________________

#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

// #include "mt19937ar.h
extern void init_by_array(unsigned long [], int);
extern double genrand_real1(void);
extern double genrand_real3(void);

#include "aleas.hxx"

#define NOALEAS 

static void initrand(void)
{
     static long done=0;
     unsigned long vec[]={
             31081996, 21012006, 17921963, 0,
             11101998, 2112003, 25111964, 0
         };

     if (! done) {
#ifdef NOALEAS
         vec[3] = vec[7] = 2082007;
#else
         vec[3] = (unsigned long) getpid();
         vec[7] = (unsigned long) time(NULL);
#endif
         init_by_array(vec, 8);
         done += 1;
     }
} 
// utilitaire
/*static void initrand(void)
{
    static long done=0;
    unsigned long vec[]={
            31081996, 21012006, 17921963, 0,
            11101998, 2112003, 25111964, 0
        };

    if (! done) {
        vec[3] = (unsigned long) getpid();
        vec[7] = (unsigned long) time(NULL);
        init_by_array(vec, 8);
        done += 1;
    }
}*/
    
static double randGauss(void)
{
    static double   next;
    static int      flag=0;
    double          v2, d, fac;
    
    if (flag) {
        flag = 0;
        return next;
    }
    else {
        do {
            next = 2.0*genrand_real3() - 1.0;
            v2 = 2.0*genrand_real3() - 1.0;
            d = next*next + v2*v2;
        } while (d >= 1.0 || d == 0.0);
        fac = sqrt(-2.0*log(d)/d);
        next *= fac;
        flag = 1;
        return v2 * fac;
    }

}

// class Aleatoire
Aleatoire::Aleatoire(long sz)
{
    size = sz;
    initrand();
}

void Aleatoire::fill(std::vector<double> &ret)
{
    int     i;

    for (i=0; i<size; i++) ret[i] = tire();
}

std::vector<double> *Aleatoire::gen()
{
    std::vector<double> *ret;
    
    ret = new std::vector<double>(size);
    fill(*ret);
    return ret;
}

// class Cube
double Cube::tire()
{
    return genrand_real1();
}

// class NormalPositif
double NormalePositif::tire()
{
    return randGauss() * 0.25 + 0.5 ;
}

// class Normal
double Normale::tire()
{
    return randGauss();
}

// class Sphere
void Sphere::fill(std::vector<double> &ret)
{
    long    i;
    double  cum, r;

    Normale::fill(ret);
    for (cum=0, i=0; i<size; i++)
        cum += ret[i] * ret[i];
    cum = sqrt(cum);
    r = pow(genrand_real1(), size);
    for (i=0; i<size; i++)
        ret[i] *= cum * r;
}

// class SpherePositif
void SpherePositif::fill(std::vector<double> &ret)
{
    long    i;

    Sphere::fill(ret);
    for (i=0; i<size; i++)
        ret[i] = fabs(ret[i]);
}

