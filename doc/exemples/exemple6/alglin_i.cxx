#include "alglin_i.hxx"
#include <iostream>

AlgLin_i::AlgLin_i()
{
}

AlgLin_i::~AlgLin_i()
{
}

void AlgLin_i::addvec(Distant::vecteur_out C,
		      const Distant::vecteur& A, 
		      const Distant::vecteur& B)
{
  long i, n = A.length();

  vecteur A_(n);
  vecteur B_(n);
  vecteur C_(n);

  double *xA = A_.x();
  double *xB = B_.x();
  double *xC = C_.x();

  for (i=0; i<n; i++) {
    xA[i] = A[i];
    xB[i] = B[i];
  }

  A_interne.addvec(&C_, &A_, &B_);

  C = new Distant::vecteur;
  C->length(n);
  for (i=0; i<n; i++) {
    (*C)[i] = xC[i];
  }
}

CORBA::Double AlgLin_i::prdscl(const Distant::vecteur& A, 
			       const Distant::vecteur& B)
{
  long i, n = A.length();

  vecteur A_(n);
  vecteur B_(n);

  double *xA = A_.x();
  double *xB = B_.x();

  for (i=0; i<n; i++) {
    xA[i] = A[i];
    xB[i] = B[i];
  }

  return A_interne.prdscl(&A_, &B_);  
}

