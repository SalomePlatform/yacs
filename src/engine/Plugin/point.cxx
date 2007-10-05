// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-03-30.16.58.42
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


#include "point.hxx"

#include <iostream>

#define START   0
#define FIRST   1
#define GOOD    2
#define SOGOOD  3
#define TOOGOOD 4
#define BAD     5
#define TOOBAD  6
#define SOBAD   7
#define BADEST  8

#define SEUIL   1.0e-10

Point::Point(long sz)
{
    size = sz;
    etat = START;
    epsil = SEUIL;
    rnd = new Cube(size);
    start = (Solution *) NULL;
    courant = (std::vector<double> *) NULL;
    baryc = (std::vector<double> *) NULL;
    minim = (std::vector<double> *) NULL;
}

Point::~Point(void)
{
    delete rnd;
    if (courant)
        delete courant;
    if (start) {
        delete start;
        delete baryc;
        delete minim;
    }
}
    
Solution *Point::inform(std::vector<double> &obj)
{
    Solution        *res, *tmp;

    res = (Solution *) NULL;
    switch (etat) {
        case START :
#ifdef SHOW
            std::cout << "@" ;
#endif
            res = new Solution(*courant, obj);
            break;
        case FIRST :
            if (obj[0] > (*start->obj)[0]) {
#ifdef SHOW
                std::cout << "-" ;
#endif
                etat = BAD ;
                delete courant;
                delete &obj;
            }
            else {
#ifdef SHOW
                std::cout << "+" ;
#endif
                etat = GOOD ;
                tmp = start;
                start = new Solution(*courant, obj);
                delete tmp;
            }
            break;
        case GOOD :
            if (obj[0] > (*start->obj)[0]) {
#ifdef SHOW
                std::cout << "P" ;
#endif
                etat = SOGOOD;
                delete courant;
                delete &obj;
                res = start;
                delete baryc;
                delete minim;
                start = (Solution *) NULL;
            }
            else {
#ifdef SHOW
                std::cout << "p" ;
#endif
                etat = TOOGOOD;
                res = new Solution(*courant, obj);
            }
            break;
        case BAD :
            if (obj[0] > (*start->obj)[0]) {
#ifdef SHOW
                std::cout << "M" ;
#endif
                etat = TOOBAD;
                delete courant;
                delete &obj;
            }
            else {
#ifdef SHOW
                std::cout << "m" ;
#endif
                etat = SOBAD;
                res = new Solution(*courant, obj);
            }
            break;
        case TOOBAD:
#ifdef SHOW
            std::cout << "X" ;
#endif
            etat = BADEST;
            res = new Solution(*courant, obj);
            break;
        default :
            std::cout << "pbl inform" << std::endl ;
            break;
    }
    return res;
}

void Point::mute(Solution &pt, std::vector<double> &bary, std::vector<double> &minm)
{
    std::vector<double>     *tmp;
    
    if (start) {
        delete baryc;
        delete minim;
        delete start;
    }
    start = &pt;
    baryc = &bary;
    minim = &minm;
    etat = FIRST;
}
            
void Point::reinit(void)
{
    etat = START;
}

std::vector<double> *Point::next(void)
{
    double      d, dd;
    int     i;

    switch (etat) {
        case START :
            courant = rnd->gen();
            break;
        case FIRST :
            courant = symetrique(*start->param, *baryc);
            break;
        case GOOD :
            courant = symetrique(*baryc, *start->param);
            break;
        case BAD :
            courant = milieu(*baryc, *start->param);
            break;
        case TOOBAD :
            courant = milieu(*minim, *start->param);
            break;
        default:
            // raise
            std::cout << "pbl next" << std::endl ;
    }
    if (baryc) {
        d=0.0;
        for (i=0; i<size; i++) {
            dd = (*courant)[0] - (*baryc)[0];
            d += dd*dd;
        } 
    }
    else
        d=1.0;
    if (d < epsil) {
        //delete courant;
        return (std::vector<double>  *) NULL;
    }
    else
        return courant;
}

std::vector<double> *Point::symetrique(std::vector<double> &pt, std::vector<double> &centr)
{
    long        i;
    double      coef, tmp;
    std::vector<double>     *res;

    // bounded coef
    coef = 1.0;
    for (i=0; i<size; i++) {
        tmp = (centr[i]-pt[i] > 0.0) ?
                (1.0 - centr[i]) / (centr[i] - pt[i]) :
                centr[i] / (pt[i] - centr[i]) ;
        coef = (coef < tmp) ? coef : tmp ;
    }
    //
    res = new std::vector<double>(size);
    for (i=0; i<size; i++)
        (*res)[i] = centr[i] + coef * (centr[i] - pt[i]);
    return res;
}

std::vector<double> *Point::milieu(std::vector<double> &un, std::vector<double> &deux)
{
    long        i;
    std::vector<double>     *res;
    
    res = new std::vector<double>(size);
    for (i=0; i<size; i++)
        (*res)[i] = (un[i] + deux[i])/2.0;
    return res;
}


    
