#include "vecteur.hxx"

vecteur::vecteur(long n)
{
  xx = new double[n];
  nn = n;
}

vecteur::~vecteur()
{
  delete [] xx;
}

