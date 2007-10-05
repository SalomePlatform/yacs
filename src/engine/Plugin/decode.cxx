// --- C++ ---
// --- coding: latin_1 ---
//
//    File
//      creation : 2007-02-21.13.37.58
//      revision : $Id$
//
//    Copyright © 2007 Commissariat à l'Energie Atomique
//      par Gilles ARNAUD (DM2S/SFME/LETR)
//        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
//        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
//        Gilles.Arnaud@cea.fr
// 
//    Object
//      decodage deu genotype
// 
//___________________________________________________________________

#include "decode.hxx"

#include <iostream>

// Decoder
void Decoder::echo(Solution &sol)
{
    std::vector<double>     *tmp;
    std::vector<double>::iterator   j;
    
    tmp = decode(*sol.param);
    for (j=tmp->begin(); j!= tmp->end(); j++)
        std::cout << *j << "  " ;
    std::cout << std::endl << "->";
    delete tmp;
    tmp = sol.obj;
    for (j=tmp->begin(); j!= tmp->end(); j++)
        std::cout << "  " << *j ;
    std::cout << std::endl ;
    
    return;
}

// LinearDecoder
LinearDecoder::LinearDecoder(std::vector<std::pair<double,double> > &borne)
{
    int     i;

    size = borne.size();
    a.resize(size);
    b.resize(size);
    for (i=0; i < size; i++) {
        a[i] = borne[i].second - borne[i].first;
        b[i] = borne[i].first;
    }
}

// representation interne -> representation physique
std::vector<double> *LinearDecoder::decode(std::vector<double> &in)
{
    std::vector<double>     *ret;
    long            i;

    ret = new std::vector<double>(size);
    for (i=0; i<size; i++)
        (*ret)[i] = a[i]*in[i] + b[i];
    return ret;
}

// representation physique -> representation interne
std::vector<double> *LinearDecoder::code(std::vector<double> &in)
{
    std::vector<double>     *ret;
    long            i;

    ret = new std::vector<double>(size);
    for (i=0; i<size; i++)
        (*ret)[i] = (in[i] - b[i]) / a[i];
    return ret;
}

