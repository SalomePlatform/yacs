%module AlgLinModule

%{
#include "alglin.hxx"
%}

class alglin {
public:
  alglin();
  ~alglin();
  void      addvec(vecteur *C, vecteur *A, vecteur *B);
  double    prdscl(vecteur *A, vecteur *B);
  vecteur * create_vector(long n);
  void      destroy_vector(vecteur *V);
};

