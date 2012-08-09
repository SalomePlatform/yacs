#include "alglin.hxx"

extern "C" {
  double * valloc(long n);
  void vfree(double *x);
  void addvec_(double *C, double *A, double *B, long *n);
  double prdscl_(double *A, double *B, long *n);
}

void   alglin::addvec(vecteur *C, vecteur *A, vecteur *B) {
  long n = A->n();
  addvec_(C->x(), A->x(), B->x(), &n);
}

double alglin::prdscl(vecteur *A, vecteur *B) {
  long n = A->n();
  return prdscl_(A->x(), B->x(), &n);
}

vecteur * alglin::create_vector(long n) {
  return new vecteur(n);
}

void   alglin::destroy_vector(vecteur *V) {
  delete V;
}
